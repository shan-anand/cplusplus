/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file datatypes.cpp
@brief Implementation of SCSI datatypes and serializing/deserializing them.
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
LICENSE: END
*/

/**
 * @file  datatypes.cpp
 * @brief Implementation of scsi datatypes
 */

#include <scsi/datatypes.hpp>
#include <string.h>
#include <sstream>
#include "local.h"

using namespace sid;
using namespace sid::scsi;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// capacity10
//
capacity10::capacity10()
{
  clear();
}

void capacity10::clear()
{
  memset(this, 0, sizeof(capacity10));
}

bool capacity10::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < READ_CAP10_REPLY_LEN )
  {
    if ( _reqSize ) *_reqSize = READ_CAP10_REPLY_LEN;
    return false;
  }

  // Set member variables
  uint32_t last_lba = _ioBuffer.get_32(0);
  if ( 0xffffffff == last_lba )
    throw std::string("Device capacity is too large. Use Capacity16 instead of capacity10");

  this->num_blocks = last_lba + 1;
  this->block_size = _ioBuffer.get_32(4);

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// capacity16
//
capacity16::capacity16()
{
  clear();
}

void capacity16::clear()
{
  memset(this, 0, sizeof(capacity16));
}

bool capacity16::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < READ_CAP16_REPLY_LEN )
  {
    if ( _reqSize ) *_reqSize = READ_CAP16_REPLY_LEN;
    return false;
  }

  // Set member variables
  uint64_t last_lba = _ioBuffer.get_64(0);
  this->num_blocks = last_lba + 1;
  this->block_size = _ioBuffer.get_32(8);
  this->prot_en = _ioBuffer.get_bool(12, 0);
  this->p_type = _ioBuffer.get_8(12, 1, 3);
  this->p_i_exp = _ioBuffer.get_8(13, 4, 4);
  this->lbppbe = _ioBuffer.get_8(13, 0, 4);
  this->lbpme = _ioBuffer.get_bool(14, 7);
  this->lbprz = _ioBuffer.get_bool(14, 6);
  this->lalba = (((uint16_t) _ioBuffer.get_8(14, 0, 6)) << 8) + _ioBuffer.get_8(15);

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// sense
//
sense::sense()
{
  clear();
}

void sense::clear()
{
  this->response_code = 0;
  this->key = scsi::sense_key::no_sense;
  this->as = static_cast<scsi::ascq>(0);
  this->length = 0;
}

bool sense::empty() const { return this->response_code == 0; }

bool sense::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < SENSE_BUFFER_REPLY_LEN )
  {
    if ( _reqSize ) *_reqSize = SENSE_BUFFER_REPLY_LEN;
    return false;
  }

  // Set member variables
  this->response_code = _ioBuffer.get_8(0, 0, 7);
  this->key = static_cast<scsi::sense_key>(_ioBuffer.get_8(1, 0, 4));
  this->asc = _ioBuffer.get_8(2);
  this->ascq = _ioBuffer.get_8(3);
  this->length = _ioBuffer.get_8(7);

  if ( _ioBuffer.rd_length() < static_cast<size_t>(this->length+SENSE_BUFFER_REPLY_LEN) )
  {
    if ( _reqSize ) *_reqSize = this->length+SENSE_BUFFER_REPLY_LEN;
    return false;
  }

  return true;
}

std::string sense::to_str() const
{
  std::ostringstream out;

  /*
  out << " SENSE KEY:" << scsi_sense_key_str(this->key) << "(" << local::to_str<int>(this->key, Base::HexaDecimal)
      << ") scsi::ascq:" << scsi_sense_ascq_str(this->ascq) << "(" << local::to_str<int>(this->ascq, Base::HexaDecimal, false, 4) << ")";
  */
  /*
  out << " SENSE KEY:" <<  "(" << local::to_str<int>(this->key, Base::HexaDecimal)
      << ") ASC/ASCQ:"  << "(" << local::to_str<int>(this->asc, Base::HexaDecimal, false, 2)
      << "/" << local::to_str<int>(this->ascq, Base::HexaDecimal, false, 2) << ")";
  */
  out << " SENSE KEY:" <<  "(" << local::to_str<int>(static_cast<int>(this->key))
      << ") ASC/ASCQ:"  << "(" << local::to_str<int>(this->asc)
      << "/" << local::to_str<int>(this->ascq) << ")";

  return out.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::basic
//
inquiry::basic::basic()
{
  clear();
}

void inquiry::basic::clear()
{
  qualifier = peripheral_qualifier::not_supported;
  device_type = peripheral_device_type::unknown;
}

bool inquiry::basic::p_set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 1 )
  {
    if ( _reqSize ) *_reqSize = 1;
    return false;
  }

  this->qualifier = (peripheral_qualifier) _ioBuffer.get_8(0, 5, 3);
  this->device_type = (peripheral_device_type) _ioBuffer.get_8(0, 0, 5);
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::standard
//
inquiry::standard::standard() : super()
{
  clear();
}

void inquiry::standard::clear()
{
  super::clear();
  //memset((this, 0, sizeof(standard));
  memset(vendor_identification, 0, sizeof(vendor_identification));
  memset(product_identification, 0, sizeof(product_identification));
  memset(product_revision_level, 0, sizeof(product_revision_level));
  memset(vendor_specific, 0, sizeof(vendor_specific));
  memset(version_descriptor, 0, sizeof(version_descriptor));
}

bool inquiry::standard::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < INQUIRY_STANDARD_REPLY_LEN )
  {
    if ( _reqSize ) *_reqSize = INQUIRY_STANDARD_REPLY_LEN;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  this->rmb = _ioBuffer.get_8(1, 7, 1);
  this->version = _ioBuffer.get_8(2);
  this->normaca = _ioBuffer.get_bool(3, 5);
  this->hisup = _ioBuffer.get_bool(3, 4);
  this->response_data_format = _ioBuffer.get_8(3, 0, 4);
  this->additional_length = _ioBuffer.get_8(4);
  memcpy(this->vendor_identification, _ioBuffer.rd_data(8), sizeof(this->vendor_identification)-1);
  memcpy(this->product_identification, _ioBuffer.rd_data(16), sizeof(this->product_identification)-1);
  memcpy(this->product_revision_level, _ioBuffer.rd_data(32), sizeof(this->product_revision_level)-1);
  memcpy(this->vendor_specific, _ioBuffer.rd_data(36), sizeof(this->vendor_specific)-1);

  //std::string x((char*) _ioBuffer.rd_data(96), (size_t) this->additional_length);

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::basic_vpd
//
inquiry::basic_vpd::basic_vpd(const uint8_t code_page) : super()
{
  clear();
  page_code = code_page;
}

void inquiry::basic_vpd::clear()
{
  super::clear();
  // page_code = 0; This should not be cleared
}

bool inquiry::basic_vpd::p_set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 2 )
  {
    if ( _reqSize ) *_reqSize = 2;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  this->page_code = _ioBuffer.get_8(1);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::unit_serial_number
//
inquiry::unit_serial_number::unit_serial_number() : super(code_page())
{
  clear();
}

void inquiry::unit_serial_number::clear()
{
  super::clear();
  page_length = 0;
  serial_number.clear();
}

bool inquiry::unit_serial_number::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 4 )
  {
    if ( _reqSize ) *_reqSize = 4;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  this->page_length = _ioBuffer.get_8(3);
  if ( page_length > 0 )
  {
    if ( _ioBuffer.rd_length() < static_cast<size_t>(this->page_length+4) )
    {
      if ( _reqSize ) *_reqSize = this->page_length+4;
      return false;
    }

    this->serial_number = _ioBuffer.get_string(4, this->page_length);
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::supported_vpd_pages
//
inquiry::supported_vpd_pages::supported_vpd_pages() : super(code_page())
{
  clear();
}

void inquiry::supported_vpd_pages::clear()
{
  super::clear();
  page_length = 0;
  pages.clear();
}

bool inquiry::supported_vpd_pages::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 3 )
  {
    if ( _reqSize ) *_reqSize = 3;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  this->page_length = _ioBuffer.get_8(3);
  const size_t needed = ( this->page_length > _ioBuffer.rd_length() )? (this->page_length - _ioBuffer.rd_length()) : 0;
  if ( needed == 0 )
  {
    const size_t n = this->page_length + 3;
    for ( size_t i = 4; i <= n && (_ioBuffer.rd_length()-i) > 0; i++ )
    {
      uint8_t page = _ioBuffer.get_8(i);
      this->pages.insert(page);
    }
  }
  return (needed == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::device_identification
//
inquiry::device_identification::device_identification() : super(code_page())
{
  clear();
}

void inquiry::device_identification::clear()
{
  super::clear();
  designators.clear();
}

bool inquiry::device_identification::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 2 )
  {
    if ( _reqSize ) *_reqSize = 2;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::block_limits
//
inquiry::block_limits::block_limits() : super(code_page())
{
  clear();
}

void inquiry::block_limits::clear()
{
  super::clear();
}

bool inquiry::block_limits::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 2 )
  {
    if ( _reqSize ) *_reqSize = 2;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::block_device_characteristics
//
inquiry::block_device_characteristics::block_device_characteristics() : super(code_page())
{
  clear();
}

void inquiry::block_device_characteristics::clear()
{
  super::clear();
}

bool inquiry::block_device_characteristics::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 2 )
  {
    if ( _reqSize ) *_reqSize = 2;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::logical_block_provisioning
//
inquiry::logical_block_provisioning::logical_block_provisioning() : super(code_page())
{
  clear();
}

void inquiry::logical_block_provisioning::clear()
{
  super::clear();
}

bool inquiry::logical_block_provisioning::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( _ioBuffer.rd_length() < 2 )
  {
    if ( _reqSize ) *_reqSize = 2;
    return false;
  }

  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// inquiry::custom_vpd
//
inquiry::custom_vpd::custom_vpd(const uint8_t code_page) : super(code_page)
{
  clear();
}

void inquiry::custom_vpd::clear()
{
  super::clear();
  data.clear();
}

bool inquiry::custom_vpd::set(const sid::io_buffer& _ioBuffer, size_t* _reqSize/* = nullptr*/)
{
  // Make sure to call the super::p_set() first before setting the member variables
  if ( !super::p_set(_ioBuffer, _reqSize) )
    return false;

  // Set member variables
  this->data = _ioBuffer;
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// read16
//
read16::read16() : data(nullptr)
{
  clear();
}

void read16::clear()
{
  // Clear everything except the data pointer
  unsigned char* d = this->data;
  memset(this, 0, sizeof(read16));
  this->data = d;
}

sid::io_buffer read16::get() const
{
  sid::io_buffer ioBuffer(16);

  // Fill the buffer with CDB data
  ioBuffer.set_8(0, this->opcode());
  ioBuffer.set_8(1, 5, 3, this->rd_protect);
  ioBuffer.set_bool(1, 4, this->dpo);
  ioBuffer.set_bool(1, 3, this->fua);
  ioBuffer.set_bool(1, 2, this->rarc);
  ioBuffer.set_bool(1, 1, this->fua_nv);
  ioBuffer.set_64(2, this->lba);
  ioBuffer.set_32(10, this->transfer_length);
  ioBuffer.set_8(14, 0, 5, this->group);
  ioBuffer.set_8(15, this->control);

  /*
  cout << "    read cdb:";
  for ( size_t i = ioBuffer.get_zero_pos(); i < ioBuffer.wr_length(); i++ )
    cout << " " << setfill('0') << setw(2) << std::hex << ((int) ioBuffer[i]);
  cout << endl;
  */
  return ioBuffer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// write16
//
write16::write16() : data(nullptr)
{
  clear();
}

void write16::clear()
{
  // Clear everything except the data pointer
  const unsigned char* d = this->data;
  memset(this, 0, sizeof(write16));
  this->data = d;
}

sid::io_buffer write16::get() const
{
  sid::io_buffer ioBuffer(16);

  // Fill the buffer with CDB data
  ioBuffer.set_8(0, this->opcode());
  ioBuffer.set_8(1, 5, 3, this->wr_protect);
  ioBuffer.set_bool(1, 4, this->dpo);
  ioBuffer.set_bool(1, 3, this->fua);
  ioBuffer.set_bool(1, 2, this->rarc);
  ioBuffer.set_bool(1, 1, this->fua_nv);
  ioBuffer.set_64(2, this->lba);
  ioBuffer.set_32(10, this->transfer_length);
  ioBuffer.set_8(14, 0, 5, this->group);
  ioBuffer.set_8(15, this->control);

  return ioBuffer;
}

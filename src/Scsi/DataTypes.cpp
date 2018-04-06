/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file DataTypes.cpp
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
 * @file  DataTypes.cpp
 * @brief Implementation of scsi datatypes
 */

#include <Scsi/DataTypes.h>
#include <string.h>
#include <sstream>
#include "local.h"

using namespace Gratis::Scsi;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Capacity10_t
//
Capacity10_t::Capacity10_t()
{
  clear();
}

void Capacity10_t::clear()
{
  memset(this, 0, sizeof(Capacity10_t));
}

bool Capacity10_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < READ_CAP10_REPLY_LEN )
  {
    if ( reqSize ) *reqSize = READ_CAP10_REPLY_LEN;
    return false;
  }

  // Set member variables
  uint32_t last_lba = ioBuffer.get_32(0);
  if ( 0xffffffff == last_lba )
    throw std::string("Device capacity is too large. Use Capacity16_t instead of Capacity10_t");

  this->num_blocks = last_lba + 1;
  this->block_size = ioBuffer.get_32(4);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Capacity16_t
//
Capacity16_t::Capacity16_t()
{
  clear();
}

void Capacity16_t::clear()
{
  memset(this, 0, sizeof(Capacity16_t));
}

bool Capacity16_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < READ_CAP16_REPLY_LEN )
  {
    if ( reqSize ) *reqSize = READ_CAP16_REPLY_LEN;
    return false;
  }

  // Set member variables
  uint64_t last_lba = ioBuffer.get_64(0);
  this->num_blocks = last_lba + 1;
  this->block_size = ioBuffer.get_32(8);
  this->prot_en = ioBuffer.get_bool(12, 0);
  this->p_type = ioBuffer.get_8(12, 1, 3);
  this->p_i_exp = ioBuffer.get_8(13, 4, 4);
  this->lbppbe = ioBuffer.get_8(13, 0, 4);
  this->lbpme = ioBuffer.get_bool(14, 7);
  this->lbprz = ioBuffer.get_bool(14, 6);
  this->lalba = (((uint16_t) ioBuffer.get_8(14, 0, 6)) << 8) + ioBuffer.get_8(15);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Sense_t
//
Sense_t::Sense_t()
{
  clear();
}

void Sense_t::clear()
{
  this->response_code = 0;
  this->key = SenseKey::NO_SENSE;
  this->as = static_cast<ASCQ>(0);
  this->length = 0;
}

bool Sense_t::empty() const { return this->response_code == 0; }

bool Sense_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < SENSE_BUFFER_REPLY_LEN )
  {
    if ( reqSize ) *reqSize = SENSE_BUFFER_REPLY_LEN;
    return false;
  }

  // Set member variables
  this->response_code = ioBuffer.get_8(0, 0, 7);
  this->key = static_cast<SenseKey>(ioBuffer.get_8(1, 0, 4));
  this->asc = ioBuffer.get_8(2);
  this->ascq = ioBuffer.get_8(3);
  this->length = ioBuffer.get_8(7);

  if ( ioBuffer.rd_length() < static_cast<size_t>(this->length+SENSE_BUFFER_REPLY_LEN) )
  {
    if ( reqSize ) *reqSize = this->length+SENSE_BUFFER_REPLY_LEN;
    return false;
  }

  return true;
}

std::string Sense_t::toString() const
{
  std::ostringstream out;

  /*
  out << " SENSE KEY:" << scsi_sense_key_str(this->key) << "(" << local::toString<int>(this->key, Base::HexaDecimal)
      << ") ASCQ:" << scsi_sense_ascq_str(this->ascq) << "(" << local::toString<int>(this->ascq, Base::HexaDecimal, false, 4) << ")";
  */
  /*
  out << " SENSE KEY:" <<  "(" << local::toString<int>(this->key, Base::HexaDecimal)
      << ") ASC/ASCQ:"  << "(" << local::toString<int>(this->asc, Base::HexaDecimal, false, 2)
      << "/" << local::toString<int>(this->ascq, Base::HexaDecimal, false, 2) << ")";
  */
  out << " SENSE KEY:" <<  "(" << local::toString<int>(static_cast<int>(this->key))
      << ") ASC/ASCQ:"  << "(" << local::toString<int>(this->asc)
      << "/" << local::toString<int>(this->ascq) << ")";

  return out.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::Basic
//
Inquiry::Basic_t::Basic_t()
{
  clear();
}

void Inquiry::Basic_t::clear()
{
  qualifier = Peripheral_Qualifier::Not_Supported;
  device_type = Peripheral_Device_Type::Unknown;
}

bool Inquiry::Basic_t::p_set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 1 )
  {
    if ( reqSize ) *reqSize = 1;
    return false;
  }

  this->qualifier = (Peripheral_Qualifier) ioBuffer.get_8(0, 5, 3);
  this->device_type = (Peripheral_Device_Type) ioBuffer.get_8(0, 0, 5);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::Standard_t
//
Inquiry::Standard_t::Standard_t() : Basic_t()
{
  clear();
}

void Inquiry::Standard_t::clear()
{
  Basic_t::clear();
  //memset((this, 0, sizeof(Standard_t));
  memset(vendor_identification, 0, sizeof(vendor_identification));
  memset(product_identification, 0, sizeof(product_identification));
  memset(product_revision_level, 0, sizeof(product_revision_level));
  memset(vendor_specific, 0, sizeof(vendor_specific));
  memset(version_descriptor, 0, sizeof(version_descriptor));
}

bool Inquiry::Standard_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < INQUIRY_STANDARD_REPLY_LEN )
  {
    if ( reqSize ) *reqSize = INQUIRY_STANDARD_REPLY_LEN;
    return false;
  }

  // Make sure to call the Inquiry::Basic_t::p_set() first before setting the member variables
  if ( !Basic_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  this->rmb = ioBuffer.get_8(1, 7, 1);
  this->version = ioBuffer.get_8(2);
  this->normaca = ioBuffer.get_bool(3, 5);
  this->hisup = ioBuffer.get_bool(3, 4);
  this->response_data_format = ioBuffer.get_8(3, 0, 4);
  this->additional_length = ioBuffer.get_8(4);
  memcpy(this->vendor_identification, ioBuffer.rd_data(8), sizeof(this->vendor_identification)-1);
  memcpy(this->product_identification, ioBuffer.rd_data(16), sizeof(this->product_identification)-1);
  memcpy(this->product_revision_level, ioBuffer.rd_data(32), sizeof(this->product_revision_level)-1);
  memcpy(this->vendor_specific, ioBuffer.rd_data(36), sizeof(this->vendor_specific)-1);

  //std::string x((char*) ioBuffer.rd_data(96), (size_t) this->additional_length);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::BasicVPD_t
//
Inquiry::BasicVPD_t::BasicVPD_t(const uint8_t code_page) : Basic_t()
{
  clear();
  page_code = code_page;
}

void Inquiry::BasicVPD_t::clear()
{
  Basic_t::clear();
  // page_code = 0; This should not be cleared
}

bool Inquiry::BasicVPD_t::p_set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 2 )
  {
    if ( reqSize ) *reqSize = 2;
    return false;
  }

  // Make sure to call the Inquiry::Basic_t::p_set() first before setting the member variables
  if ( !Basic_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  this->page_code = ioBuffer.get_8(1);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::UnitSerialNumber_t
//
Inquiry::UnitSerialNumber_t::UnitSerialNumber_t() : BasicVPD_t(codePage())
{
  clear();
}

void Inquiry::UnitSerialNumber_t::clear()
{
  BasicVPD_t::clear();
  page_length = 0;
  serial_number.clear();
}

bool Inquiry::UnitSerialNumber_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 4 )
  {
    if ( reqSize ) *reqSize = 4;
    return false;
  }

  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  this->page_length = ioBuffer.get_8(3);
  if ( page_length > 0 )
  {
    if ( ioBuffer.rd_length() < static_cast<size_t>(this->page_length+4) )
    {
      if ( reqSize ) *reqSize = this->page_length+4;
      return false;
    }

    this->serial_number = ioBuffer.get_string(4, this->page_length);
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::SupportedVPDPages_t
//
Inquiry::SupportedVPDPages_t::SupportedVPDPages_t() : BasicVPD_t(codePage())
{
  clear();
}

void Inquiry::SupportedVPDPages_t::clear()
{
  BasicVPD_t::clear();
  page_length = 0;
  pages.clear();
}

bool Inquiry::SupportedVPDPages_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 3 )
  {
    if ( reqSize ) *reqSize = 3;
    return false;
  }

  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  this->page_length = ioBuffer.get_8(3);
  const size_t needed = ( this->page_length > ioBuffer.rd_length() )? (this->page_length - ioBuffer.rd_length()) : 0;
  if ( needed == 0 )
  {
    const size_t n = this->page_length + 3;
    for ( size_t i = 4; i <= n && (ioBuffer.rd_length()-i) > 0; i++ )
    {
      uint8_t page = ioBuffer.get_8(i);
      this->pages.insert(page);
    }
  }
  return (needed == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::DeviceIdentification_t
//
Inquiry::DeviceIdentification_t::DeviceIdentification_t() : BasicVPD_t(codePage())
{
  clear();
}

void Inquiry::DeviceIdentification_t::clear()
{
  BasicVPD_t::clear();
  designators.clear();
}

bool Inquiry::DeviceIdentification_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 2 )
  {
    if ( reqSize ) *reqSize = 2;
    return false;
  }

  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::BlockLimits_t
//
Inquiry::BlockLimits_t::BlockLimits_t() : BasicVPD_t(codePage())
{
  clear();
}

void Inquiry::BlockLimits_t::clear()
{
  BasicVPD_t::clear();
}

bool Inquiry::BlockLimits_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 2 )
  {
    if ( reqSize ) *reqSize = 2;
    return false;
  }

  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::BlockDeviceCharacteristics_t
//
Inquiry::BlockDeviceCharacteristics_t::BlockDeviceCharacteristics_t() : BasicVPD_t(codePage())
{
  clear();
}

void Inquiry::BlockDeviceCharacteristics_t::clear()
{
  BasicVPD_t::clear();
}

bool Inquiry::BlockDeviceCharacteristics_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 2 )
  {
    if ( reqSize ) *reqSize = 2;
    return false;
  }

  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::LogicalBlockProvisioning_t
//
Inquiry::LogicalBlockProvisioning_t::LogicalBlockProvisioning_t() : BasicVPD_t(codePage())
{
  clear();
}

void Inquiry::LogicalBlockProvisioning_t::clear()
{
  BasicVPD_t::clear();
}

bool Inquiry::LogicalBlockProvisioning_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Check for minimum data buffer size required to contain the command's data
  if ( ioBuffer.rd_length() < 2 )
  {
    if ( reqSize ) *reqSize = 2;
    return false;
  }

  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Inquiry::CustomVPD_t
//
Inquiry::CustomVPD_t::CustomVPD_t(const uint8_t code_page) : BasicVPD_t(code_page)
{
  clear();
}

void Inquiry::CustomVPD_t::clear()
{
  BasicVPD_t::clear();
  data.clear();
}

bool Inquiry::CustomVPD_t::set(const Util::IOBuffer_t& ioBuffer, size_t* reqSize/* = nullptr*/)
{
  // Make sure to call the Inquiry::BasicVPD_t::p_set() first before setting the member variables
  if ( !BasicVPD_t::p_set(ioBuffer, reqSize) )
    return false;

  // Set member variables
  this->data = ioBuffer;
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ByteRange_t
//
BlockRange_t ByteRange_t::block_range(const uint32_t block_size) const
{
  BlockRange_t r_block;
  r_block.lba      = this->offset / block_size;
  r_block.n_blocks = this->n_bytes / block_size;
  return r_block;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// BlockRange_t
//
ByteRange_t BlockRange_t::byte_range(const uint32_t block_size) const
{
  ByteRange_t r_byte;
  r_byte.offset  = this->lba * block_size;
  r_byte.n_bytes = this->n_blocks * block_size;
  return r_byte;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IOFlags_t (For Read and Write)
//
IOFlags_t::IOFlags_t()
{
  clear();
}

void IOFlags_t::clear()
{
  memset(this, 0, sizeof(IOFlags_t));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Read16_t
//
Read16_t::Read16_t() : flags(), range(), data(nullptr), bytes_read(0)
{
  clear();
}

void Read16_t::clear()
{
  // Clear everything except the data pointer
  flags.clear();
  range.clear();
  bytes_read = 0;
  // data; DO NOT CLEAR
}

Util::IOBuffer_t Read16_t::get() const
{
  if ( (this->range.n_blocks & 0xFFFFFFFF) != this->range.n_blocks )
    throw std::string("Read16_t cannot address blocks over 32-bits: ") + local::toString(this->range.n_blocks);

  Util::IOBuffer_t ioBuffer(16);

  // Fill the buffer with CDB data
  ioBuffer.set_8(0, this->opcode());
  ioBuffer.set_8(1, 5, 3, this->flags.protect);
  ioBuffer.set_bool(1, 4, this->flags.dpo);
  ioBuffer.set_bool(1, 3, this->flags.fua);
  ioBuffer.set_bool(1, 2, this->flags.rarc);
  ioBuffer.set_bool(1, 1, this->flags.fua_nv);
  ioBuffer.set_64(2, this->range.lba);
  ioBuffer.set_32(10, static_cast<uint32_t>(this->range.n_blocks));
  ioBuffer.set_8(14, 0, 5, this->flags.group);
  ioBuffer.set_8(15, this->flags.control);

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
// Write16_t
//
Write16_t::Write16_t() : flags(), range(), data(nullptr), bytes_written(0)
{
}

void Write16_t::clear()
{
  // Clear everything except the data pointer
  flags.clear();
  range.clear();
  bytes_written = 0;
  // data; DO NOT CLEAR
}

Util::IOBuffer_t Write16_t::get() const
{
  if ( (this->range.n_blocks & 0xFFFFFFFF) != this->range.n_blocks )
    throw std::string("Write16_t cannot address blocks over 32-bits: ") + local::toString(this->range.n_blocks);

  Util::IOBuffer_t ioBuffer(16);

  // Fill the buffer with CDB data
  ioBuffer.set_8(0, this->opcode());
  ioBuffer.set_8(1, 5, 3, this->flags.protect);
  ioBuffer.set_bool(1, 4, this->flags.dpo);
  ioBuffer.set_bool(1, 3, this->flags.fua);
  ioBuffer.set_bool(1, 2, this->flags.rarc);
  ioBuffer.set_bool(1, 1, this->flags.fua_nv);
  ioBuffer.set_64(2, this->range.lba);
  ioBuffer.set_32(10, static_cast<uint32_t>(this->range.n_blocks));
  ioBuffer.set_8(14, 0, 5, this->flags.group);
  ioBuffer.set_8(15, this->flags.control);

  return ioBuffer;
}

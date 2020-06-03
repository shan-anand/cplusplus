/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file datatype.hpp
@brief Definition of SCSI datatypes for serializing/deserializing them.
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
 * @file  datatypes.hpp
 * @brief Definition of SCSI datatypes
 */

#ifndef _SID_SCSI_DATATYPES_H_
#define _SID_SCSI_DATATYPES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include "constants.hpp"
#include <common/io_buffer.hpp>

#define SCSI_DEFAULT_IO_SIZE        (128*1024)

// read capacity 10 response length
#define READ_CAP10_REPLY_LEN         8
// read capacity 16 response length
#define READ_CAP16_REPLY_LEN        32
// read Sense buffer length
#define SENSE_BUFFER_REPLY_LEN       8
// inquiry standard response length
//#define INQUIRY_STANDARD_REPLY_LEN  96
#define INQUIRY_STANDARD_REPLY_LEN  0xFF

#define IMPLEMENT_CLASS_EX(CLASS)                 \
struct CLASS##_ex : public CLASS                  \
{                                                 \
  using super = CLASS;				  \
  scsi::sense sense;				  \
  CLASS##_ex() : CLASS() { sense.clear(); }       \
  void clear() { CLASS::clear(); sense.clear(); } \
};

namespace sid {
namespace block {
namespace scsi {

struct test_unit_ready
{ 
  static size_t static_cdb_size() { return 6; }
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x00; } //! [Byte 0:(0-7)]
  uint32_t reserved;                      //! [Bytes 1 - 4]
  uint8_t  control;                       //! [Byte 5]

  test_unit_ready();
  void clear();
  sid::io_buffer get() const;
  sid::io_buffer& get(sid::io_buffer& ioBuffer) const;
};

struct capacity10
{
  uint32_t num_blocks; //! Number of blocks [Bytes 0-3] + 1
  uint32_t block_size; //! Block length in bytes [Bytes 4-7]

  capacity10();
  void clear();
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr);
};

struct capacity16
{
  static size_t static_cdb_size() { return 16; }
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x9E; } //! [Byte 0:(0-7)]

  // Response
  uint64_t num_blocks; //! Number of blocks [Bytes 0 - 7] + 1
  uint32_t block_size; //! Block length in bytes [Bytes 8 - 11]

  // == Byte 12 == ///////////////////////////////////////////////////////////////////////////
                       //! Reserved [Byte 12:(4-7)]
  uint8_t  p_type;     //! Protection type [Byte 12:(1-3)]
  bool     prot_en;    //! Protection enabled? [Byte 12:(0)]

  // == Byte 13 == ///////////////////////////////////////////////////////////////////////////
  uint8_t  p_i_exp;    //! Protection exponent [Byte 13:(4-7)]
  uint8_t  lbppbe;     //! Logical blocks per physical block exponent [Byte 13:(0-3)]

  // == Byte 14 == ///////////////////////////////////////////////////////////////////////////
  bool     lbpme;      //! Locical block provisioning [Byte 14:(7)]
  bool     lbprz;      //! Locical block provisioning [Byte 14:(6)]
  uint16_t lalba;      //! Lowest aligned logical block address [Byte 14:(0-5), Byte 15(0-7]]

                       //! Reserved [Bytes 16 - 31]

  capacity16();
  void clear();
  uint64_t bytes() const { return num_blocks * block_size; }

  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr);
  sid::io_buffer get() const;
  sid::io_buffer& get(sid::io_buffer& ioBuffer) const;
};

struct sense
{
  uint8_t      response_code;  //! Response Code (0x72 or 0x73) [Byte 0:(0-6)]
  sense_key    key;            //! Sense key [Byte: 1:(0-3)]
  union
  {
    scsi::ascq as;             //! Addtional sense (Has both asc and ascq) [Bytes 2-3]
    struct
    {
      uint8_t  asc;            //! Additional Sense Code [Byte 2:(0-7)]
      uint8_t  ascq;           //! Additional Sense Code Qualifier[Byte 3:(0-7)]
    };
  };
  uint8_t      length;         //! Additional sense length

  sense();
  void clear();
  bool empty() const;
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr);
  std::string to_str() const;
};

/**
 * @struct read16
 * @brief Data structure for SCSI READ(16)
 */
struct read16
{
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x88; } //! [Byte 0:(0-7)]
  uint8_t        rd_protect;        //! [Byte 1:(5-7)]
  bool           dpo;               //! [Byte 1:(4)]
  bool           fua;               //! [Byte 1:(3)]
  bool           rarc;              //! [Byte 1:(2)]
  bool           fua_nv;            //! [Byte 1:(1)]
  uint64_t       lba;               //! [Bytes 2 - 9]
  uint32_t       transfer_length;   //! [Bytes 10 - 13]
  uint8_t        group;             //! [Byte 14:(0-5)]
  uint8_t        control;           //! [Byte 15:(0-7)]

  // Other members
  unsigned char* data;              //! Pointer to the data where the data is read
                                    //! The size of the buffer must be "transfer_length" or more
  uint32_t       data_size_read;    //! Output

  read16();
  void clear();
  sid::io_buffer get() const;
};

IMPLEMENT_CLASS_EX(read16);

/**
 * @struct write16
 * @brief Data structure for SCSI WRITE(16)
 */
struct write16
{
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x8A; } //! [Byte 0:(0-7)]
  uint8_t        wr_protect;        //! [Byte 1:(5-7)]
  bool           dpo;               //! [Byte 1:(4)]
  bool           fua;               //! [Byte 1:(3)]
  bool           rarc;              //! [Byte 1:(2)]
  bool           fua_nv;            //! [Byte 1:(1)]
  uint64_t       lba;               //! [Bytes 2 - 9]
  uint32_t       transfer_length;   //! [Bytes 10 -13]
  uint8_t        group;             //! [Byte 14:(0-5)]
  uint8_t        control;           //! [Byte 15:(0-7)]

  // Other members
  const unsigned char* data;        //! Pointer to the data from where the data is to be written
  uint32_t       data_size_written; //! Output

  write16();
  void clear();
  sid::io_buffer get() const;
};

IMPLEMENT_CLASS_EX(write16);

namespace inquiry
{

//code_page get_code_page(const uint8_t _page_code) { return (scsi::code_page) _page_code; }

struct cdb
{
  static size_t static_size() { return 6; }
  uint8_t opcode() const { return 0x12; } //! [Byte 0:(0-7)]
  bool    evpd;              //! [Byte 1:(0-7)]
  uint8_t page_code;         //! [Byte 2:(0-7)]
  // reserved                //! [Byte 3:(0-7)]
  uint8_t reply_len;         //! [Byte 4:(0-7)]
  // control                 //! [Byte 5:(0-7)]

  cdb(const bool _evpd = false, uint8_t _page_code = 0x00, uint8_t _reply_len = 0xFF);
  void clear();
};

struct basic
{
  static size_t static_cdb_size() { return 6; }
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x12; } //! [Byte 0:(0-7)]

  // == Byte 0 == ////////////////////////////////////////////////////////////////////////////
  peripheral_qualifier   qualifier;      //! Peripheral qualifier [Byte 0:(5-7)]
  peripheral_device_type device_type;    //! Device type [Byte 0:(0-4)]

  sid::io_buffer get() const;

  virtual void clear();
  virtual bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) = 0;
  virtual sid::io_buffer& get(sid::io_buffer& _ioBuffer) const = 0;

protected:
  basic();
  sid::io_buffer& p_get(sid::io_buffer& _ioBuffer, const cdb& _cdb) const;
  bool p_set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr);
};

struct standard : public basic
{
  using super = basic;

  // == Byte 1 == ////////////////////////////////////////////////////////////////////////////
  uint8_t  rmb;                          //! removable medium bit [Byte 1:(7)]
                                         //! Device type modifier [Byte 1:(0-6)]

  // == Byte 2 == ////////////////////////////////////////////////////////////////////////////
  uint8_t  version;                      //! version [Byte 2:(0-7)]

  // == Byte 3 == ////////////////////////////////////////////////////////////////////////////
                                         //! AENC [Byte 3:(7)]
                                         //! TrmIOP [Byte 3:(6)]
  bool     normaca;                      //! normal ACA supported [Byte 3:(5)]
  bool     hisup;                        //! hierarchical support bit [Byte 3:(4)]
  uint8_t  response_data_format;         //! [Byte 3:(0-3)]

  // == Byte 4 == ////////////////////////////////////////////////////////////////////////////
  uint8_t  additional_length;            //! Additional data length (N-4) [Byte 4:(0-7)] 

  // == Byte 5 == ////////////////////////////////////////////////////////////////////////////
  bool     sccs;                         //! SSC suppored bit [Byte 5:(7)]
  bool     acc;                          //! access controls coordinator bit [Byte 5:(6)]
  uint8_t  tpgs;                         //! target port group support [Byte 5:(4-5)]
  bool     threepc;                      //! thrid party copy bit [Byte 5:(3)]
                                         //! Reserved [Byte 5:(1-2)]
  bool     protect;                      //! protect bit [Byte 5:(0)]

  // == Byte 6 == ////////////////////////////////////////////////////////////////////////////
                                         //! Reserved [Byte 6:(7)]
  bool     encserv;                      //! enclosure services bit [Byte 6:(6)]
                                         //! vendor-specific [Byte 6:(5)]
  bool     multip;                       //! multi-port  bit [Byte 6:(4)]
                                         //! Reserved [Byte 6:(1-3)]
  bool     addr16;                       //! wide SCSI address 16 bit [Byte 6:(0)]

  // == Byte 7 == ////////////////////////////////////////////////////////////////////////////
  bool     wbus16;                       //! wide bus 16 bit [Byte 7:(5)]
  bool     sync;                         //! [Byte 7:(4)]
  bool     cmdque;                       //! command queueing bit [Byte 7:(1)]
                                         //! vendor-specific [Byte 7:(0)]

  // == Others == ///////////////////////////////////////////////////////////////////////////
  char     vendor_identification[8+1];   //! Vendor ID [Bytes 8 - 15]
  char     product_identification[16+1]; //! Product ID [Bytes 16 - 31]
  char     product_revision_level[4+1];  //! Product revision [Bytes 32 - 35]
  char     vendor_specific[20+1];        //! [Bytes 36 - 55]

  uint16_t version_descriptor[8];        //! [Bytes 58 - 73]
  // [Bytes 74 - 95] are reserved

  // == Byte 96 == //////////////////////////////////////////////////////////////////////////
  // [Bytes 96 - N] are vendor specific-parameters

  standard();
  void clear();
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;
  sid::io_buffer& get(sid::io_buffer& _ioBuffer) const override;

private:
  void p_clear();
};

struct basic_vpd : public basic
{
  using super = basic;
  // == Byte 1 == ////////////////////////////////////////////////////////////////////////////
  uint8_t page_code;                     //! Page code [Byte 1:(0-7)]

  virtual scsi::code_page get_code_page() const = 0;
  void clear() override;
  virtual bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) = 0;

  sid::io_buffer& get(sid::io_buffer& _ioBuffer) const override;

protected:
  basic_vpd(const uint8_t _code_page);
  basic_vpd(const scsi::code_page& _code_page);
  bool p_set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr);

private:
  void p_clear();
};

struct supported_vpd_pages : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::supported_vpd_pages; }
  scsi::code_page get_code_page() const override { return code_page(); }

  uint8_t           page_length;  //! Number of pages (n-3) [Byte 3:(0-7)]
  std::set<uint8_t> pages;        //! Supported Pages [Bytes 4-n]

  supported_vpd_pages();
  void clear() override;
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

struct unit_serial_number : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::unit_serial_number; }
  scsi::code_page get_code_page() const override { return code_page(); }

  // == Byte 2 == ////////////////////////////////////////////////////////////////////////////
  // Reserved                            //! Reserved [Byte 2:(0-7)]

  // == Byte 3 == ////////////////////////////////////////////////////////////////////////////
  uint8_t page_length;                   //! Page length [Byte 3:(0-7)]

  // == Bytes 4 - n == ///////////////////////////////////////////////////////////////////////
  std::string serial_number;             //! Product serial number [Bytes 4 - (page_length+4)]

  unit_serial_number();
  void clear() override;
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

struct device_designator
{
  scsi::protocol_id     protocol_id;        //! [Byte 0:(4-7)]
  scsi::code_set        code_set;           //! [Byte 0:(0-3)]
  bool                  protocol_id_valid;  //! [Byte 1:(7)]
  scsi::association     association;        //! [Byte 1:(4-5)]
  scsi::identifier_type identifier_type;    //! [Byte 1:(0-3)]
  // identifier_length                      //! (n-3) [Byte 3:(0-7)]
  std::string           identifier;         //! [Bytes 4-n]
};
using device_designators = std::vector<device_designator>;

struct device_identification : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::device_identification; }
  scsi::code_page get_code_page() const override { return code_page(); }
  device_designators designators;

  device_identification();
  void clear() override;
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

struct block_limits : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::block_limits; }
  scsi::code_page get_code_page() const override { return code_page(); }

  block_limits();
  void clear() override;
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

struct block_device_characteristics : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::block_device_characteristics; }
  scsi::code_page get_code_page() const override { return code_page(); }

  block_device_characteristics();
  void clear();
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

struct logical_block_provisioning : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::logical_block_provisioning; }
  scsi::code_page get_code_page() const override { return code_page(); }

  logical_block_provisioning();
  void clear();
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

struct custom_vpd : public basic_vpd
{
  using super = basic_vpd;
  static scsi::code_page code_page() { return scsi::code_page::custom_vpd; }
  scsi::code_page get_code_page() const override { return code_page(); }

  sid::io_buffer data; //! Raw data, to be processed by the caller

  custom_vpd(const uint8_t _code_page);
  void clear();
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr) override;

private:
  void p_clear();
};

} // namespace inquiry

} // namespace scsi
} // namespace block
} // namespace sid

#endif // _SID_SCSI_DATATYPES_H_

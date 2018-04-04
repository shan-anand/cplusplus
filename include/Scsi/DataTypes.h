/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/anand-gs
@file IOBuffer.h
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
 * @file  Scsi_DataTypes.h
 * @brief Definition of scsi datatypes
 */

#ifndef _SCSI_DATATYPES_H_
#define _SCSI_DATATYPES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include "Constants.h"
#include <Common/IOBuffer.h>

#define SCSI_DEFAULT_IO_SIZE        (128*1024)

// read capacity 10 response length
#define READ_CAP10_REPLY_LEN         8
// read capacity 16 response length
#define READ_CAP16_REPLY_LEN        32
// read Sense buffer length
#define SENSE_BUFFER_REPLY_LEN       8
// inquiry standard response length
#define INQUIRY_STANDARD_REPLY_LEN  96

#define IMPLEMENT_CLASS_EX(CLASS)                 \
struct CLASS##Ex : public CLASS                   \
{                                                 \
  Sense sense;                                    \
  CLASS##Ex() : CLASS() { sense.clear(); }        \
  void clear() { CLASS::clear(); sense.clear(); } \
};

namespace Gratis {
namespace Scsi {

struct Capacity10
{
  uint32_t num_blocks; //! Number of blocks [Bytes 0-3] + 1
  uint32_t block_size; //! Block length in bytes [Bytes 4-7]

  Capacity10();
  void clear();
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr);
};

struct Capacity16
{
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

  Capacity16();
  void clear();
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr);
};

struct Sense
{
  uint8_t     response_code;  //! Response Code (0x72 or 0x73) [Byte 0:(0-6)]
  SenseKey    key;            //! Sense key [Byte: 1:(0-3)]
  union
  {
    ASCQ      as;             //! Addtional sense (Has both asc and ascq) [Bytes 2-3]
    struct
    {
      uint8_t asc;            //! Additional Sense Code [Byte 2:(0-7)]
      uint8_t ascq;           //! Additional Sense Code Qualifier[Byte 3:(0-7)]
    };
  };
  uint8_t     length;         //! Additional sense length

  Sense();
  void clear();
  bool empty() const;
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr);
  std::string toString() const;
};

/**
 * @struct Read16
 * @brief Data structure for SCSI READ(16)
 */
struct Read16
{
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x88; } //! [Byte 0:(0-7)]
  uint8_t        rdprotect;         //! [Byte 1:(5-7)]
  bool           dpo;               //! [Byte 1:(4)]
  bool           fua;               //! [Byte 1:(3)]
  bool           rarc;              //! [Byte 1:(2)]
  bool           fua_nv;            //! [Byte 1:(1)]
  uint64_t       lba;               //! [Bytes 2 - 9]
  uint32_t       transfer_length;   //! [Bytes 10 -13]
  uint8_t        group;             //! [Byte 14:(0-5)]
  uint8_t        control;           //! [Byte 15:(0-7)]

  // Other members
  unsigned char* data;              //! Pointer to the data where the data is read
                                    //! The size of the buffer must be "transfer_length" or more
  uint32_t       dataSizeRead;      //! Output

  Read16();
  void clear();
  Util::IOBuffer get() const;
};

IMPLEMENT_CLASS_EX(Read16);

/**
 * @struct Write16
 * @brief Data structure for SCSI WRITE(16)
 */
struct Write16
{
  // CDB (Command Descriptor Block) members
  uint8_t opcode() const { return 0x8A; } //! [Byte 0:(0-7)]
  uint8_t        wrprotect;         //! [Byte 1:(5-7)]
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
  uint32_t       dataSizeWritten;   //! Output

  Write16();
  void clear();
  Util::IOBuffer get() const;
};

IMPLEMENT_CLASS_EX(Write16);

namespace Inquiry
{

struct Basic
{
  // == Byte 0 == ////////////////////////////////////////////////////////////////////////////
  Peripheral_Qualifier   qualifier;      //! Peripheral qualifier [Byte 0:(5-7)]
  Peripheral_Device_Type device_type;    //! Device type [Byte 0:(0-4)]

  virtual void clear();
  virtual bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) = 0;

protected:
  Basic();
  bool p_set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr);
};

struct Standard : public Basic
{
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

  Standard();
  void clear();
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct BasicVPD : public Basic
{
  // == Byte 1 == ////////////////////////////////////////////////////////////////////////////
  uint8_t page_code;                     //! Page code [Byte 1:(0-7)]

  virtual CodePage getCodePage() const = 0;
  void clear() override;
  virtual bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) = 0;

protected:
  BasicVPD(const uint8_t code_page);
  BasicVPD(const CodePage& code_page) : BasicVPD(static_cast<uint8_t>(code_page)) {}
  bool p_set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr);
};

struct SupportedVPDPages : public BasicVPD
{
  static CodePage codePage() { return CodePage::Supported_VPD_Pages; }
  CodePage getCodePage() const override { return codePage(); }

  uint8_t           page_length;  //! Number of pages (n-3) [Byte 3:(0-7)]
  std::set<uint8_t> pages;        //! Supported Pages [Bytes 4-n]

  SupportedVPDPages();
  void clear() override;
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct UnitSerialNumber : public BasicVPD
{
  static CodePage codePage() { return CodePage::Unit_Serial_Number; }
  CodePage getCodePage() const override { return codePage(); }

  // == Byte 2 == ////////////////////////////////////////////////////////////////////////////
  // Reserved                            //! Reserved [Byte 2:(0-7)]

  // == Byte 3 == ////////////////////////////////////////////////////////////////////////////
  uint8_t page_length;                   //! Page length [Byte 3:(0-7)]

  // == Bytes 4 - n == ///////////////////////////////////////////////////////////////////////
  std::string serial_number;             //! Product serial number [Bytes 4 - (page_length+4)]

  UnitSerialNumber();
  void clear() override;
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct DeviceDesignator
{
  ProtocolId     protocol_id;        //! [Byte 0:(4-7)]
  CodeSet        code_set;           //! [Byte 0:(0-3)]
  bool           protocol_id_valid;  //! [Byte 1:(7)]
  Association    association;        //! [Byte 1:(4-5)]
  IdentifierType identifier_type;    //! [Byte 1:(0-3)]
  // identifier_length               //! (n-3) [Byte 3:(0-7)]
  std::string    identifier;         //! [Bytes 4-n]
};
typedef std::vector<DeviceDesignator> DeviceDesignators;

struct DeviceIdentification : public BasicVPD
{
  static CodePage codePage() { return CodePage::Device_Identification; }
  CodePage getCodePage() const override { return codePage(); }
  DeviceDesignators designators;

  DeviceIdentification();
  void clear() override;
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct BlockLimits : public BasicVPD
{
  static CodePage codePage() { return CodePage::Block_Limits; }
  CodePage getCodePage() const override { return codePage(); }

  BlockLimits();
  void clear() override;
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct BlockDeviceCharacteristics : public BasicVPD
{
  static CodePage codePage() { return CodePage::Block_Device_Characteristics; }
  CodePage getCodePage() const override { return codePage(); }

  BlockDeviceCharacteristics();
  void clear();
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct LogicalBlockProvisioning : public BasicVPD
{
  static CodePage codePage() { return CodePage::Logical_Block_Provisioning; }
  CodePage getCodePage() const override { return codePage(); }

  LogicalBlockProvisioning();
  void clear();
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

struct CustomVPD : public BasicVPD
{
  static CodePage codePage() { return CodePage::Custom_VPD; }
  CodePage getCodePage() const override { return codePage(); }

  Util::IOBuffer data; //! Raw data, to be processed by the caller

  CustomVPD(const uint8_t code_page);
  void clear();
  bool set(const Util::IOBuffer& ioBuffer, size_t* reqSize = nullptr) override;
};

} // namespace Inquiry

/**
 * @struct DataBlock
 * @brief Data to be read or written at the given offset/blocks
 */
struct DataBlock
{
  uint64_t  lba;      //! Logical Block Address to start IO operation
  uint32_t  blocks;   //! Numbers of blocks to read/write

  DataBlock(const uint64_t& lba_p = 0, const uint32_t& blocks_p = 0) : lba(lba_p), blocks(blocks_p) {}
  void clear() { lba = blocks = 0; }
  bool empty() const { return (lba == 0 && blocks == 0); }
  uint64_t bytes(const uint32_t& block_size) const { return (this->blocks * block_size); }
};

struct DataBlocks : public std::vector<DataBlock>
{
  //! Returns the sum of total number of blocks in the vector
  uint64_t blocks() const;
  //! Returns the sum of total number of bytes in the vector
  uint64_t bytes(const uint32_t& block_size) const;
};

} // namespace Scsi
} // namespace Gratis

#endif // _SCSI_DATATYPES_H_

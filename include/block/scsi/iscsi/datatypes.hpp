/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file datatype.hpp
@brief Definition of ISCSI datatypes for serializing/deserializing them.
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
 * @brief Definition of iscsi datatypes
 */

#pragma once

#include "../datatypes.hpp"
#include <common/smart_ptr.hpp>
#include <common/io_buffer.hpp>

//
// @see https://tools.ietf.org/html/rfc3720
//


/*

   The overall structure of an iSCSI  PDU is as follows:

   Byte/     0       |       1       |       2       |       3       |
      /              |               |               |               |
     |0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
     +---------------+---------------+---------------+---------------+
    0/ Basic Header Segment (BHS)                                    /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
   48/ Additional Header Segment 1 (AHS)  (optional)                 /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
     / Additional Header Segment 2 (AHS)  (optional)                 /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
   ----
     +---------------+---------------+---------------+---------------+
     / Additional Header Segment n (AHS)  (optional)                 /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
   ----
     +---------------+---------------+---------------+---------------+
    k/ Header-Digest (optional)                                      /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
    l/ Data Segment(optional)                                        /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
    m/ Data-Digest (optional)                                        /
    +/                                                               /
     +---------------+---------------+---------------+---------------+

Initiator opcodes defined in this specification are:

     0x00 NOP-Out
     0x01 SCSI Command (encapsulates a SCSI Command Descriptor Block)
     0x02 SCSI Task Management function request
     0x03 Login Request
     0x04 Text Request
     0x05 SCSI Data-Out (for WRITE operations)
     0x06 Logout Request
     0x10 SNACK Request
     0x1c-0x1e Vendor specific codes
*/

#define ISCSI_OPCODE_NOP_OUT        0x00
#define ISCSI_OPCODE_SCSI_CMD       0x01
#define ISCSI_OPCODE_SCSI_TASK_MGMT 0x02
#define ISCSI_OPCODE_LOGIN          0x03
#define ISCSI_OPCODE_TEXT           0x04
#define ISCSI_OPCODE_SCSI_DATAOUT   0x05
#define ISCSI_OPCODE_LOGOUT         0x06
#define ISCSI_OPCODE_SNACK          0x10

namespace sid::block::iscsi {

/*
   The format of the Basic Header Segment (BHS) is:

   Byte/     0       |       1       |       2       |       3       |
      /              |               |               |               |
     |0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
     +---------------+---------------+---------------+---------------+
    0|.|I| Opcode    |F|  Opcode-specific fields                     |
     +---------------+---------------+---------------+---------------+
    4|TotalAHSLength | DataSegmentLength                             |
     +---------------+---------------+---------------+---------------+
    8| LUN or Opcode-specific fields                                 |
     +                                                               +
   12|                                                               |
     +---------------+---------------+---------------+---------------+
   16| Initiator Task Tag                                            |
     +---------------+---------------+---------------+---------------+
   20/ Opcode-specific fields                                        /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
   48
*/
struct basic_header_segment
{
  // byte [0:0]
  bool     immediate;           //! [Byte 0:(1)] Immediate delivery marker
  uint8_t  op_code;             //! [Byte 0:(2-7)] Operation code
  bool     is_final;            //! [Byte 1:(1)] Final bit
  uint32_t op_specific;         //! [Byte 1:(1-7), Bytes 2 - 3] Op-code specific field
  uint8_t  total_ahs_length;    //! [Byte 4] Additional Header Segment length
  uint32_t data_segment_length; //! [Bytes 5 - 7] Length of the data segment
  uint64_t lun;                 //! [Bytes 8 - 15] Logical Unit Number or Opcode-specific
  uint8_t  initiator_task_tag;  //! [Byte 16] Initiator Task Tag
  uint8_t  op_specific3[28];

  basic_header_segment();
  void clear();
  bool empty() const;
  bool set(const sid::io_buffer& _ioBuffer, size_t* _reqSize = nullptr);
  std::string to_str() const;
};

/*
   The general format of an AHS is:

   Byte/     0       |       1       |       2       |       3       |
      /              |               |               |               |
     |0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
     +---------------+---------------+---------------+---------------+
    0| AHSLength                     | AHSType       | AHS-Specific  |
     +---------------+---------------+---------------+---------------+
    4/ AHS-Specific                                                  /
    +/                                                               /
     +---------------+---------------+---------------+---------------+
    x
*/

#define ISCSI_AHS_TYPE_EXTENDED_CDB            1
#define ISCSI_AHS_TYPE_EXP_BIDIR_READ_DATA_LEN 2

struct additional_header_segment
{
  uint16_t ahs_length;
  uint8_t  ahs_type;
  // ... AHS-specific
};

#define SCSI_TASK_ATTR_UNTAGGED       0
#define SCSI_TASK_ATTR_SIMPLE         1
#define SCSI_TASK_ATTR_ORDERED        2
#define SCSI_TASK_ATTR_HEAD_OF_QUEUE  3
#define SCSI_TASK_ATTR_ACA            4

struct login
{
  struct pdu
  {
    bool     immediate;                  //! [Byte 0:(1)] Immediate delivery marker
    uint8_t  op_code;                    //! [Byte 0:(2-7)] Operation code
    bool     is_transit;                 //! [Byte 1:(0)] Transit bit
    bool     is_continue;                //! [Byte 1:(1)] Set when command is expected to input data
    uint8_t  csg;                        //! [Byte 1:(4-5)] Current Stage
    uint8_t  nsg;                        //! [Byte 1:(6-7)] Next Statge
  };
};

struct scsi_command
{
  struct pdu
  {
    // byte [0:0]
    bool     immediate;                  //! [Byte 0:(1)] Immediate delivery marker
    uint8_t  op_code;                    //! [Byte 0:(2-7)] Operation code
    bool     is_final;                   //! [Byte 1:(1)] Final bit
    bool     is_read;                    //! [Byte 1:(2)] Set when command is expected to input data
    bool     is_wrtie;                   //! [Byte 1:(3)] Set when command is expected to output data
    uint8_t  attr;                       //! [Byte 1:(5-7)] Task attributes. See SCSI_TASK_ATTR_xxx
    uint8_t  total_ahs_length;           //! [Byte 4] Additional Header Segment length
    uint32_t data_segment_length;        //! [Bytes 5 - 7] Length of the data segment
    uint64_t lun;                        //! [Bytes 8 - 15] Logical Unit Number or Opcode-specific
    uint32_t initiator_task_tag;         //! [Bytes 16 - 19] Initiator Task Tag
    uint32_t expected_data_transfer_len; //! [Bytes 20 - 23]
    uint32_t cmd_sn;                     //! [Bytes 24 - 27] Command Sequence
                                         //!    Enables ordered delivery across multiple connections in a single session
    uint32_t exp_stat_sn;                //! [Bytes 28 - 31] Command Sequence
                                         //!    Command responses up to ExpStatSN-1 (mod 2**32) have been received (acknowledges status) on the connection
    // SCSI CDB [Bytes 32 - 47]
  };
};

struct nop_out
{
  struct pdu
  {
    bool     immediate;                  //! [Byte 0:(1)] Immediate delivery marker
    uint8_t  op_code;                    //! [Byte 0:(2-7)] Operation code
    bool     is_final;                   //! [Byte 1:(1)] Final bit (Must be set to true)
    uint8_t  total_ahs_length;           //! [Byte 4] Additional Header Segment length
    uint32_t data_segment_length;        //! [Bytes 5 - 7] Length of the data segment
    uint64_t lun;                        //! [Bytes 8 - 15] Logical Unit Number or Opcode-specific
    uint32_t initiator_task_tag;         //! [Bytes 16 - 19] Initiator Task Tag
                                         //!   If not expecting a nop_in response set it to 0xffffffff
                                         //!   If expecting a nop_in response set a proper value that will be retured in nop_in response
    uint32_t target_transfer_tag;        //! [Bytes 20 - 23]
                                         //!    Must be set to 0xffffffff when a request is sent my initiator. In the case of a response to a nop_in request, this must be set from nop_in
    uint32_t cmd_sn;                     //! [Bytes 24 - 27] Command Sequence
                                         //!    Enables ordered delivery across multiple connections in a single session
    uint32_t exp_stat_sn;                //! [Bytes 28 - 31] Command Sequence
                                         //!    Command responses up to ExpStatSN-1 (mod 2**32) have been received (acknowledges status) on the connection
    // Header Digest (Optional)
    // Data segment - ping data (Optional) (Length given in data_segment_length)
    // Data digest (Optional)
  };
};

struct nop_in
{
  struct pdu
  {
    bool     immediate;                  //! [Byte 0:(1)] Immediate delivery marker
    uint8_t  op_code;                    //! [Byte 0:(2-7)] Operation code
    bool     is_final;                   //! [Byte 1:(1)] Final bit (Must be set to true)
    uint8_t  total_ahs_length;           //! [Byte 4] Additional Header Segment length
    uint32_t data_segment_length;        //! [Bytes 5 - 7] Length of the data segment
    uint64_t lun;                        //! [Bytes 8 - 15] Logical Unit Number or Opcode-specific
    uint32_t initiator_task_tag;         //! [Bytes 16 - 19] Initiator Task Tag
                                         //!   If not expecting a nop_in response set it to 0xffffffff
                                         //!   If expecting a nop_in response set a proper value that will be retured in nop_in response
    uint32_t target_transfer_tag;        //! [Bytes 20 - 23]
                                         //!    Must be set to 0xffffffff when a request is sent my initiator. In the case of a response to a nop_in request, this must be set from nop_in
    uint32_t stat_sn;                    //! [Bytes 24 - 27] Command Sequence
                                         //!    Enables ordered delivery across multiple connections in a single session
    uint32_t exp_cmd_sn;                 //! [Bytes 28 - 31] Command Sequence
                                         //!    Command responses up to ExpStatSN-1 (mod 2**32) have been received (acknowledges status) on the connection
    uint32_t max_cmd_sn;                 //! [Bytes 32 - 35] Command Sequence
                                         //!    Command responses up to ExpStatSN-1 (mod 2**32) have been received (acknowledges status) on the connection
    // Header Digest (Optional)
    // Data segment - ping data (Optional) (Length given in data_segment_length)
    // Data digest (Optional)
  };
};

struct pdu : public sid::io_buffer
{
  bool immediate() const;
  void immediate(bool v);

  uint8_t op_code() const;
  void op_code(uint8_t v);

  bool is_final() const;
  void is_final(bool v);

  bool is_read() const;
  void is_read(bool v);

  bool is_write() const;
  void is_write(bool v);

  sid::io_buffer cdb() const;
  void cdb(const sid::io_buffer& v);


  //pdu.immediate(true);
  //pdu.op_code();
  //pdu.is_final(true);
  //pdu.is_read(false);
  //pdu.is_write(false);
  //pdu.cdb(ioBuffer);
};

struct task : public sid::smart_ref
{
  uint64_t       id;
  bool           is_completed;
  sid::io_buffer pduBuf;
};
using task_ptr = sid::smart_ptr<task>;
using tasks = std::vector<task_ptr>;

/*
template <typedef T> struct u_task : public task
{
  T user;
  sid::smart_ptr<u_task> create() { return new u_task(); }
  task_ptr base_ptr();
};
task_ptr task =  u_task<scsi::read16>::create().base_ptr();

auto read16_cb = [&](iscsi::task_ptr task, const std::io_buffer& response)->bool
  {
    sid::smart_ptr< u_task<scsi::read16> > read16_task = dynamic_cast< u_task<scsi::read16>* >(task.ptr());
    if ( !read16_task.empty() )
    {
      scsi::read16& r16 = read16_task->data;
      // get response cdb
      read16.dataRead = response.get();
      read16.buffer.set();
      return true;
    }
  };

struct task : public sid::smart_ref
{
  uint64_t       id;
  std::io_buffer pduBuf;
};
using task_ptr = sid::smart_ptr<task>;
using tasks = std::vector<task_ptr>;

iscsi::tasks tasks;
tasks.push_back(task);

bool initiator::run(iscsi::tasks& tasks, timeout timeOut)
{
  if ( send(tasks) && recv(tasks, timeOut) )
    return true;
  return false;
}

bool initiator::send(iscsi::tasks& tasks)
{
  for ( iscsi::task_ptr task : tasks )
  {
    // send(task->request);
    m_taskMap.add(task);
  }
  // cleanup in case of failure
  for ( iscsi::task_ptr task : tasks )
  {
    m_taskMap.remove(task->id);
  }
}

bool initiator::recv(iscsi::tasks& tasks, timeout timeOut)
{
  tasks.wait(timeOut);
  if ( tasks.completed() )
    break;
  for ( iscsi::task_ptr task : tasks )
  {
    // recv(task->response);
    m_taskMap.remove(task->id);
  }
}

struct task_map : public std::map<uint64_t, task_ptr>;
{
  void add(task_ptr task)
    {
      *(this)[task->id] = task;
    }
  bool remove(const uint64_t id, task_ptr* pTask = nullptr)
    {
    }
};

bool read_capacity(scsi::capacity16& _capacity)
{
  scsi_command::pdu pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.immediate = true;
  pdu.op_code = ISCSI_OPCODE_SCSI_CMD;
  pdu.is_final = true;
  pdu.is_read = false;
  pdu.is_write = false;
  pdu.attr = SCSI_TASK_ATTR_SIMPLE;
  pdu.total_ahs_length = 0;
  pdu.data_segment_length = 0;
  pdu.lun = obj.lun;
  pdu.initiator_task_tag = iscsi->get_next_id();
  pdu.expected_data_transfer_len = 0;

  iscsi::task_ptr task = iscsi::task::create();
  task->request.resize(8*1024);
  sid::io_buffer& ioBuffer = task->request;
  ioBuffer.set_bool(0, 1, pdu.immediate);
  ioBuffer.set_8(0, 2, 5, pdu.op_code);
  ioBuffer.set_bool(1, 0, pdu.is_final);
  ioBuffer.set_bool(1, 1, pdu.is_read);
  ioBuffer.set_bool(1, 2, pdu.is_write);
  ioBuffer.set_8(1, 5, 3, pdu.attr);

  bStatus = run(task);
  if ( bStatus )
    _capacity.set(task->response);

  return bStatus;
}
*/

} // namespace sid::block::iscsi

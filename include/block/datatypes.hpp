/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file datatype.hpp
@brief Definition of block datatypes for serializing/deserializing them.
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
 * @brief Definition of block datatypes
 */

#ifndef _SID_BLOCK_DATATYPES_H_
#define _SID_BLOCK_DATATYPES_H_

#include <string>
#include <vector>
#include <common/io_buffer.hpp>

namespace sid {
namespace block {

//! block device types
struct device_type
{
  //! Datatype definitions
  enum id : uint8_t { invalid = 0, scsi_disk, iscsi, nvme };

private:
  //! Member variables
  device_type::id m_id;

public:
  // Constructors
  device_type(const device_type::id& _id = device_type::invalid)
    : m_id(_id) {}
  device_type(const device_type&) = default;

  //! Overloaded operators
  device_type& operator=(const device_type&) = default;
  device_type& operator=(const device_type::id& _id) { m_id = _id; return *this; }
  operator bool() const { return m_id != device_type::invalid; }

  bool empty() const { return m_id == device_type::invalid; }
  void clear() { m_id = device_type::invalid; }

  const device_type::id& id() const { return m_id; }
  std::string name() const;

  bool is_scsi() const { return (m_id == device_type::scsi_disk || m_id == device_type::iscsi); }
  bool is_nvme() const { return (m_id == device_type::nvme); }
};

//! block data state
struct data_state
{
  //! Datatype definitions
  enum id : uint8_t { set, reset };

private:
  //! Member variables
  data_state::id m_id;

public:
  //! Constructors
  data_state(const data_state::id& _id = data_state::set)
    : m_id(_id) {}

  //! Overloaded operators
  data_state& operator=(const data_state&) = default;
  data_state& operator=(const data_state::id& _id) { m_id = _id; return *this; }

  //! Member functions
  void clear() { m_id = data_state::set; }
  const data_state::id& id() const { return m_id; }
  bool is_set() const { return m_id == data_state::set; }
  std::string name() const;
};

//! data region in bytes
struct byte_region
{
  //! Member variables
  uint64_t offset; //! Offset in bytes
  uint64_t length; //! Length in bytes

  //! Constructor
  byte_region(const uint64_t _offset = 0, const uint64_t _length = 0)
    : offset(_offset), length(_length) {}

  //! Member functions
  void clear() { offset = length = 0; }
  bool empty() { return (length == 0); }
  void validate(const uint32_t _blockSize) const;
};
using byte_regions = std::vector<byte_region>;

//! data unit in bytes (region + state)
struct byte_unit : public byte_region
{
  //! Datatype definitions
  using super = byte_region;

  //! Member variables
  data_state state;

  //! Constructors
  byte_unit(const byte_region& _region = byte_region(), const data_state& _state = data_state())
    : super(_region), state(_state) {}
  byte_unit(const uint64_t _offset, const uint64_t _length, const data_state& _state = data_state())
    : super(_offset, _length), state(_state) {}
  byte_unit(const data_state& _state)
    : super(), state(_state) {}

  //! Member functions
  void clear() { super::clear(); state.clear(); }
  std::string to_str() const;
};

//! A vector of data units in bytes (region + state)
struct byte_units : std::vector<byte_unit>
{
  //! Datatype definitions
  using super = std::vector<byte_unit>;

  //! Member functions
  std::string to_str() const;
  uint64_t length() const;
};

//! data region in blocks
struct block_region
{
  //! Member variables
  uint64_t lba;    //! Offset in blocks (Logical block address)
  uint64_t blocks; //! Number of blocks

  //! Constructor
  block_region(const uint64_t& _lba = 0, const uint64_t& _blocks = 0 )
    : lba(_lba), blocks(_blocks) {}

  //! Member functions
  void clear() { lba = blocks = 0; }
  bool empty() { return (blocks == 0); }
};

//! data unit in blocks (region + state)
struct block_unit : public block_region
{
  //! Datatype definitions
  using super = block_region;

  //! Member variables
  data_state state;

  //! Constructors
  block_unit(const block_region& _region = block_region(), const data_state& _state = data_state())
    : super(_region), state(_state) {}
  block_unit(const uint64_t _lba, const uint64_t _blocks, const data_state& _state = data_state())
    : super(_lba, _blocks), state(_state) {}
  block_unit(const data_state& _state)
    : super(), state(_state) {}

  //! Member functions
  void clear() { super::clear(); state.clear(); }
  std::string to_str() const;
};

//! A vector of data units in blocks (region + state)
struct block_units : std::vector<block_unit>
{
  //! Datatype definitions
  using super = std::vector<byte_unit>;

  //! Member functions
  uint64_t blocks() const;
};

//! common capacity structure
struct capacity
{
  //! Member variables
  uint64_t blocks;      //! Number of blocks
  uint64_t block_size;  //! Block size in bytes

  //! Constructor
  capacity(const uint64_t _blocks = 0, const uint64_t _block_size = 0)
    : blocks(_blocks), block_size(_block_size) {}

  //! Member functions
  //! Return the capacity in bytes
  uint64_t bytes() const { return (blocks * block_size); }
  void clear() { blocks = block_size = 0; }
  bool empty() const { return (blocks == 0 && block_size == 0); }
};

//! structure for read and write
struct io_byte_unit : public byte_unit
{
  //! Datatype definitions
  using super = byte_unit;

  uchar8_t* data;           //! (input) IO buffer
  uint64_t  data_processed; //! (output) data size read/written in bytes

  //! Constructor
  io_byte_unit(const byte_unit& _unit = byte_unit(), uchar8_t* _data = nullptr)
    : super(_unit), data(_data), data_processed(0) {}
  io_byte_unit(const uint64_t _offset, const uint64_t _length, uchar8_t* _data = nullptr)
    : super(_offset, _length), data(_data), data_processed(0) {}
  io_byte_unit(const uint64_t _offset, const uint64_t _length,
          const data_state& _state, uchar8_t* _data = nullptr)
    : super(_offset, _length, _state), data(_data), data_processed(0) {}

  //! Member functions
  void clear(bool _full = true) { super::clear(); data = nullptr; data_processed = 0; }
  void clear_processed() { data_processed = 0; }
  void validate(const uint32_t _blockSize) const;
};

//! A vector of io_read
struct io_byte_units : public std::vector<io_byte_unit>
{
  //! Member functions
  uint64_t data_processed() const;
  void clear_processed();
  void validate(const uint32_t _blockSize) const;
};

} // namespace block
} // namespace sid

#endif // _SID_BLOCK_DATATYPES_H_

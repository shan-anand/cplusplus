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

namespace sid {

namespace block {
struct data_region;
struct data_unit;
}

namespace byte {
}

namespace block {

//! block device types
struct device_type
{
  enum id : uint8_t { invalid = 0, scsi_disk, iscsi, nvme };

private:
  device_type::id m_id;

public:
  // Constructors
  device_type() : m_id(device_type::invalid) {}
  device_type(const device_type&) = default;
  device_type(const device_type::id& _id) : m_id(_id) {}

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

struct data_state
{
  enum id : uint8_t { modified, removed, lost };

private:
  data_state::id m_id;

public:
  // Constructors
  data_state() : m_id(data_state::modified) {}
  data_state(const data_state&) = default;
  data_state(const data_state::id& _id) : m_id(_id) {}

  //! Overloaded operators
  data_state& operator=(const data_state&) = default;
  data_state& operator=(const data_state::id& _id) { m_id = _id; return *this; }

  void clear() { m_id = data_state::modified; }

  const data_state::id& id() const { return m_id; }
  std::string name() const;
};

struct byte_region
{
  uint64_t offset, length;
  byte_region(const uint64_t _offset = 0, const uint64_t _length = 0 )
    : offset(_offset), length(_length)
    {}
  void clear() { offset = length = 0; }
  bool empty() { return (length == 0); }
};
using byte_regions = std::vector<byte_region>;

struct byte_unit : public byte_region
{
  using super = byte_region;
  data_state state;
  byte_unit() : byte_region(), state() {}
  byte_unit(const byte_region& _region) : byte_region(_region), state() {}
  byte_unit(const data_state& _state) : byte_region(), state(_state) {}
  byte_unit(const byte_region& _region, const data_state& _state) : byte_region(_region), state(_state) {}
  void clear() { super::clear(); state = data_state::modified; }
  std::string to_str() const;
};

struct byte_units : std::vector<byte_unit>
{
  using super = std::vector<byte_unit>;
  std::string to_str() const;
  uint64_t length() const;
};

struct block_region
{
  uint64_t lba, blocks;
  block_region(const uint64_t& _lba = 0, const uint64_t& _blocks = 0 )
    : lba(_lba), blocks(_blocks)
    {}
  void clear() { lba = blocks = 0; }
  bool empty() { return (blocks == 0); }
};
using block_regions = std::vector<block_region>;

struct block_unit : public block_region
{
  using super = block_region;
  data_state state;
  block_unit(const data_state& _state = data_state::modified) : block_region(), state(_state) {}
  block_unit(const block_region& _region, const data_state& _state = data_state::modified) : block_region(_region), state(_state) {}
  void clear() { super::clear(); state = data_state::modified; }
  std::string to_str() const;
};

//! common capacity structure
struct capacity
{
  uint64_t num_blocks; //! Number of blocks
  uint64_t block_size; //! Block size in bytes

  //! Return the capacity in bytes
  uint64_t num_bytes() const { return num_blocks * block_size; }

  capacity();
  void clear();
  bool empty() const { return (num_blocks == 0 && block_size == 0); }
};

} // namespace block
} // namespace sid

#endif // _SID_BLOCK_DATATYPES_H_

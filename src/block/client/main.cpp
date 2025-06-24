#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <block/block.hpp>
#include <common/hash.hpp>
#include <common/util.hpp>

#include "main.h"
#include <cstring>
#include <cstdlib>

using namespace sid;
using namespace sid::block;
using namespace std;

void call_using_block(block::device_ptr dev);
void call_using_scsi(scsi_disk::device_ptr dev);

int main(int argc, char* argv[])
{
  try
  {
    if ( argc < 2 )
      throw sid::exception(std::string("Usage: ") + argv[0] + std::string(" <command>"));

    const std::string cmd = argv[1];
    if ( cmd == "--list-devices" )
    {
      std::vector<std::string> args;
      for ( int i = 2; i < argc; i++ )
        args.push_back(argv[i]);
      device_details deviceDetails = (args.empty())?  device_details::get() : device_details::get(args);
      for ( const device_detail& deviceDetail : deviceDetails )
      {
        if ( !deviceDetail.isLoop() )
	        cout << deviceDetail.path << " " << sid::to_size_str(deviceDetail.size) << " " << deviceDetail.serial << " " << deviceDetail.wwn << endl;
      }
      return 0;
    }

    scsi_disk::device_info info;
    enum class CallType : uint8_t { Block, Scsi };
    CallType callType = CallType::Block;
    for ( int i = 1; i < argc; i++ ) {
      std::string_view arg(argv[i]);
      if ( arg[0] != '-' )
        info.path = arg;
      else if ( arg == "--use-block" )
        callType = CallType::Block;
      else if ( arg == "--use-scsi" )
        callType = CallType::Scsi;
      else
        throw sid::exception(std::string("Usage: ") + argv[0] + std::string(" [--use-block|--use-scsi] <device_path>"));
    }

    scsi_disk::device_ptr dev = scsi_disk::device::create(info);

    switch ( callType ) {
      case CallType::Block:
        call_using_block(dev->to_block_device_ptr());
        break;
      case CallType::Scsi:
        call_using_scsi(dev);
        break;
    }

    dev.clear();
  }
  catch (const sid::exception& e)
  {
    cout << e.what() << endl;
  }
  catch (...)
  {
    cout << "Unhandled exception" << endl;
  }

  return 0;
}

void call_using_block(block::device_ptr dev)
{
  // tests for unit ready
  if ( !dev->ready() )
    throw dev->exception();

  // gets the device capacity information
  const block::capacity capacity = dev->capacity();
  cout << "Device Capacity...: " << std::dec << capacity.bytes()
       << " (" << sid::to_size_str(capacity.bytes()) << ")" << endl;

  // gets the wwn associated with the device
  const std::string wwn = dev->wwn();
  cout << "Device USN........: " << wwn << endl;

  {
    sid::io_buffer ioBuffer(10*1024*1024);
    uint64_t totalSize = 0;
    block::io_byte_unit io_byte_unit;
    sid::hash::md5 md5;
    for ( size_t i = 0; i < 1024; i++ )
    {
      io_byte_unit.data_processed = 0;
      io_byte_unit.offset = totalSize;
      io_byte_unit.length = ioBuffer.wr_length();
      io_byte_unit.data = ioBuffer.wr_data();
      if ( ! dev->read(io_byte_unit) )
        throw dev->exception();
      //cout << "@" << totalSize << ": " << md5.get_hash(ioBuffer.wr_data(), io_byte_unit.data_processed).to_hex_str() << endl;
      totalSize += io_byte_unit.data_processed;
    }
    cout << "Device Size Read..: " << totalSize << endl;
  }
}

void call_using_scsi(scsi_disk::device_ptr dev)
{
  {
    scsi::sense sense;
    if ( ! dev->test_unit_ready(sense) )
      throw dev->exception();
  }

  uint32_t blockSize = 0;
  {
    scsi::capacity16 capacity16;
    if ( ! dev->read_capacity(capacity16) )
      throw dev->exception();
    cout << "ScsiDisk Capacity...: " << std::dec << capacity16.bytes()
         << " (" << sid::to_size_str(capacity16.bytes()) << ")" << endl;
    blockSize = capacity16.block_size;
  }

  {
    scsi::inquiry::unit_serial_number usn;
    if ( ! dev->inquiry(&usn) )
      throw dev->exception();
    cout << "ScsiDisk USN........: " << usn.serial_number << endl;
  }

  if ( blockSize > 0 )
  {
    sid::io_buffer ioBuffer(10*1024*1024);
    uint64_t totalSize = 0, totalSizeRead = 0;
    scsi::read16_vec read16_vec;
    sid::hash::md5 md5;
    for ( size_t i = 0; i < 1024; i++ )
    {
      read16_vec.clear();
      scsi::read16 read16;
      uint64_t bytesToRead = 0;
      for ( uint64_t sizeLeft = ioBuffer.wr_length(); sizeLeft != 0;
            sizeLeft -= bytesToRead, totalSize += bytesToRead)
      {
        // Determine how much data to read
        bytesToRead = std::min(sizeLeft, static_cast<uint64_t>(SCSI_DEFAULT_IO_BYTE_SIZE));
        // Fill the read16 structure
        read16.data_size_read = 0;
        read16.lba = totalSize / blockSize;
        read16.data = ioBuffer.wr_data();
        read16.transfer_length = bytesToRead / blockSize;
        // Add it to the vector
        read16_vec.push_back(read16);
      }
      if ( ! dev->read(read16_vec) )
        throw dev->exception();
      totalSizeRead += read16_vec.data_size_read();
      //cout << "@" << totalSize << ": " << md5.get_hash(ioBuffer.wr_data(), read16.data_size_read).to_hex_str() << endl;
    }
    cout << "ScsiDisk Size Read..: " << totalSize << endl;
  }
}

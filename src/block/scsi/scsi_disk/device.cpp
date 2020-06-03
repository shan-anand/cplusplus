/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.cpp
@brief SCSI disk device implementation.
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
 * @file  device.cpp
 * @brief SCSI disk device implentation of SCSI calls.
 */

#include <block/scsi/scsi_disk/datatypes.hpp>
#include <block/scsi/scsi_disk/device.hpp>
#include <common/regex.hpp>
#include <common/convert.hpp>

#include <atomic>
#include <iomanip>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include <scsi/sg.h>
#include <scsi/sg_lib.h>

using namespace std;
using namespace sid;
using namespace sid::block::scsi_disk;

#define SENSE_BUFFER_REPLY_LEN_MAX     64
#define DEF_TIMEOUT 40000       /* 40,000 millisecs == 40 seconds */

static atomic_int pack_id_count;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device_info
//
device_info::device_info()
{
}

std::string device_info::id() const
{
  return path;
}

void device_info::clear()
{
  path.clear();
}

bool device_info::empty() const
{
  return path.empty();
}

void device_info::set(const std::string& _infoStr)
{
  try
  {
    clear();

    sid::regex regEx("^(sg:/)?(/.+)$");
    sid::regex::result out;
    if ( !regEx.exec(_infoStr.c_str(), /*out*/ out) )
      throw sid::exception(std::string("Invalid device info [") + _infoStr + "]: " + regEx.error());

    this->path = out[2];
  }
  catch (const sid::exception&)
  {
    throw;
  }
  catch (...)
  {
    throw sid::exception(std::string("An unhandled exception occurred in device_info::") + __func__);
  }
}

bool device_info::set(const std::string& _infoStr, std::string& csError) noexcept
{
  bool isSuccess = false;

  try
  {
    this->set(_infoStr);

    // Set successful status
    isSuccess  = true;
  }
  catch (const sid::exception& e)
  {
    csError = e.message();
  }
  catch (...)
  {
    csError = std::string("An unhandled exception occurred in device_info::") + __func__;
  }

  return isSuccess;
}

// Get the device info as a string
std::string device_info::to_str() noexcept
{
  return std::string("sg:/") + this->path;
}

block::device_ptr device_info::create() const
{
  return device::create(*this)->to_block_device_ptr();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device
//
//! Create new SCSI-disk device object
/*static*/
device_ptr device::create(const device_info& _deviceInfo)
{
  device_ptr dev;

  try
  {
    dev = new device();
    if ( !dev )
      throw sid::exception("Failed to create scsi_disk device");

    // Set the device info object before
    dev->m_info = _deviceInfo;

    if ( !dev->p_open() )
      throw dev->exception();
  }
  catch (const sid::exception&) { throw; }
  catch (const device* p)
  {
    if ( p ) delete p;
    throw sid::exception("Failed to create smart pointer object for scsi_disk device");
  }
  catch (...)
  {
    throw sid::exception("An unknown exeception while creating scsi_disk device");
  }

  if ( !dev )
    throw sid::exception("Failed to create scsi_disk device");

  return dev;
}

device::device()
{
  m_fd = -1;
  //m_verbose = Verbose::None;
}

device::~device()
{
  p_close();
}

void device::p_close()
{
  if ( m_fd != -1 )
  {
    ::close(m_fd);
    m_fd = -1;
  }
}

bool device::p_open()
{
  bool isSuccess = false;

  try
  {
    // Close already open device
    this->p_close();

    struct stat s = {0};
    if ( ::stat(m_info.path.c_str(), &s) )
      throw sid::exception(sid::to_errno_str(errno));
    /*
      st_mode flags
         S_IFSOCK   0140000   socket
         S_IFLNK    0120000   symbolic link
         S_IFREG    0100000   regular file
         S_IFBLK    0060000   block device
         S_IFDIR    0040000   directory
         S_IFCHR    0020000   character device
         S_IFIFO    0010000   FIFO
    */
    if ( ((s.st_mode & S_IFCHR) != S_IFCHR) && ((s.st_mode & S_IFBLK) != S_IFBLK) )
      throw sid::exception("Not a character or block device");

    int ioFlags = O_RDWR;
    if ( m_info.mode.is_read_only() )
      ioFlags = O_RDONLY;
    else if ( m_info.mode.is_write_only() )
      ioFlags = O_WRONLY;

    m_fd = ::open(m_info.path.c_str(), ioFlags);
    if ( m_fd == -1 )
      throw sid::exception("Failed to open device. " + sid::to_errno_str(errno));

    p_set_non_blocking();

    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = sid::exception(e.code(), std::string(__func__) +
                                       "(" + this->id()+ "): " + e.message());
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" +
                                       this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::p_set_non_blocking()
{
  if ( m_fd < 0 ) return false;
  int flags = ::fcntl(m_fd, F_GETFL);
  if ( flags == -1 ) return false;
  flags |= O_NONBLOCK;
  return ( ::fcntl(m_fd, F_SETFL, flags) == 0 );
}

/*
//std::map<int, cb_scsi_object> io_buffer_map;
struct cb_scsi_object
{
  sid::io_buffer  buffer;
  scsi_object_ptr object;
};

void cb_test_unit_ready(const int pack_id)
{
  const cb_scsi_object& cb = cmd_map.at(pack_id);
  cb.object->set(buffer);
}
*/

int local_exec(const char* _fnName, bool _use_ioctl, int fd, sg_io_hdr& _io_hdr, block::scsi::sense& _sense)
{
  int retVal = 0;

  try
  {
    sid::io_buffer ioBufferSense(SENSE_BUFFER_REPLY_LEN_MAX);
    _io_hdr.sbp = ioBufferSense.wr_data();
    _io_hdr.mx_sb_len = ioBufferSense.wr_length();

    auto print_bytes = [&](const char* _prefix, const uint8_t* _p, int _len)->void
      {
        cout << _fnName << " " << (_prefix? _prefix:"") << " <" << _len << ">:";
        if ( _p )
        {
          for ( int i = 0; i < _len; i++ )
            cout << " " << std::setfill('0') << std::setw(2) << std::hex << ((int) _p[i]);
          cout << std::dec << endl;
        }
        else
        {
          cout << " NULL" << endl;
        }
      };

    // Print the CDB
    print_bytes("CDB", _io_hdr.cmdp, static_cast<int>(_io_hdr.cmd_len));

    if ( _use_ioctl )
    {
      retVal = ::ioctl(fd, SG_IO, &_io_hdr);
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno, "ioctl(SG_IO) failed with retVal(" + sid::to_str(retVal) + ")"));
    }
    else
    {
      retVal = ::write(fd, &_io_hdr, sizeof(_io_hdr));
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno, "write() failed with retVal(" + sid::to_str(retVal) + ")"));
      //cout << "Written: " << retVal << endl;

      memset(&_io_hdr, 0, sizeof(_io_hdr));
      retVal = ::read(fd, &_io_hdr, sizeof(_io_hdr));
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno, "read() failed with retVal(" + sid::to_str(retVal) + ")"));
    }

    // Set the return sense
    _sense.set(ioBufferSense);

    cout << (_use_ioctl? "ioctl()" : "read()") << ": retVal: " << retVal << ", ID: " << std::dec << _io_hdr.pack_id << " " << _sense.to_str() << endl;

    // Print the RESPONSE
    //print_bytes("RESPONSE", (uint8_t*) _io_hdr.dxferp, static_cast<int>(_io_hdr.dxfer_len));
  }
  catch (...)
  {
    _io_hdr.sbp = nullptr;
    _io_hdr.mx_sb_len = 0;
    // re-throw the exception
    throw;
  }

  return retVal;
}

bool device::test_unit_ready(scsi::sense& _sense)
{
  bool isSuccess = false;

  try
  {
    scsi::test_unit_ready tur;
    sid::io_buffer request = tur.get();
    sg_io_hdr io_hdr = {0};

    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmdp = request.rd_data();
    io_hdr.cmd_len = request.rd_length();
    io_hdr.dxfer_direction = SG_DXFER_NONE;
    io_hdr.timeout = 0;//DEF_TIMEOUT;
    io_hdr.pack_id = ++pack_id_count;
    io_hdr.usr_ptr = nullptr;

    local_exec(__func__, true, m_fd, io_hdr, _sense);

    if ( io_hdr.status != 0 )
      throw sid::exception(std::string("scsi command test_unit_ready failed with status ") + sid::to_str((int)io_hdr.status));

    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = sid::exception(e.code(), std::string(__func__) +
                                       "(" + this->id() + "): " + e.message());
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__)
                                       + "(" + this->id() + "): Unknown exception");
  }

  return isSuccess;
}

bool device::read_capacity(scsi::capacity16& _capacity)
{
  bool isSuccess = false;

  try
  {
    sid::io_buffer request = _capacity.get();
    sid::io_buffer response(READ_CAP16_REPLY_LEN);
    sg_io_hdr io_hdr = {0};

    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmdp = request.rd_data();
    io_hdr.cmd_len = request.rd_length();
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxferp = (void *) response.wr_data();
    io_hdr.dxfer_len = response.wr_length();
    io_hdr.timeout = 0;//DEF_TIMEOUT;
    io_hdr.pack_id = ++pack_id_count;
    io_hdr.usr_ptr = nullptr;

    scsi::sense sense;
    local_exec(__func__, true, m_fd, io_hdr, sense);

    if ( io_hdr.status != 0 )
      throw sid::exception(std::string("Failed with status ") + sid::to_str((int)io_hdr.status));

    _capacity.set(response);

    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = sid::exception(e.code(), std::string(__func__)
                                       + "(" + this->id() + "): " + e.message());
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__)
                                       + "(" + this->id() + "): Unknown exception");
  }

  return isSuccess;
}

bool device::read(scsi::read16& _read16)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = sid::exception(e.code(), std::string(__func__)
                                       + "(" + this->id() + "): " + e.message());
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__)
                                       + "(" + this->id() + "): Unknown exception");
  }

  return isSuccess;
}

bool device::write(scsi::write16& _write16)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = sid::exception(e.code(), std::string(__func__)
                                       + "(" + this->id()+ "): " + e.message());
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__)
                                       + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::inquiry(scsi::inquiry::basic* _inquiry)
{
  bool isSuccess = false;

  try
  {
    sid::io_buffer request = _inquiry->get();
    sid::io_buffer response(INQUIRY_STANDARD_REPLY_LEN);
    sg_io_hdr io_hdr = {0};

    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmdp = request.rd_data();
    io_hdr.cmd_len = request.rd_length();
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxferp = (void *) response.wr_data();
    io_hdr.dxfer_len = response.wr_length();
    io_hdr.timeout = 0;//DEF_TIMEOUT;
    io_hdr.pack_id = ++pack_id_count;
    io_hdr.usr_ptr = nullptr;

    scsi::sense sense;
    local_exec(__func__, true, m_fd, io_hdr, sense);

    if ( io_hdr.status != 0 )
      throw sid::exception(std::string("Failed with status ") + sid::to_str((int)io_hdr.status));

    _inquiry->set(response);

    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = sid::exception(e.code(), std::string(__func__)
                                       + "(" + this->id()+ "): " + e.message());
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__)
                                       + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

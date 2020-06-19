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
#include <aio.h>

#include <scsi/sg.h>

using namespace std;
using namespace sid;
using namespace sid::block::scsi_disk;

#define SENSE_BUFFER_REPLY_LEN_MAX     64
#define DEF_TIMEOUT 40000       /* 40,000 millisecs == 40 seconds */

static atomic_int pack_id_count;

///////////////////////////////////////////////////////////////////////////////////////////
//
// Definition of local::sg_io_hdr
//
namespace local
{
struct sg_io_hdr : public ::sg_io_hdr
{
  sid::io_buffer io_cdb;     //! CDB request. Ignored if "cmdp" and "cmd_len" are set
  sid::io_buffer io_xfer;    //! Data transfer. Ignored if "dxferp" and "dxfer_len" are set
  sid::io_buffer io_sense;   //! Response sense. Ignored if "sbp" and "mx_sb_len" are set

  sg_io_hdr();
  void clear();
  block::scsi::sense sense() const;
  int exec(const char* _fnName, int _fd, bool _use_ioctl);
};
} // namespace local

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
      throw sid::exception(std::string("Invalid device info [") + _infoStr + "]: "
                           + regEx.error());

    this->path = out[2];
  }
  catch (const sid::exception&)
  {
    throw;
  }
  catch (...)
  {
    throw sid::exception(std::string("An unhandled exception occurred in device_info::")
                         + __func__);
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
  m_devMode = 0;
  //m_verbose = Verbose::None;
}

device::~device()
{
  p_close();
}

bool device::is_char_device() const
{
  return S_ISCHR(m_devMode);
}

bool device::is_block_device() const
{
  return S_ISBLK(m_devMode);
}

void device::p_close()
{
  if ( m_fd != -1 )
  {
    ::close(m_fd);
    m_fd = -1;
    m_devMode = 0;
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
    if ( (! S_ISCHR(s.st_mode)) && (! S_ISBLK(s.st_mode)) )
      throw sid::exception("Not a character or block device");

    int ioFlags = O_RDWR;
    if ( m_info.mode.is_read_only() )
      ioFlags = O_RDONLY;
    else if ( m_info.mode.is_write_only() )
      ioFlags = O_WRONLY;

    m_fd = ::open(m_info.path.c_str(), ioFlags);
    if ( m_fd == -1 )
      throw sid::exception("Failed to open device. " + sid::to_errno_str(errno));

    m_devMode = s.st_mode;

    //cout << "Mode...: " << std::hex << m_devMode << std::dec
    //     << " " << (this->is_char_device()? "char_dev":"")
    //     << " " << (this->is_block_device()? "block_dev":"") << endl;

    //p_set_non_blocking();

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

bool device::test_unit_ready(scsi::sense& _sense)
{
  bool isSuccess = false;

  try
  {
    scsi::test_unit_ready tur;
    local::sg_io_hdr io_hdr;

    io_hdr.io_cdb = tur.get_cdb();
    io_hdr.dxfer_direction = SG_DXFER_NONE;
    io_hdr.pack_id = ++pack_id_count;
    //io_hdr.usr_ptr = nullptr;

    io_hdr.exec(__func__, m_fd, true);
    _sense = io_hdr.sense();

    if ( io_hdr.status != 0 )
      throw sid::exception("Failed with status " + sid::to_str((int)io_hdr.status));

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
    local::sg_io_hdr io_hdr;

    io_hdr.io_cdb = _capacity.get_cdb();
    io_hdr.io_xfer = sid::io_buffer(READ_CAP16_REPLY_LEN);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.pack_id = ++pack_id_count;
    //io_hdr.usr_ptr = nullptr;

    io_hdr.exec(__func__, m_fd, true);

    if ( io_hdr.status != 0 )
      throw sid::exception("Failed with status " + sid::to_str((int)io_hdr.status));

    _capacity.set(io_hdr.io_xfer);

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

bool device::read(scsi::read16_vec& _read16_vec)
{
  bool isSuccess = false;

  try
  {
    if ( m_fd < 0 )
      throw sid::exception("No device is open");

    if ( this->capacity().block_size == 0 )
      throw sid::exception("Block size is not set");

    if ( this->is_char_device() )
    {
      for ( scsi::read16& read16 : _read16_vec )
        p_read(read16);
    }
    else
    {
      std::vector<::aiocb> aio_vec;
      ::aiocb aio;
      for ( scsi::read16& read16 : _read16_vec )
      {
        memset(&aio, 0, sizeof(::aiocb));
        {
          aio.aio_fildes = m_fd;
          aio.aio_offset = read16.lba * this->capacity().block_size;
          aio.aio_buf = read16.data;
          aio.aio_nbytes = read16.transfer_length * this->capacity().block_size;
          aio.aio_sigevent.sigev_notify = SIGEV_NONE;
        }
        aio_vec.push_back(aio);
      }
      if ( !aio_vec.empty() )
      {
        ::aiocb** aio_list = (::aiocb**) ::calloc(aio_vec.size()+1, sizeof(::aiocb*));
        for ( size_t i = 0; i < aio_vec.size(); i++ )
          aio_list[i] = (aio_vec.data() + i);

        int retVal = ::lio_listio(LIO_WAIT, aio_list, aio_vec.size(), nullptr);
        if ( retVal != 0 )
          throw sid::exception(
            sid::to_errno_str(errno, "lio_listio() failed with retVal(" + sid::to_str(retVal) + ")"));

        for ( size_t i = 0; i < aio_vec.size(); i++ )
        {
          ::aiocb& aio = aio_vec[i];
          if ( 0 == ::aio_error(&aio) )
          {
            ssize_t bytesRead = ::aio_return(&aio);
            if ( bytesRead >= 0 )
              _read16_vec[i].data_size_read = bytesRead;
          }
        }
      }
    }
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::write(scsi::write16_vec& _write16_vec)
{
  bool isSuccess = false;

  try
  {
    if ( m_fd < 0 )
      throw sid::exception("No device is open");

    if ( this->capacity().block_size == 0 )
      throw sid::exception("Block size is not set");

    if ( this->is_char_device() )
    {
      for ( scsi::write16& write16 : _write16_vec )
        p_write(write16);
    }
    else
    {
      throw sid::exception("write not impemented for block");
    }
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id() + "): Unknown exception");
  }

  return isSuccess;
}

/*
bool device::read(scsi::read16& _read16)
{
  bool isSuccess = false;

  try
  {
    if ( m_fd < 0 )
      throw sid::exception("No device is open");

    if ( this->capacity().block_size == 0 )
      throw sid::exception("Block size is not set");

    std::vector<::aiocb> aio_vec;
    const uint32_t minDataBlock = SCSI_DEFAULT_IO_BYTE_SIZE / this->capacity().block_size;
    uint32_t remainingDataBlocks = _read16.transfer_length;
    scsi::read16 read16 = _read16;
    for ( read16.transfer_length = 0; remainingDataBlocks != 0;
          remainingDataBlocks -= read16.transfer_length )
    {
      read16.lba += read16.transfer_length;
      read16.data += (read16.transfer_length * this->capacity().block_size);
      read16.transfer_length = (remainingDataBlocks > minDataBlock)?
        minDataBlock : remainingDataBlocks;

      if ( !this->is_block_device() )
      {
        // Read the data
        p_read(read16);
        _read16.data_size_read += read16.data_size_read;
      }
      else
      {
        ::aiocb aio;
        memset(&aio, 0, sizeof(::aiocb));
        {
          aio.aio_fildes = m_fd;
          aio.aio_offset = read16.lba * this->capacity().block_size;
          aio.aio_buf = read16.data;
          aio.aio_nbytes = read16.transfer_length * this->capacity().block_size;
          aio.aio_sigevent.sigev_notify = SIGEV_NONE;
        }
        aio_vec.push_back(aio);
      }
    }

    // If aio_vec is set, perform asynchronous IO
    if ( !aio_vec.empty() )
    {
      ::aiocb** aio_list = (::aiocb**) ::calloc(aio_vec.size()+1, sizeof(::aiocb*));
      for ( size_t i = 0; i < aio_vec.size(); i++ )
        aio_list[i] = (aio_vec.data() + i);
      int retVal = ::lio_listio(LIO_WAIT, aio_list, aio_vec.size(), nullptr);
      if ( retVal != 0 )
        throw sid::exception(sid::to_errno_str(errno,
          "lio_listio() failed with retVal(" + sid::to_str(retVal) + ")"));
      for ( ::aiocb& aio : aio_vec )
      {
        if ( 0 == ::aio_error(&aio) )
        {
         ssize_t bytesRead = ::aio_return(&aio);
         if ( bytesRead >= 0 )
           _read16.data_size_read += bytesRead;
        }
      }
    }

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
*/

void device::p_read(scsi::read16& _read16)
{
  try
  {
    if ( this->is_block_device() )
    {
      // This is a block device. We can perform ::read() call directly
      //   1. Set the offet to the place where we want to read the data from
      ::lseek(m_fd, SEEK_SET, _read16.lba*this->capacity().block_size);
      //   2. Read the data at the set position
      int retVal = ::read(m_fd, _read16.data, _read16.transfer_length * this->capacity().block_size);
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno,
          "read() failed with retVal(" + sid::to_str(retVal) + ")"));

      // Set the amount of data read
      _read16.data_size_read = retVal;
    }
    else
    {
      local::sg_io_hdr io_hdr;

      io_hdr.io_cdb = _read16.get_cdb();
      if ( _read16.transfer_length == 0 )
        io_hdr.dxfer_direction = SG_DXFER_NONE;
      else
      {
        io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
        io_hdr.dxfer_len = _read16.transfer_length * this->capacity().block_size;
        io_hdr.dxferp = (void *) _read16.data;
        //io_hdr.flags |= 0x02;
      }
      io_hdr.pack_id = ++pack_id_count;
      io_hdr.timeout = DEF_TIMEOUT;
      //io_hdr.usr_ptr = nullptr;

      io_hdr.exec(__func__, m_fd, true);
      //_read16_ex.sense = io_hdr.sense();

      if ( io_hdr.status != 0 )
        throw sid::exception("Failed with status " + sid::to_str((int)io_hdr.status));

      // Set the amount of data read
      _read16.data_size_read = io_hdr.dxfer_len - io_hdr.resid;
    }
  }
  catch (const sid::exception&) { throw; }
  catch (...) { throw sid::exception(std::string(__func__) + ": Unknown exception"); }
}

/*
bool device::write(scsi::write16& _write16)
{
  bool isSuccess = false;

  try
  {
    if ( m_fd < 0 )
      throw sid::exception("No device is open");

    if ( this->capacity().block_size == 0 )
      throw sid::exception("Block size is not set");

    p_write(_write16);

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
*/

void device::p_write(scsi::write16& _write16)
{
  try
  {
    local::sg_io_hdr io_hdr;

    io_hdr.io_cdb = _write16.get_cdb();
    if ( _write16.transfer_length == 0 )
        io_hdr.dxfer_direction = SG_DXFER_NONE;
    else
    {
      io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
      io_hdr.dxfer_len = _write16.transfer_length * this->capacity().block_size;
      io_hdr.dxferp = (void *) _write16.data;
      //io_hdr.flags |= 0x02;
    }
    io_hdr.pack_id = ++pack_id_count;
    io_hdr.timeout = DEF_TIMEOUT;
    //io_hdr.usr_ptr = nullptr;

    io_hdr.exec(__func__, m_fd, true);

    if ( io_hdr.status != 0 )
      throw sid::exception("Failed with status " + sid::to_str((int)io_hdr.status));

    // Set the amount of data written
    _write16.data_size_written = io_hdr.dxfer_len - io_hdr.resid;
  }
  catch (const sid::exception&) { throw; }
  catch (...) { throw sid::exception(std::string(__func__) + ": Unknown exception"); }
}

bool device::inquiry(scsi::inquiry::basic* _inquiry)
{
  bool isSuccess = false;

  try
  {
    local::sg_io_hdr io_hdr;

    io_hdr.io_cdb = _inquiry->get_cdb();
    io_hdr.io_xfer = sid::io_buffer(INQUIRY_STANDARD_REPLY_LEN);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.pack_id = ++pack_id_count;
    //io_hdr.usr_ptr = nullptr;

    io_hdr.exec(__func__, m_fd, true);

    if ( io_hdr.status != 0 )
      throw sid::exception(std::string("Failed with status ")
                           + sid::to_str((int)io_hdr.status));

    _inquiry->set(io_hdr.io_xfer);

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of local::sg_io_hdr
//
local::sg_io_hdr::sg_io_hdr()
{
  clear();
}

void local::sg_io_hdr::clear()
{
  memset(dynamic_cast<::sg_io_hdr*>(this), 0, sizeof(::sg_io_hdr));
  this->interface_id = 'S';
  if ( io_sense.empty() )
    io_sense.reserve(SENSE_BUFFER_REPLY_LEN_MAX);
}

block::scsi::sense local::sg_io_hdr::sense() const
{
  block::scsi::sense s;
  if ( ! io_sense.empty() )
    s.set(io_sense);
  return s;
}

int local::sg_io_hdr::exec(const char* _fnName, int _fd, bool _use_ioctl)
{
  int retVal = 0;

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

  try
  {
    // Request CDB
    if ( this->cmdp == nullptr )
    {
      this->cmdp = this->io_cdb.rd_data();
      this->cmd_len = this->io_cdb.rd_length();
    }
    // Response buffer (if exists)
    if ( this->dxferp == nullptr && this->io_xfer.capacity() > 0 )
    {
      this->dxferp = (void *) this->io_xfer.wr_data();
      this->dxfer_len = this->io_xfer.wr_length();
    }
    // Sense buffer output
    if ( this->sbp == nullptr && this->io_sense.capacity() > 0 )
    {
      this->sbp = this->io_sense.wr_data();
      this->mx_sb_len = this->io_sense.wr_length();
    }
    if ( this->timeout == 0 )
      this->timeout = 0;//DEF_TIMEOUT;

    // Print the CDB
    //print_bytes("CDB", this->cmdp, static_cast<int>(this->cmd_len));

    if ( _use_ioctl )
    {
      retVal = ::ioctl(_fd, SG_IO, dynamic_cast<::sg_io_hdr*>(this));
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno,
          "ioctl(SG_IO) failed with retVal(" + sid::to_str(retVal) + ")"));
    }
    else
    {
      retVal = ::write(_fd, dynamic_cast<::sg_io_hdr*>(this), sizeof(::sg_io_hdr));
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno,
          "write() failed with retVal(" + sid::to_str(retVal) + ")"));
      //cout << "Written: " << retVal << endl;

      memset(dynamic_cast<::sg_io_hdr*>(this), 0, sizeof(::sg_io_hdr));
      retVal = ::read(_fd, dynamic_cast<::sg_io_hdr*>(this), sizeof(::sg_io_hdr));
      if ( retVal < 0 )
        throw sid::exception(sid::to_errno_str(errno,
          "read() failed with retVal(" + sid::to_str(retVal) + ")"));
    }

    /*
    cout << (_use_ioctl? "ioctl()" : "read()") << ": retVal: " << retVal
         << ", ID: " << std::dec << this->pack_id
         << " " << this->sense().to_str() << endl;
    */

    // Print the RESPONSE
    //print_bytes("RESPONSE", (uint8_t*) this->dxferp, static_cast<int>(this->dxfer_len));
  }
  catch (...)
  {
    // re-throw the exception
    throw;
  }

  return retVal;
}

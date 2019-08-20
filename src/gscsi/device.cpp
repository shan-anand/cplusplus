/**
 * @file Device.cpp
 * @brief Implementation of Device API. C++ wrapper for libgsutil2.
 *        Send SCSI commands to sg device.
 */
#include "local.h"
#include <gscsi/device.hpp>
#include <Common/io_buffer.hpp>
#include <scsi/datatypes.hpp>

#include <atomic>
#include <string.h>

#include <sys/ioctl.h>

#include <scsi/sg.h>
#include <scsi/sg_lib.h>
#include <scsi/sg_cmds.h>

using namespace std;
using namespace sid::GScsi;

#define SENSE_BUFFER_REPLY_LEN_MAX     512

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of DeviceInfo_t class
DeviceInfo_t::DeviceInfo_t()
{
  clear();
}

DeviceInfo_t::~DeviceInfo_t()
{
}

bool DeviceInfo_t::set(const std::string& infoStr, std::string* pcsError = nullptr)
{
  bool bStatus = false;
  std::string csError;

  try
  {
    clear();

    std::map<size_t, std::string> out;
    RegEx regEx("^(sg:/)?(/.+)$");
    if ( !regEx.Exec(infoStr.c_str(), /*out*/ out) )
      throw std::string("Invalid device path [") + infoStr + "]: " + regEx.errStr();

    this->path = out[2];

    // Set successful status
    bStatus  = true;
  }
  catch (const std::string& csErr) { csError = csErr; }
  catch (...) { csError = std::string("An unhandled exception occurred in DeviceInfo_t::") + __func__; }

  if ( !bStatus )
  {
    if ( !pcsError )
      throw csError;
    *pcsError  = csError;
  }
  return bStatus;
}

void DeviceInfo_t::clear()
{
  path.clear();
}

bool DeviceInfo_t::empty() const
{
  return path.empty();
}

std::string DeviceInfo_t::id() const
{
  return path;
}

std::string DeviceInfo_t::toString() const
{
  std::ostringstream out;
  if ( !this->path.empty() )
  {
    out << "sg:/" << (( this->path[0] != '/' )? "/":"") << this->path;
  }
  return out.str();
}

Scsi::Device_s DeviceInfo_t::create() const
{
  return Device_t::create(*this, 0)->base_ptr();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of Device_t class
void Device_t::Info::clear()
{
  memset(this, 0, sizeof(Device_t::Info));
}

Device_t::Device()
  : m_path(std::string()),
    m_fd(-1),
    m_fpstream(nullptr),
    m_res(0),
    m_noisy(false),
    m_verboseLevel(0)
{
}

Device_t::~Device()
{
  p_close();
  clear_warnings_strm();
}

struct Connection_t
{
  std::string devicePath;
};

/*static*/
Device_s Device_t::create(const DeviceInfo_t& info, int verboseLevel/*= 0*/)
{
  Device_s sg_device;

  try
  {
    // Create a Device object
    sg_device = new Device_t();
    // If the connection object is empty, the object was not created successfully. So, throw an exception.
    if ( sg_device.empty() )
      throw std::string("Failed to allocate device object");

    // Create the device information object
    sg_device->m_deviceInfo = new DeviceInfo_t(info);
    if ( sg_device->m_deviceInfo.empty() )
      throw std::string("Failed to allocate device information object");

    // Set the verbose level
    sg_device->setVerboseLevel(verboseLevel);

    /////////////////////////////////////////////////////////
    // Open the device
    if ( !sg_device->p_open(info.path) )
      throw sg_device->m_error;

    // Set the default warning stream to standard output
    sg_device->set_warnings_strm(STDOUT_FILENO);

    // set internal variables
    sg_device->m_error.clear();
    /////////////////////////////////////////////////////////
  }
  catch ( const std::string& csErr )
  {
    /* Rethrow string exception with a prefix */
    throw std::string("Device object creation failed: ") + csErr;
  }
  catch (...)
  {
    throw std::string("Device object creation failed: An unhandled exception occurred");
  }

  // Guarantee object creation
  if ( sg_device.empty() )
    throw std::string("Device object cannot be created for unknown reason");

  // Return the smart pointer object. Guarantees that the object is not a null pointer
  return sg_device;
}

void Device_t::p_close()
{
  if ( m_fd != -1 )
  {
    //::close(m_fd);
    sg_cmds_close_device(m_fd);
    m_fd = -1;
  }
  m_path.clear();
  m_info.clear();
}

void Device_t::setVerboseLevel(int level)
{
  if ( level <= 0 )
  {
    m_noisy = false;
    m_verboseLevel = 0;
  }
  else
  {
    m_noisy = true;
    m_verboseLevel = level;
  }
}

std::string Device_t::resultStr() const
{
  std::string resStr;
  if ( m_res != 0 )
  {
    int res = ( m_res < 0 )? -m_res : m_res;
    resStr = local::toString(res) + std::string(": ") + safe_strerror(res);
  }
  return resStr;
}

bool Device_t::set_warnings_strm(const std::string& filePath)
{
  bool bStatus = false;

  try
  {
    this->clear_warnings_strm();

    m_fpstream = fopen(filePath.c_str(), "w");
    if ( !m_fpstream )
      throw std::string("Unable to open file ") + filePath;

    sg_set_warnings_strm(m_fpstream);
    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::set_warnings_strm(int fd)
{
  bool bStatus = false;

  try
  {
    this->clear_warnings_strm();

    m_fpstream = fdopen(dup(fd), "w");
    if ( !m_fpstream )
      throw std::string("Unable to open fd ") + local::toString(fd);

    sg_set_warnings_strm(m_fpstream);
    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::clear_warnings_strm()
{
  if ( m_fpstream )
  {
    fclose(m_fpstream);
    m_fpstream = nullptr;
    //sg_set_warnings_strm(m_fpstream);
  }
  return true;
}

bool Device_t::p_open(const std::string& devicePath)
{
  bool bStatus = false;

  try
  {
    // Close already open device
    this->p_close();

    struct stat s = {0};
    if ( stat(devicePath.c_str(), &s) )
      throw std::string("The device ") + devicePath + " does not exist, Error: " + local::toString(errno);
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
    if ( ((s.st_mode & S_IFCHR) != S_IFCHR) && ((s.st_mode & S_IFBLK) !=S_IFBLK) )
      throw std::string("The given path ") + devicePath + " is not a character or block device.";
             
    //m_fd = ::open(devicePath.c_str(), 0);
    //m_fd = sg_cmds_open_device(devicePath.c_str(), 0, m_noisy);
    m_fd = sg_cmds_open_flags(devicePath.c_str(), O_RDWR, m_noisy);
    if ( m_fd == -1 )
      throw std::string("Failed to open device ") + devicePath + ", Error: " + local::toString(errno);

    m_path = devicePath;
    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::test_unit_ready()
{
  bool bStatus = false;

  try
  {
    if ( this->empty() )
      throw std::string("No device is opened");

    m_res = sg_ll_test_unit_ready(m_fd, 0, m_noisy, m_verboseLevel);
    switch ( m_res )
    {
    case 0: break; // Success
    case -1: throw std::string("Failed with -1");
    default: throw resultStr();
    }

    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::read_capacity(Scsi::Capacity10_t& capacity10)
{
  bool bStatus = false;

  try
  {
    if ( this->empty() )
      throw std::string("No device is opened");

    capacity10.clear();

    sid::io_buffer_t response(READ_CAP10_REPLY_LEN);
    m_res = sg_ll_readcap_10(m_fd, 0, 0, response.wr_data(), response.wr_length(), m_noisy, m_verboseLevel);
    switch ( m_res )
    {
    case 0: break; // Success
    case -1: throw std::string("Failed with -1");
    default: throw resultStr();
    }

    capacity10.set(response);

    m_info.block_size = capacity10.block_size;
    m_info.num_blocks = capacity10.num_blocks;

    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::read_capacity(Scsi::Capacity16_t& capacity16)
{
  bool bStatus = false;

  try
  {
    if ( this->empty() )
      throw std::string("No device is opened");

    capacity16.clear();

    sid::io_buffer_t response(READ_CAP16_REPLY_LEN);
    m_res = sg_ll_readcap_16(m_fd, 0, 0, response.wr_data(), response.wr_length(), m_noisy, m_verboseLevel);
    switch ( m_res )
    {
    case 0: break; // Success
    case -1: throw std::string("Failed with -1");
    default: throw resultStr();
    }

    capacity16.set(response);

    m_info.block_size = capacity16.block_size;
    m_info.num_blocks = capacity16.num_blocks;

    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::inquiry(Scsi::Inquiry::Standard_t& inquiry)
{
  bool bStatus = false;

  try
  {
    if ( this->empty() )
      throw std::string("No device is opened");

    inquiry.clear();

    sid::io_buffer_t response(INQUIRY_STANDARD_REPLY_LEN+512);
    m_res = sg_ll_inquiry(m_fd, 0 /*cmddt*/, 0 /*evpd*/, 0 /*pg_op*/, response.wr_data(), response.wr_length(), m_noisy, m_verboseLevel);
    switch ( m_res )
    {
    case 0: break; // Success
    case -1: throw std::string("Failed with -1");
    default: throw resultStr();
    }

    bStatus = inquiry.set(response);
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::p_inquiry(Scsi::Inquiry::BasicVPD_t* vpd)
{
  bool bStatus = false;

  try
  {
    if ( !vpd )
      throw std::string("Inquiry pointer cannot be null");

    if ( this->empty() )
      throw std::string("No device is opened");

    sid::io_buffer_t response(512); // Good enough for most Inquiry

    vpd->clear();

    m_res = sg_ll_inquiry(m_fd, 0 /*cmddt*/, 1 /*evpd*/,
                          static_cast<int>(vpd->page_code) /*pg_op*/,
                          response.wr_data(), response.wr_length(), m_noisy, m_verboseLevel);
    switch ( m_res )
    {
    case 0: break; // Success
    case -1: throw std::string("Failed with -1");
    default: throw resultStr();
    }

    bStatus = vpd->set(response);
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

bool Device_t::inquiry(Scsi::Inquiry::UnitSerialNumber_t& inq)
{
  return p_inquiry(&inq);
}

bool Device_t::inquiry(Scsi::Inquiry::SupportedVPDPages_t& inq)
{
  return p_inquiry(&inq);
}

static atomic_int pack_id_count;
#define DEF_TIMEOUT 40000       /* 40,000 millisecs == 40 seconds */
#include <iostream>
#include <iomanip>

int Device_t::sg_reserved_size()
{
  if ( m_info.sg_reserved_size == 0 )
  {
    if ( ioctl(m_fd, SG_GET_RESERVED_SIZE, &m_info.sg_reserved_size ) < 0)
      throw std::string("Failed in ioctl:SG_GET_RESERVED_SIZE. errno = ") + local::toString(errno);
  }
  return m_info.sg_reserved_size;
}

int Device_t::sg_set_reserved_size(int rs)
{
  if ( ioctl(m_fd, SG_SET_RESERVED_SIZE, &rs) < 0 )
    throw std::string("Failed in ioctl:SG_SET_RESERVED_SIZE(") + local::toString(rs) + "). errno = " + local::toString(errno);
  m_info.sg_reserved_size = rs;
  return m_info.sg_reserved_size;
}

bool Device_t::read(Scsi::Read16_t& read16, Scsi::Sense_t* sense/* = nullptr */)
{
  bool bStatus = false;

  try
  {
    if ( this->empty() )
      throw std::string("No device is opened");

    if ( this->block_size() == 0 )
      throw std::string("Block size is not set");

    /*
    auto print = [&](const Scsi::Read16& r16)->void
      {
        cout << " Read16:"
             << " lba=" << (r16.range.lba * this->block_size())
             << " len=" << r16.range.n_blocks
             << " data=" << std::hex << r16.data
             << endl;
      };
    */
    uint64_t trLen = read16.range.bytes(this->block_size());
    if ( this->sg_reserved_size() < (int) trLen )
      sg_set_reserved_size((int) trLen);

    //print(read16);
    if ( trLen <= SCSI_DEFAULT_IO_SIZE )
    {
      p_read(read16, sense);
      return true;
    }

    const uint64_t defDataBlocks = SCSI_DEFAULT_IO_SIZE / this->block_size();
    uint64_t remainingBlocks = read16.range.n_blocks;
    Scsi::Read16 r16 = read16;
    for ( r16.range.n_blocks = 0; remainingBlocks != 0; remainingBlocks -= r16.range.n_blocks )
    {
      r16.range.lba += r16.range.n_blocks;
      r16.data += r16.range.bytes(this->block_size());
      r16.range.n_blocks = (remainingBlocks > defDataBlocks)? defDataBlocks : remainingBlocks;
      //cout << "     R: " << static_cast<const void*>(r16.data) << " (" << r16.range.lba << ", " << r16.range.n_blocks << ") (" << (r16.range.lba*block_size()) << ", " << r16.range.bytes(block_size()) << ")" << endl;
      //print(r16);
      // Read the data
      p_read(r16, sense);
      read16.bytes_read += r16.bytes_read;
    }
    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

void Device_t::p_read(Scsi::Read16_t& read16, Scsi::Sense_t* sense/* = nullptr */)
{
  try
  {
    sid::io_buffer_t request = read16.get();
    sid::io_buffer_t ioBufferSense(SENSE_BUFFER_REPLY_LEN_MAX);

    sg_io_hdr io_hdr = {0};
    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = request.wr_length();
    io_hdr.cmdp = request.wr_data();

    if ( read16.range.n_blocks == 0 )
        io_hdr.dxfer_direction = SG_DXFER_NONE;
    else
    {
      io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
      io_hdr.dxfer_len = read16.range.bytes(this->block_size());
      io_hdr.dxferp = (void *) read16.data;
      //io_hdr.flags |= 0x02;
    }
    io_hdr.sbp = ioBufferSense.wr_data();
    io_hdr.mx_sb_len = ioBufferSense.wr_length();
    io_hdr.timeout = DEF_TIMEOUT;
    io_hdr.pack_id = pack_id_count++;

    if ( ioctl(m_fd, SG_IO, &io_hdr) < 0)
    {
      if ( ENOMEM == errno )
        throw std::string("ENOMEM");
      throw std::string("reading (SG_IO) on sg device, error");
    }

    if ( sense )
      sense->set(ioBufferSense);

    if ( io_hdr.sb_len_wr > 0 )
    {
      cout << "sb_len_wr: (" << local::toString((int) io_hdr.sb_len_wr) << ")";
      for ( size_t i = 0; i < (size_t) io_hdr.sb_len_wr; i++ )
        cout << " " << setfill('0') << setw(2) << std::hex << ((int) io_hdr.sbp[i]);
      cout << endl;
    }
    if ( io_hdr.status != 0 )
      throw std::string("scsi command read failed with status ") + local::toString((int)io_hdr.status);

    read16.bytes_read = io_hdr.dxfer_len - io_hdr.resid;
    //read16.bytes_read = io_hdr.dxfer_len;
  }
  catch (const std::string& err) { throw; }
  catch (...) { throw std::string("Failed with unknown exception"); }
}

bool Device_t::write(Scsi::Write16_t& write16, Scsi::Sense_t* sense/* = nullptr */)
{
  bool bStatus = false;

  try
  {
    if ( this->empty() )
      throw std::string("No device is opened");

    if ( this->block_size() == 0 )
      throw std::string("Block size is not set");

    uint64_t trLen = write16.range.bytes(this->block_size());
    if ( this->sg_reserved_size() < (int) trLen )
      sg_set_reserved_size((int) trLen);

    if ( trLen <= SCSI_DEFAULT_IO_SIZE )
    {
      p_write(write16, sense);
      return true;
    }

    const uint64_t defDataBlocks = SCSI_DEFAULT_IO_SIZE / this->block_size();
    uint64_t remainingBlocks = write16.range.n_blocks;
    Scsi::Write16 w16 = write16;
    for ( w16.range.n_blocks = 0; remainingBlocks != 0; remainingBlocks -= w16.range.n_blocks )
    {
      w16.range.lba += w16.range.n_blocks;
      w16.data += w16.range.bytes(this->block_size());
      w16.range.n_blocks = (remainingBlocks > defDataBlocks)? defDataBlocks : remainingBlocks;
      //cout << "     W: " << static_cast<const void*>(w16.data) << " (" << w16.range.lba << ", " << w16.range.n_blocks << ") (" << (w16.range.lba*block_size()) << ", " << w16.range.bytes(block_size()) << ")" << endl;
      // Write the data
      p_write(w16, sense);
      write16.bytes_written += w16.bytes_written;
    }

    bStatus = true;
  }
  catch (const std::string& err)
  {
    m_error = std::string("Device_t::") + __func__ + ": " + err;
  }
  catch (...)
  {
    m_error = std::string("Device_t::") + __func__ + ": Failed with unknown exception";
  }

  return bStatus;
}

void Device_t::p_write(Scsi::Write16_t& write16, Scsi::Sense_t* sense/* = nullptr */)
{
  try
  {
    sid::io_buffer_t request = write16.get();
    sid::io_buffer_t ioBufferSense(SENSE_BUFFER_REPLY_LEN_MAX);

    sg_io_hdr io_hdr = {0};
    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = request.wr_length();
    io_hdr.cmdp = request.wr_data();

    if ( write16.range.n_blocks == 0 )
        io_hdr.dxfer_direction = SG_DXFER_NONE;
    else
    {
      io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
      io_hdr.dxfer_len = write16.range.bytes(this->block_size());
      io_hdr.dxferp = (void *) write16.data;
      //io_hdr.flags |= 0x02;
    }
    io_hdr.sbp = ioBufferSense.wr_data();
    io_hdr.mx_sb_len = ioBufferSense.wr_length();
    io_hdr.timeout = DEF_TIMEOUT;
    io_hdr.pack_id = pack_id_count++;

    if ( ioctl(m_fd, SG_IO, &io_hdr) < 0)
    {
      if ( ENOMEM == errno )
        throw std::string("ENOMEM");
      throw std::string("writing (SG_IO) on sg device, error");
    }

    if ( sense )
      sense->set(ioBufferSense);

    if ( io_hdr.sb_len_wr > 0 )
    {
      cout << "sb_len_wr: (" << local::toString((int) io_hdr.sb_len_wr) << ")";
      for ( size_t i = 0; i < (size_t) io_hdr.sb_len_wr; i++ )
        cout << " " << setfill('0') << setw(2) << std::hex << ((int) io_hdr.sbp[i]);
      cout << endl;
    }
    if ( io_hdr.status != 0 )
      throw std::string("scsi command write failed with status ") + local::toString((int)io_hdr.status);

    write16.bytes_written = io_hdr.dxfer_len - io_hdr.resid;
  }
  catch (const std::string& err) { throw; }
  catch (...) { throw std::string("Failed with unknown exception"); }
}

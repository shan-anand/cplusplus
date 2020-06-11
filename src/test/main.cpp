#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <signal.h>
#include <unistd.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aio.h>
#include <sys/wait.h>

#include <common/exception.hpp>
#include <common/convert.hpp>

using namespace std;
using namespace sid;

std::string get_file_contents(const std::string& _filePath);

int main(int argc, char* argv[])
{
  
  int status = -1;
  
  try
  {
    if ( argc < 2 )
      throw std::string("Need atleast one argument");

    std::string data = get_file_contents(argv[1]);
    cout << "File size: " << data.size() << endl;
    //cout << data << endl;
    status = 0;
  }
  catch (const sid::exception& _e)
  {
    cerr << "Error: " << _e.what() << endl;
  }
  catch (...)
  {
    cerr << "Error: An unhandled exception occurred" << endl;
  }

  return status;
}

std::string get_file_contents(const std::string& _filePath)
{
  std::string out;
  int fd = -1;

  try
  {
    fd = ::open(_filePath.c_str(), O_RDONLY);
    if ( fd == -1 )
      throw sid::exception(sid::to_errno_str(errno, "Failed to open file"));

    uint8_t buf[128*1024];
    ::aiocb io;
    memset(&io, 0, sizeof(::aiocb));
    {
      io.aio_fildes = fd;
      io.aio_buf = buf;
      io.aio_nbytes = sizeof(buf);
      io.aio_sigevent.sigev_notify = SIGEV_NONE;
    }
    //io.aio_offset = 1000204886016;
    io.aio_offset = 1000100000000;
    //io.aio_offset = 1000204887000;
    //int i = 0;
    uint64_t sizeRead = 0;
    ssize_t outSize = 0;
    do
    {
      int retVal = ::aio_read(&io);
      if ( retVal == -1 )
        throw sid::exception(sid::to_errno_str(errno, "aio_read() failed"));

      while ( (retVal = ::aio_error(&io)) == EINPROGRESS );
      if ( retVal == ECANCELED )
        throw sid::exception("The read operation was cancelled");
      else if ( retVal > 0 )
      {
        errno = retVal;
        throw sid::exception(sid::to_errno_str(errno, "aio_error()"));
      }

      outSize = aio_return(&io);
      if ( outSize == -1 )
        throw sid::exception(sid::to_errno_str(errno, "aio_return() failed"));

      io.aio_offset += outSize;
      sizeRead += outSize;
      //out.append((const char*) io.aio_buf, outSize);
      //if ( ++i > 33554432 ) break;
      //if ( i % 1000 == 0 ) cout << i << endl;
      //if ( ++i > 33554 ) break;
    }
    while ( outSize == (ssize_t) sizeof(buf) );
    cout << "Total size read....: " << sizeRead << endl;
  }
  catch (...)
  {
    throw;
  }

  if ( fd >= 0 )
  {
    ::close(fd);
    fd = -1;
  }
  return out;
}

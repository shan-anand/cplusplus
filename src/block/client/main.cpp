#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <block/block.hpp>

#include "main.h"
#include <cstring>
#include <cstdlib>

using namespace sid;
using namespace sid::block;
using namespace std;

int main(int argc, char* argv[])
{
  try
  {
    if ( argc < 2 )
      throw sid::exception("Device path is required as a paremeter");

    scsi_disk::device_info info;
    info.path = argv[1];

    scsi_disk::device_ptr dev = scsi_disk::device::create(info);
    {
      scsi::sense sense;
      if ( ! dev->test_unit_ready(sense) )
        throw dev->exception();
    }

    {
      scsi::capacity16 capacity;
      if ( ! dev->read_capacity(capacity) )
        throw dev->exception();
      cout << "Capacity...: " << std::dec << capacity.bytes() << endl;
    }

    {
      scsi::inquiry::unit_serial_number usn;
      if ( ! dev->inquiry(&usn) )
        throw dev->exception();
      cout << "USN........: " << usn.serial_number << endl;
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

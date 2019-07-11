//////////////////////////////////////////////////////
//
// response.cpp
//
//////////////////////////////////////////////////////

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief HTTP library implementation in C++
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

#include "http/http.hpp"
#include "common/convert.hpp"
#include <sstream>

using namespace sid;
using namespace sid::http;

using string_map = std::map< std::string, std::string >;

struct data_chunk
{
  int         length;
  string_map  params;
  std::string data;

  data_chunk() { clear(); }
  void clear() { length = 0; params.clear(); data.clear(); }
};

/**
 * @class IResponseCallback
 * @brief Response callback classed used during response parsing
 *
 * This is an internal class that is used only in this file.
 */
class IResponseCallback
{
public:
  static IResponseCallback* get_singleton();

  bool is_valid(const connection_ptr _conn, const status& _status, response& _response);
  bool is_valid(const connection_ptr _conn, const header& _header, response& _response);
  bool is_valid(const connection_ptr _conn, const headers& _headers, response& _response);
  bool is_valid(const connection_ptr _conn, const data_chunk& _chunk, response& _response, bool _isStart);
  bool is_valid(const connection_ptr _conn, response& _response);

private: // should not be instantiated separately
  IResponseCallback() {}
};

/**
 * @class response_handler
 * @brief Class definition handling response parsing.
 *
 * This is an internal class that is used only in this file.
 */
class response_handler
{
public:
response_handler(connection_ptr _conn) : m_conn(_conn)
  {
    m_endOfStatus = false;
    m_endOfHeaders = false;
    m_endOfData = false;
    m_forceStop = false;
    m_pos = 0;
    m_contentLength = 0;
    m_encoding = http::transfer_encoding::none;
    m_keepAlive = false;
    m_chunk.clear();
    m_chunkToBeRead = 0;
    pCallback = IResponseCallback::get_singleton();
  }

  
public:
  void parse(const char* buffer, int nread, const method& _requestMethod, /*in/out*/ response& _response) throw (std::string);
  bool is_end_of_data() const { return m_endOfData; }
  bool is_force_stop() const { return m_forceStop; }
  bool continue_parsing() const { return ( ! (m_endOfData || m_forceStop) ); } 

private:
  bool parse_status(/*in/out*/ response& _response);
  bool parse_headers(const method& _requestMethod, /*in/out*/ response& _response);
  void parse_data_normal(/*in/out*/ response& _response);
  void parse_data_chunked(/*in/out*/ response& _response);

private:
  http::connection_ptr       m_conn;          //! Pointer to the connection object
  std::string                m_csResponse;    //! Response data (holds binary data too)
  bool                       m_endOfStatus;   //! Indicates end of status has been reached
  bool                       m_endOfHeaders;  //! Indicates end of headers has been reached
  bool                       m_forceStop;     //! Is forcefully stopped
  size_t                     m_pos;           //! Indicates current position of parsing
  bool                       m_endOfData;     //! Indicates end of data has been reached
  size_t                     m_contentLength; //! Content length
  http::transfer_encoding    m_encoding;      //! Transfer encoding
  bool                       m_keepAlive;     //! Is keep alive set?
  data_chunk                 m_chunk;         //! Current chunk object (if response is in chunks)
  int                        m_chunkToBeRead; //! Remaining chunk bytes to be read (if response is in chunks)
  IResponseCallback*         pCallback;       //! Response callback in case something needs to be processed
};

//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of response class
//
//////////////////////////////////////////////////////////////////////////////////////
response::response()
{
  clear();
}

void response::clear()
{
  // clear all the response objects
  version.clear();
  status = http::status_code::NotFound;
  headers.clear();
  content.clear();
  content_is_file_path = false;
  error.clear();
}

std::string response::to_str(bool _showContent/* = true*/) const
{
  std::ostringstream out;

  out << this->version.to_str() << " "
      << this->status.to_str()
      << CRLF;
  out << this->headers.to_str();
  out << CRLF; // marks end of data
  if ( ! this->content_is_file_path )
  {
    if ( _showContent )
      out << this->content;
  }
  else
    out << "File: " << this->content;
  return out.str();
}

bool response::send(connection_ptr _conn)
{
  bool bStatus = false;

  try
  {
    this->error.clear();

    if ( _conn.empty() || ! _conn->is_open() )
      throw std::string("Connection is not established");

    ////////////////////////////////////////////////////
    // NOTE: BASIC IMPLEMENTATION. TO BE CHANGED LATER.
    ////////////////////////////////////////////////////
    std::string csResponse = this->to_str();

    ssize_t written = _conn->write(csResponse.c_str(), csResponse.length());
    if ( written < 0 || csResponse.length() != static_cast<size_t>(written) )
      throw std::string("Failed to write data");
    ////////////////////////////////////////////////////

    // set the return status to true
    bStatus = true;
  }
  catch ( const std::string& csErr )
  {
    this->error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    this->error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

bool response::recv(connection_ptr _conn, const method& _requestMethod)
{
  bool bStatus = false;
  char buffer[32*1024] = {0};
  int nread = 0;

  try
  {
    this->error.clear();

    if ( _conn.empty() || ! _conn->is_open() )
      throw std::string("Connection is not established");

    response_handler rd(_conn);
    while ( rd.continue_parsing() && (nread = _conn->read(buffer, sizeof(buffer)-1)) > 0 )
      rd.parse(buffer, nread, _requestMethod, /*in/out*/ *this);

    // set the return status to true
    bStatus = true;
  }
  catch ( const std::string& csErr )
  {
    this->error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    this->error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

void response::set(const std::string& input)
{
  size_t pos1, pos2;
  size_t eol;
  std::string headerStr;

  try
  {
    // HTTP/1.x <CODE> <CODESTR>\r\n
    pos1 = 0;
    eol = input.find(CRLF, pos1);
    pos2 = input.find(' ', pos1);
    if ( pos2 == std::string::npos || pos2 > eol )
      throw std::string("Invalid response from server");
    version = http::version::get(input.substr(pos1, (pos2-pos1)));
    pos1 = pos2+1;

    pos2 = input.find(CRLF, pos1);
    if ( pos2 == std::string::npos || pos2 > eol )
      throw std::string("Invalid response from server");
    status = http::status::get(input.substr(pos1, (pos2-pos1)));
    pos1 = pos2+2;

    // Followed by response headers
    do
    {
      pos2 = input.find(CRLF, pos1);
      if ( pos2 == std::string::npos ) throw std::string("Invalid response from server");
      headerStr = input.substr(pos1, (pos2-pos1));
      pos1 = pos2+2;
      if ( headerStr.empty() ) break;
      this->headers.add(headerStr);
    }
    while (true);

    // Followed by data
    this->content = input.substr(pos1);
  }
  catch (const std::string& csErr)
  {
    throw;
  }
  catch (...)
  {
    throw std::string("Unhandled exception in response::set");
  }
}

/////////////////////////////////////////////////////////////////////////////

bool response_handler::parse_status(/*in/out*/ response& _response)
{
  std::string line;

  m_pos = 0;
  if ( ! http::get_line(m_csResponse, m_pos, line) )
    throw std::string("Invalid response received");

  // HTTP/1.x <CODE> <CODESTR>\r\n
  size_t pos1 = 0;
  size_t pos2 = line.find(' ', pos1);
  if ( pos2 == std::string::npos )
    throw std::string("Invalid response from server");
  _response.version = http::version::get(line.substr(pos1, (pos2-pos1)));
  _response.status = http::status::get(line.substr(pos2+1));
  //cerr << "End of Status" << endl;

  m_endOfStatus = true;

  if ( pCallback && ! pCallback->is_valid(m_conn, _response.status, _response) )
    m_forceStop = true; // Force stop

  return continue_parsing();
}

bool response_handler::parse_headers(const method& _requestMethod, /*in/out*/ response& _response)
{
  std::string line;

  if ( m_csResponse.size() == m_pos )
    return true;

  while ( ! m_endOfHeaders )
  {
    if ( ! http::get_line(m_csResponse, m_pos, line) )
    {
      // give the benefit of doubt, may be we aren't finised with Headers yet
      // we'll try to continue after next read
      m_csResponse = m_csResponse.substr(m_pos);
      m_pos = 0;
      //m_forceStop = true; // Force stop
      break;
    }
    if ( ! line.empty() )
    {
      const http::header& header = _response.headers.add(line);
      if ( pCallback && ! pCallback->is_valid(m_conn, header, _response) )
      {
        m_forceStop = true; // Force stop
        break;
      }
    }
    else
    {
      m_endOfHeaders = true;
      //cerr << "End of Headers" << endl;

      if ( pCallback && ! pCallback->is_valid(m_conn, _response.headers, _response) )
      {
        m_forceStop = true; // Force stop
        break;
      }

      if ( _requestMethod == http::method_type::head )
      {
        m_endOfData = true; // END OF DATA
        break;
      }

      // start of data
      m_csResponse = m_csResponse.substr(m_pos);
      m_pos = 0;

      bool bFound;
      m_contentLength = _response.headers.content_length(&bFound);
      m_encoding = _response.headers.transfer_encoding();
      m_keepAlive = ( _response.headers.connection() == http::header_connection::keep_alive );
      break;
    }
  } // finished with all headers
  return continue_parsing();
}

void response_handler::parse_data_normal(/*in/out*/ response& _response)
{
  size_t copyLen = m_csResponse.length() - m_pos;
  _response.content.append(m_csResponse, m_pos, copyLen);
  //cerr << "Len: " << m_contentLength << "-" << _response.content.length() << endl;
  if ( _response.content.length() >= m_contentLength )
    m_endOfData = true; // END OF DATA
  else
  {
    m_csResponse.clear(); m_pos = 0;
  }

  return;
}

void response_handler::parse_data_chunked(/*in/out*/ response& _response)
{
  std::string line;
  bool bContinueLoop = true;

  while ( bContinueLoop && !m_forceStop && !m_endOfData )
  {
    if ( m_chunkToBeRead <= 0 )
    {
      if ( m_chunkToBeRead < 0 ) m_pos = 2+m_chunkToBeRead;
      if ( ! http::get_line(m_csResponse, m_pos, line) )
      {
        // give the benefit of doubt, may be we aren't finised with Headers yet
        // we'll try to continue after next read
        //cerr << "S1: begin: " << m_csResponse.length() << ", " << m_pos << ", " << m_chunkToBeRead << endl;
        m_csResponse = m_csResponse.substr(m_pos);
        //cerr << "S1: end" << endl;
        m_pos = 0;
        break;
        //throw std::string("No benefit of doubt");
      }
      if ( ! sid::to_num(line, sid::num_base::hex, /*out*/ m_chunk.length) )
        throw std::string("Expecting chunk length. Encountered ") + line.substr(0, line.length() > 10? 10:line.length());
      m_chunkToBeRead = m_chunk.length;
      if ( pCallback && ! pCallback->is_valid(m_conn, m_chunk, _response, true) )
        m_forceStop = true; // Force stop

      if ( m_chunk.length == 0 )
      {
        if ( pCallback && ! pCallback->is_valid(m_conn, m_chunk, _response, false) )
          m_forceStop = true; // Force stop
        m_chunk.clear();
        m_endOfData = true; // END OF DATA
        break;
      }
      //cerr << "New chunklength: " << m_chunkToBeRead << endl;
      // cerr << "********* Length: " << m_csResponse.length() << ", m_pos: " << m_pos << ", m_chunkToBeRead: " << m_chunkToBeRead << " 0x" << line << endl;
    }
    //cerr << "m_csResponse: " << m_csResponse.length() << ", m_chunkToBeRead: " << m_chunkToBeRead << ", m_pos: " << m_pos << endl;
    if ( (m_csResponse.length()-m_pos) < (size_t) m_chunkToBeRead )
    {
      size_t copyLen = m_csResponse.length() - m_pos;
      //_response.content.append(m_csResponse, m_pos, copyLen);
      m_chunk.data.append(m_csResponse, m_pos, copyLen);
      m_chunkToBeRead -= copyLen;
      // cerr << "********* Body Length-1: " << _response.content.length() << ", Response-Length: " << m_csResponse.length() << ", m_pos: " << m_pos << endl;
      m_csResponse.clear(); m_pos = 0;
      break;
    }
    else
    {
      size_t balanceLen = m_csResponse.length() - m_pos - m_chunkToBeRead;
      //_response.content.append(m_csResponse, m_pos, m_chunkToBeRead);
      m_chunk.data.append(m_csResponse, m_pos, m_chunkToBeRead);
      if ( balanceLen <= 2 )
      {
        //cerr << "S2: begin: " << m_csResponse.length() << ", " << balanceLen << ", " << m_chunk.data.length() << endl;
        //cerr << m_csResponse << endl;
        m_csResponse.clear();
        /*
          if ( balanceLen == 0 )
          {
          m_csResponse.clear();
          //m_chunkToBeRead = 0;
          //m_pos = 0;
          }
          else
          {
          m_csResponse = m_csResponse.substr(m_csResponse.length()-balanceLen);
          }
        */
        m_chunkToBeRead = -balanceLen;
        m_pos = 2-balanceLen;
        //cerr << "S2: end: m_csResponse: " << m_csResponse.length() << ", m_chunkToBeRead: " << m_chunkToBeRead << ", m_pos: " << m_pos << endl;
        bContinueLoop = false;
      }
      else
      {
        m_pos += (m_chunkToBeRead + 2);
        m_chunkToBeRead = 0;//m_csResponse.length() - m_pos;
        //cerr << "S3:: " << m_csResponse.length() << ", " << m_pos << endl;
      }
      // cerr << "********* Body Length-2: " << _response.content.length() << ", Response-Length: " << m_csResponse.length() << ", m_pos: " << m_pos << endl;
    }
    
    if ( m_chunkToBeRead <= 0 )
    {
      _response.content.append(m_chunk.data);
      if ( pCallback && ! pCallback->is_valid(m_conn, m_chunk, _response, false) )
        m_forceStop = true; // Force stop
      m_chunk.clear();
    }
  }

  return;
}

void response_handler::parse(const char* buffer, int nread, const method& _requestMethod, /*in/out*/ response& _response) throw (std::string)
{
  //cerr.write(buffer, nread); return;

  try
  {
    //cerr << "New data received: " << nread << endl;
    //cerr.write(buffer, nread);
    m_csResponse.append(buffer, nread);

    // Parse status from the first line
    if ( ! m_endOfStatus && ! parse_status(_response) )
      return;

    // From now on it's only HTTP headers
    if ( ! m_endOfHeaders && ! parse_headers(_requestMethod, _response) )
      return;

    if ( m_encoding == http::transfer_encoding::chunked )
    {
      parse_data_chunked(_response);
    }
    else if ( m_contentLength > 0 )
    {
      parse_data_normal(_response);
    }
    else if ( m_endOfStatus && m_endOfHeaders )
    {
      size_t copyLen = m_csResponse.length() - m_pos;
      _response.content.append(m_csResponse, m_pos, copyLen);

      bool bFound;
      http::header_connection header_conn = _response.headers.connection(&bFound);
      if ( bFound && header_conn == http::header_connection::close )
      {
        m_csResponse.clear(); m_pos = 0;
      }
      else
        m_endOfData = true; // END OF DATA
    }

    if ( m_endOfData )
    {
      if ( pCallback ) pCallback->is_valid(m_conn, _response);
    }
  }
  catch (const std::string& csErr)
  {
    // rethrow the exception
    throw;
  }
  catch (...)
  {
    throw __func__ + std::string(": An unhandled exception occurred");
  }
}

//////////////////////////////////////////////////////////////////////////
/*static*/ IResponseCallback* IResponseCallback::get_singleton()
{
  static IResponseCallback callback;
  return &callback;
}

bool IResponseCallback::is_valid(const connection_ptr _conn, const status& _status, response& _response)
{
  return true;
}

bool IResponseCallback::is_valid(const connection_ptr _conn, const header& _header, response& _response)
{
  if ( strcasecmp(_header.key.c_str(), "Set-Cookie") == 0 )
  {
    http::cookie cookie;
    if ( cookie.set(_header.value) )
      http::cookies::set_session_cookie(_conn->server(), cookie);
  }

  return true;
}

bool IResponseCallback::is_valid(const connection_ptr _conn, const headers& _headers, response& _response)
{
  return true;
}

bool IResponseCallback::is_valid(const connection_ptr _conn, const data_chunk& _chunk, response& _response, bool _isStart)
{
/*
  cerr << "Data chunk " << (isStart? "start":"end") << ": length=" << chunk.length;
  if ( !isStart )
  {
    cerr << ", actual=" << chunk.data.length();
    cerr << endl << chunk.data;
  }
  cerr << endl;
  if ( !isStart && chunk.length != static_cast<int>(chunk.data.length()) )
  {
    cerr << "*** PANIC ***: Something wrong with getting the data" << endl;
    return false;
  }
*/
  return true;
}

bool IResponseCallback::is_valid(const connection_ptr _conn, response& _response)
{
  return true;
}

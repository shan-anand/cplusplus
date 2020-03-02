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

/**
 * @file connection.hpp
 * @brief Defines connection object.
 *
 * This is an abstract class that cannot be instantiated directly. You must use
 * the create() method to create an instance of httpconnection or httpsconnection.
 */
#ifndef _SID_HTTP_CONNECTION_H_
#define _SID_HTTP_CONNECTION_H_

#include "method.hpp"
#include "status.hpp"
#include <common/smart_ptr.hpp>
#include <string>
#include <unistd.h>

#define DEFAULT_PORT_HTTP  80
#define DEFAULT_PORT_HTTPS 443

#define DEFAULT_IO_TIMEOUT_SECS 30 // seconds

namespace sid {

namespace ssl
{
//! SSL Certificate type
enum class certificate_type : uint8_t { none = 0, client, server };

//! SSL Client certificate
struct client_certificate
{
  std::string chainFile;
  std::string privateKeyFile;
  int         privateKeyType;
  client_certificate() : chainFile(), privateKeyFile(), privateKeyType(0) {}
  void clear() { chainFile.clear(); privateKeyFile.clear(); privateKeyType = 0; }
  bool empty() const { return chainFile.empty() && privateKeyFile.empty(); }
};

  //! SSL Server certificate
struct server_certificate
{
  std::string caFile;
  std::string caPath;
  void clear() { caFile.clear(); caPath.clear(); }
  bool empty() const { return caFile.empty() && caPath.empty(); }
};

//! SSL Certificate
struct certificate
{
  //! Default constructor
  certificate() : type(certificate_type::none), client(), server() {}
  void clear() { type = certificate_type::none; client.clear(); server.clear(); }

  // Member variables
  certificate_type   type;
  client_certificate client;
  server_certificate server;
};

} // namespace ssl

namespace http {

class connection;
class request;
class response;

//! A smart pointer to the connection object.
using connection_ptr = sid::smart_ptr<connection>;

//! Available connection types
enum connection_type : uint8_t { http, https };

//! Available connection families
enum connection_family : uint8_t { none = 0, ip_v4 = 4, ip_v6 = 6 };

/**
 * @class connection_description
 * @brief Description of the connection
 */
struct connection_description
{
  struct ssl_info
  {
    bool         isAvailable;
    std::string  info;
    //! Default constructor
    ssl_info();
    //! Virtual destructor
    virtual ~ssl_info();
    //! Clear the objecgt
    void clear();
    //! Convert to string
    std::string to_str() const;
  };

  bool              isConnected;
  std::string       server;
  uint16_t          port;
  connection_type   type;
  connection_family family;
  bool              isBlocking;
  uint32_t          nonBlockingTimeout;
  ssl_info          ssl;

  //! Default constructor
  connection_description();

  //! Virtual destructor
  virtual ~connection_description();

  //! Clear the objecgt
  void clear();

  //! Convert to string
  std::string to_str() const;
};

/**
 * @class connection
 * @brief An abstract class for HTTP/HTTPS connection.
 */
class connection : public sid::smart_ref
{
public:
  //! Default constructor
  connection();

  //! Virtual destructor
  virtual ~connection();

  ////////////////////////////////////////////////////////////////////////////
  // Virtual functions
  // These functions will be overwritten in the derived class

  //! Get the connection type
  virtual const connection_type type() const = 0;

  /**
   * @fn bool open(const std::string& _server, const unsigned short& _port = 0);
   * @brief Open a connection to the given server at the specified port
   *
   * @param _server [in] Server name or IP address
   * @param _port [in] Port to be connected to at the server. If left unspecified or 0,
   *                  it takes the default port as per the connection type.
   *
   * @return true if the connection was successful, false otherwise.
   *         error() will contain the last error string in case of failure.
   * @seealso error()
   */
  virtual bool open(const std::string& _server, const unsigned short& _port = 0) = 0;

  /**
   * @fn bool open(int _sockfd);
   * @brief Open a connection object with the given sockfd.
   *        This function is called on the server side after the accept() function
   *        returns the client socket.
   *
   * @param _sockfd [in] Socket descriptor. (Usually the client socket).
   *
   * @return true if the connection object was successfully created, false otherwise.
   *         On success, sockfd is owned by the connection object and caller should not close it.
   *         On error, sockfd must be closed by the caller.
   *         error() will contain the last error string in case of failure.
   * @seealso error()
   */
  virtual bool open(int _sockfd) = 0;

  //! Checks if the connection is open or not.
  virtual bool is_open() const = 0;

  /**
   * @fn bool close();
   * @brief Closes the open connection.
   *
   * @return true if the connection was closed successfully, false otherwise.
   */
  virtual bool close() = 0;

  /**
   * @fn ssize_t write(const void* _buffer, size_t _count);
   * @brief Write data to the connection object.
   *
   * @param _buffer [in] pointer to the data to be written
   * @param _count [in] length of data to be written
   *
   * @return The number of bytes written is returned.
   *         On error, -1 is returned, and errno is set appropriately.
   */
  virtual ssize_t write(const void* _buffer, size_t _count) = 0;

  /**
   * @fn ssize_t read(void* _buffer, size_t _count);
   * @brief Read data from the connection object.
   *
   * @param _buffer [in/out] pointer to the data where data is to be read. It should be preallocated by the user.
   * @param _count [in] length of data to be read
   *
   * @return The number of bytes read is returned (zero indicates end of data).
   *         On error, -1 is returned, and errno is set appropriately.
   */
  virtual ssize_t read(void* _buffer, size_t _count) = 0;

  /**
   * @fn void set_blocking(bool _bEnable);
   * @brief Set or resets blocking mode
   *
   * @param _bEnable [in] if true, it enables blocking mode. If false, it enables non-blocking mode
   * @param _ioTimeoutSeconds [in] IO timeout in seconds. If 0, it defaults to DEFAULT_IO_TIMEOUT_SECS
   *
   */
  void set_blocking(bool _bEnable) { m_isBlocking = _bEnable; }
  void set_blocking(bool _bEnable, uint32_t _ioTimeoutSeconds) { set_blocking(_bEnable); set_timeout(_ioTimeoutSeconds); }

  //! returns true if the communication is non-blocking
  bool is_non_blocking() const { return !m_isBlocking; }
  bool is_blocking() const { return m_isBlocking; }


  //! A string representation of the connect used
  virtual connection_description description() const = 0;

  //! Accept - SSL-specific
  virtual void accept() {}
  //
  ////////////////////////////////////////////////////////////////////////////

  //! Get the non-blocking timeout value in seconds
  uint32_t get_timeout() const { return m_ioTimeout; }
  //! Set the non-blocking timeout value in seconds. Returns the old value.
  uint32_t set_timeout(uint32_t _seconds)
    {
      uint32_t old = m_ioTimeout;
      m_ioTimeout = (_seconds > 0)? _seconds : DEFAULT_IO_TIMEOUT_SECS;
      return old;
    }
    
  //! Returns the last error string
  const std::string& error() const { return m_error; }
  //! Used for setting the last error string
  const std::string& error(const std::string& err) { m_error = err; return m_error; }

  //! Get the server name or IP address as provided in the connection() method.
  const std::string& server() const { return m_server; }

  //! Connection family used
  const connection_family family() const { return m_family; }

  //! Get the actual server port in use.
  const unsigned int port() const { return m_port; }

  //! Checks if the error is a retryable error so that applications can retry the last method.
  const bool is_retryable() const { return m_retryable; }

  // Certificate used
  const ssl::certificate& certificate() const { return m_sslCert; }

  /**
   * @fn connection_ptr create(const connection_type& _type, const connection_family& _family);
   * @brief Creates a connection object based on the connection type specified. In case of error it throws a sid::exception.
   *
   * @param _type [in] Type of connection (HTTP or HTTPS)
   * @param _family [in] Connection family to use (ipv4 or ipv6 or any)
   *
   * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
   */
  static connection_ptr create(const connection_type& _type, const connection_family& _family = connection_family::none);

  /**
   * @fn connection_ptr create(const ssl::certificate& _sslCert, const connection_family& _family);
   * @brief Creates a connection object based on the connection type specified. In case of error it throws a sid::exception.
   *
   * @param _sslCert [in] SSL Certificate to be used for HTTPS connection
   * @param _family [in] Connection family to use (ipv4 or ipv6 or any)
   *
   * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
   */
  static connection_ptr create(const ssl::certificate& _sslCert, const connection_family& _family = connection_family::none);

private:
  /**
   * @fn connection_ptr create(const ssl::certificate& _sslCert, const connection_family& _family);
   * @brief Creates a connection object based on the connection type specified. In case of error it throws a sid::exception.
   *
   * @param _type [in] Type of connection (HTTP or HTTPS)
   * @param _sslCert [in] SSL Certificate to be used for HTTPS connection
   * @param _family [in] Connection family to use (ipv4 or ipv6 or any)
   *
   * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
   */
  static connection_ptr p_create(const connection_type& _type, const ssl::certificate& _sslCert, const connection_family& _family);

protected:
  std::string       m_server;        //! Server or IP address of the connection
  connection_family m_family;        //! Connection family in use
  std::string       m_error;         //! Last error
  unsigned short    m_port;          //! Port connected to at the server
  bool              m_retryable;     //! Retryable error or not? Set by implemented virtual function.
  bool              m_isBlocking;    //! Set blocking I/O. Internally it is non-blocking, but for blocking we just keep looping over infinitely.
  uint32_t          m_ioTimeout;     //! I/O timeout in seconds.
  ssl::certificate  m_sslCert;       //! SSL Certificate to be used for https
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_CONNECTION_H_

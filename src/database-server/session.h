/*! @brief This file have the interface for Session class.
    @file session.h
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 6/29/2019

    @copyright
    @attention <h1><center><strong>COPYRIGHT &copy; 2019 </strong>
    [<strong>denisacostaq</strong>][denisacostaq-URL].
    All rights reserved.</center></h1>
    @attention This file is part of [<strong>DAQs</strong>][DAQs-URL].

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    - 1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - 2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - 3. Neither the name of the University nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

    THIS PRODUCT IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS PRODUCT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    [denisacostaq-URL]: https://about.me/denisacostaq "Alvaro Denis Acosta"
    [DAQs-URL]: https://github.com/denisacostaq/DAQs "DAQs"
 */
#ifndef DAQS_DATABASESERVER_SESSION_H
#define DAQS_DATABASESERVER_SESSION_H

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <messages.pb.h>
#include <boost/asio.hpp>

#include "src/database-server/data-access/idataaccess.h"

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(boost::asio::ip::tcp::socket socket, IDataAccess* da);

  void start() { do_read(); }

 private:
  void read_header();

  void read_save_value_request(std::size_t b_size);

  void read_get_values_request(std::size_t b_size);

  void read_body(message::MessageType msg_type, std::size_t b_size);

  void do_read() { read_header(); }

  void send_msg(std::shared_ptr<uint8_t> f_buf, std::size_t f_size);

  std::shared_ptr<std::uint8_t> build_f_msg(
      std::unique_ptr<std::uint8_t[]>&& h_buf, std::size_t h_size,
      std::unique_ptr<std::uint8_t[]>&& b_buf, std::size_t b_size);

  std::unique_ptr<std::uint8_t[]> build_h_msg(std::size_t b_size,
                                              message::MessageType msg_type,
                                              std::size_t* out_fh_size);

  std::unique_ptr<std::uint8_t[]> build_b_response(
      const std::string& msg, message::ResponseStatus status,
      std::size_t* out_b_size);

  std::unique_ptr<std::uint8_t[]> build_b_response(
      const std::vector<IDataSource::VarValue>& values, std::size_t* out_b_size);

  void send_status_response(const std::string& msg,
                            message::ResponseStatus status);

  void send_values_response(const std::vector<IDataSource::VarValue>& values);

  boost::asio::ip::tcp::socket socket_;
  IDataAccess* da_;
};

#endif  // DAQS_DATABASESERVER_SESSION_H

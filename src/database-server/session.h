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

#include <boost/asio.hpp>

#include "messages.pb.h"

namespace ba = boost::asio;
using ba::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
 public:
  explicit Session(tcp::socket socket);

  void start() { do_read(); }

 private:
  void read_header();

  void read_save_value_request(std::size_t b_size) {
    auto self(shared_from_this());
    std::shared_ptr<std::uint8_t[]> body_buff{new std::uint8_t[b_size]};
    ba::async_read(socket_, ba::buffer(body_buff.get(), b_size),
                   [this, self, body_buff, b_size](boost::system::error_code ec,
                                                   size_t length) {
                     if (!ec && length == b_size) {
                       message::SaveValue sv{};
                       sv.ParseFromArray(body_buff.get(),
                                         static_cast<int>(b_size));
                       std::clog << "finally"
                                 << "\n";
                       std::clog << "name " << sv.variable() << "\n";
                       std::clog << "value " << sv.value() << "\n\n";
                       send_status_response("Ok", message::ResponseStatus::OK);
                     } else {
                       std::cerr << "reading body_buf" << ec.message() << "\n";
                     }
                   });
  }

  void read_body(message::MessageType msg_type, std::size_t b_size) {
    switch (msg_type) {
      case message::MessageType::REQUEST_SAVE_VALUE:
        read_save_value_request(b_size);
        break;
      default:
        std::cerr << "unknow request " << msg_type << "\n";
        send_status_response("unknow error", message::ResponseStatus::FAILED);
        break;
    }
  }

  void do_read() { read_header(); }

  void send_msg(std::shared_ptr<std::uint8_t[]> f_buf, std::size_t f_size) {
    auto self(shared_from_this());
    ba::async_write(socket_, ba::buffer(f_buf.get(), f_size),
                    [this, self, f_buf, f_size](boost::system::error_code ec,
                                                std::size_t length) {
                      if (!ec && f_size == length) {
                        do_read();
                      } else {
                        std::cerr << "writing full buffer "
                                  << "\n";
                        send_status_response("writing response",
                                             message::ResponseStatus::FAILED);
                      }
                    });
  }

  std::shared_ptr<std::uint8_t[]> build_f_msg(
      std::unique_ptr<std::uint8_t[]>&& h_buf, std::size_t h_size,
      std::unique_ptr<std::uint8_t[]>&& b_buf, std::size_t b_size);

  std::unique_ptr<std::uint8_t[]> build_h_msg(std::size_t b_size,
                                              std::size_t* out_fh_size);

  std::unique_ptr<std::uint8_t[]> build_b_response(
      const std::string& msg, message::ResponseStatus status,
      std::size_t* out_b_size);

  void send_status_response(const std::string& msg,
                            message::ResponseStatus status);

  tcp::socket socket_;
};

#endif  // DAQS_DATABASESERVER_SESSION_H

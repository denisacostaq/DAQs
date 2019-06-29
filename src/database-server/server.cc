/*! \brief This file have the implementation for server.
    \file server.c
    \author Alvaro Denis <denisacostaq@gmail.com>
    \date 6/28/2019

    \copyright
    \attention <h1><center><strong>COPYRIGHT &copy; 2019 </strong>
    [<strong>denisacostaq</strong>][denisacostaq-URL].
    All rights reserved.</center></h1>
    \attention This file is part of
   [<strong>APIRestGenerator</strong>][APIRestGenerator-URL].

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
    [APIRestGenerator-URL]: http://github.com/denisacostaq/apirestgenerator
   "APIRestGenerator based on golang revel and gorm"
 */

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <boost/asio.hpp>

#include "messages.pb.h"

namespace ba = boost::asio;
using ba::ip::tcp;

class session : public std::enable_shared_from_this<session> {
 public:
  explicit session(tcp::socket socket) : socket_(std::move(socket)) {}

  void start() { do_read(); }

 private:
  void do_read() {
    auto self(shared_from_this());
    decltype(message::MetaHeader{}.ByteSizeLong()) mh_size{0};
    {
      auto m{message::MetaHeader{}};
      m.set_headersize(1);
      mh_size = m.ByteSizeLong();
    }
    std::clog << "mh_size " << mh_size << "\n";
    std::shared_ptr<std::uint8_t[]> mh_buff{new std::uint8_t[mh_size]};
    ba::async_read(
        socket_, ba::buffer(mh_buff.get(), mh_size),
        [this, self, mh_size, mh_buff](boost::system::error_code ec,
                                       std::size_t length) {
          if (!ec && length == mh_size) {
            auto mh{message::MetaHeader{}};
            mh.ParseFromArray(mh_buff.get(), static_cast<int>(mh_size));
            std::clog << "mh.headersize() " << mh.headersize() << std::endl;
            std::shared_ptr<std::uint8_t[]> h_buff{
                new std::uint8_t[mh.headersize()]};
            ba::async_read(
                socket_, ba::buffer(h_buff.get(), mh.headersize()),
                [this, self, h_size = mh.headersize(), h_buff](
                    boost::system::error_code ec, std::size_t length) {
                  if (!ec && length == h_size) {
                    message::Header h{};
                    h.ParseFromArray(h_buff.get(), static_cast<int>(h_size));
                    std::clog << "h.bodysize() " << h.bodysize() << std::endl;
                    std::shared_ptr<std::uint8_t[]> body_buff{
                        new std::uint8_t[h.bodysize()]};
                    ba::async_read(
                        socket_, ba::buffer(body_buff.get(), h.bodysize()),
                        [this, self, body_buff, body_size = h.bodysize()](
                            boost::system::error_code ec, size_t length) {
                          if (!ec && length == body_size) {
                            message::SaveValue sv{};
                            sv.ParseFromArray(body_buff.get(),
                                              static_cast<int>(body_size));
                            std::clog << "finally"
                                      << "\n";
                            std::clog << "name " << sv.variable() << "\n";
                            std::clog << "value " << sv.value() << "\n\n";
                            do_write(sv.value());
                          } else {
                            std::cerr << "reading body_buf" << ec.message()
                                      << "\n";
                          }
                        });
                  } else {
                    std::cerr << "reading h_buff" << ec.message() << "\n";
                  }
                });
          } else {
            std::cerr << "reading mh_buff " << ec.message() << "\n";
          }
        });
  }

  void do_write(double val) {
    auto self(shared_from_this());
    message::Failure body;
    if (val > 50.) {
      body.set_status(message::ResponseStatus::OK);
      body.set_msg("all ok");
    } else {
      body.set_status(message::ResponseStatus::INVALID_ARGUMENT);
      body.set_msg("fallo carijo");
    }
    message::Header h{};
    h.set_msg_type(message::RESPONSE_FAILURE);
    h.set_bodysize(body.ByteSizeLong());
    std::unique_ptr<std::uint8_t[]> h_data{new std::uint8_t[h.ByteSizeLong()]};
    h.SerializeToArray(h_data.get(), h.ByteSize());
    std::unique_ptr<std::uint8_t[]> body_data{
        new std::uint8_t[body.ByteSizeLong()]};
    body.SerializeToArray(body_data.get(), body.ByteSize());
    message::MetaHeader mh{};
    mh.set_headersize(h.ByteSizeLong());
    std::unique_ptr<std::uint8_t[]> mh_data{
        new std::uint8_t[mh.ByteSizeLong()]};
    mh.SerializeToArray(mh_data.get(), mh.ByteSize());

    std::shared_ptr<std::uint8_t[]> full_buffer{
        new std::uint8_t[mh.ByteSizeLong() + h.ByteSizeLong() +
                         body.ByteSizeLong()]};
    std::memcpy(full_buffer.get(), mh_data.get(), mh.ByteSizeLong());
    std::memcpy(&full_buffer.get()[mh.ByteSizeLong()], h_data.get(),
                h.ByteSizeLong());
    std::memcpy(&full_buffer.get()[mh.ByteSizeLong() + h.ByteSizeLong()],
                body_data.get(), body.ByteSizeLong());
    auto full_buffer_size{mh.ByteSizeLong() + h.ByteSizeLong() +
                          body.ByteSizeLong()};
    ba::async_write(socket_, ba::buffer(full_buffer.get(), full_buffer_size),
                    [this, self, full_buffer, full_buffer_size](
                        boost::system::error_code ec, std::size_t length) {
                      if (!ec && full_buffer_size == length) {
                        do_read();
                      } else {
                        std::cerr << "writing full buffer " << "\n";
                      }
                    });
  }

  tcp::socket socket_;
};

class server {
 public:
  server(ba::io_context& io_context, std::uint16_t port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

 private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          } else {
            std::cerr << "acepting " << ec.message() << "\n";
          }
          do_accept();
        });
  }

  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }
    ba::io_context io_context;
    server s(io_context, std::atoi(argv[1]));
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}

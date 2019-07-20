/*! @brief This file have the implementation for Session class.
    @file session.cc
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
#include "src/database-server/session.h"

namespace ba = boost::asio;
using ba::ip::tcp;

Session::Session(tcp::socket socket, IDataAccess *da)
    : socket_(std::move(socket)), da_{da} {}

void Session::read_header() {
  auto mh_size_func = []() {
    auto mh{message::MetaHeader{}};
    mh.set_headersize(1);
    return mh.ByteSizeLong();
  };
  // FIXME(denisacostaq@gmail.com): verify critical section.
  static const auto mh_size{mh_size_func()};
  auto mh_buff{std::shared_ptr<std::uint8_t>(
      new std::uint8_t[mh_size], std::default_delete<std::uint8_t[]>())};
  auto self(shared_from_this());
  ba::async_read(
      socket_, ba::buffer(mh_buff.get(), mh_size),
      [this, self, mh_buff](boost::system::error_code ec, std::size_t length) {
        if (!ec && length == mh_size) {
          message::MetaHeader mh{};
          mh.ParseFromArray(mh_buff.get(), static_cast<int>(mh_size));
          std::clog << "mh.headersize() " << mh.headersize() << std::endl;
          auto h_buff{std::shared_ptr<std::uint8_t>{
              new std::uint8_t[mh.headersize()],
              std::default_delete<std::uint8_t[]>()}};
          ba::async_read(
              socket_, ba::buffer(h_buff.get(), mh.headersize()),
              [this, self, h_size = mh.headersize(), h_buff](
                  boost::system::error_code ec, std::size_t length) {
                if (!ec && length == h_size) {
                  message::Header h{};
                  h.ParseFromArray(h_buff.get(), static_cast<int>(h_size));
                  std::clog << "h.bodysize() " << h.bodysize() << std::endl;
                  read_body(h.msg_type(), h.bodysize());
                } else {
                  std::cerr << "reading h_buff" << ec.message() << "\n";
                  send_status_response("reading h_buff",
                                       message::ResponseStatus::FAILED);
                }
              });
        } else {
          std::cerr << "reading mh_buff " << ec.message() << "\n";
          send_status_response("reading mh_buff",
                               message::ResponseStatus::FAILED);
        }
      });
}

void Session::send_status_response(const std::string &msg,
                                   message::ResponseStatus status) {
  std::size_t b_size{};
  auto b_buf{build_b_response(msg, status, &b_size)};
  std::size_t fh_size{};
  auto fh_buf{
      build_h_msg(b_size, message::MessageType::RESPONSE_FAILURE, &fh_size)};
  auto f_buf{build_f_msg(std::move(fh_buf), fh_size, std::move(b_buf), b_size)};
  send_msg(f_buf, fh_size + b_size);
}

void Session::send_values_response(std::vector<VarValue> &&values) {
  std::size_t b_size{};
  auto b_buf{build_b_response(std::move(values), &b_size)};
  std::size_t fh_size{};
  auto fh_buf{
      build_h_msg(b_size, message::MessageType::RESPONSE_VALUES, &fh_size)};
  auto f_buf{build_f_msg(std::move(fh_buf), fh_size, std::move(b_buf), b_size)};
  send_msg(f_buf, fh_size + b_size);
}

std::unique_ptr<std::uint8_t[]> Session::build_b_response(
    const std::string &msg, message::ResponseStatus status,
    std::size_t *out_b_size) {
  message::Failure b_msg{};
  b_msg.set_status(status);
  b_msg.set_msg(msg);
  std::unique_ptr<std::uint8_t[]> b_buf{new std::uint8_t[b_msg.ByteSizeLong()]};
  b_msg.SerializeToArray(b_buf.get(), b_msg.ByteSize());
  *out_b_size = b_msg.ByteSizeLong();
  return b_buf;
}

std::unique_ptr<std::uint8_t[]> Session::build_b_response(
    std::vector<VarValue> &&vals, std::size_t *out_b_size) {
  message::ValuesResponse b_msg{};
  std::for_each(vals.cbegin(), vals.cend(), [&b_msg](const VarValue &val) {
    auto v{b_msg.mutable_values()->Add()};
    v->set_name(val.name());
    v->set_value(val.val());
    v->set_timestamp(val.timestamp());
  });
  std::unique_ptr<std::uint8_t[]> b_buf{new std::uint8_t[b_msg.ByteSizeLong()]};
  b_msg.SerializeToArray(b_buf.get(), b_msg.ByteSize());
  *out_b_size = b_msg.ByteSizeLong();
  return b_buf;
}

std::unique_ptr<std::uint8_t[]> Session::build_h_msg(
    std::size_t b_size, message::MessageType msg_type,
    std::size_t *out_fh_size) {
  message::Header h_msg{};
  h_msg.set_msg_type(msg_type);
  h_msg.set_bodysize(b_size);
  auto h_buf{std::make_unique<std::uint8_t[]>(h_msg.ByteSizeLong())};
  h_msg.SerializeToArray(h_buf.get(), h_msg.ByteSize());
  message::MetaHeader mh_msg{};
  mh_msg.set_headersize(h_msg.ByteSizeLong());
  auto mh_buf{std::make_unique<std::uint8_t[]>(mh_msg.ByteSizeLong())};
  mh_msg.SerializeToArray(mh_buf.get(), mh_msg.ByteSize());
  const auto fh_size{mh_msg.ByteSizeLong() + h_msg.ByteSizeLong()};
  auto f_buf_addr{new std::uint8_t[fh_size]};
  auto fh_buf{std::unique_ptr<std::uint8_t[]>(f_buf_addr)};
  std::memcpy(f_buf_addr, mh_buf.get(), mh_msg.ByteSizeLong());
  std::memcpy(&f_buf_addr[mh_msg.ByteSizeLong()], h_buf.get(),
              h_msg.ByteSizeLong());
  *out_fh_size = fh_size;
  return fh_buf;
}

std::shared_ptr<std::uint8_t> Session::build_f_msg(
    std::unique_ptr<std::uint8_t[]> &&h_buf, std::size_t h_size,
    std::unique_ptr<std::uint8_t[]> &&b_buf, std::size_t b_size) {
  auto f_buffer_size{h_size + b_size};
  auto f_buf_addr{new std::uint8_t[f_buffer_size]};
  auto f_buf{std::shared_ptr<std::uint8_t>{
      f_buf_addr, std::default_delete<std::uint8_t[]>()}};
  std::memcpy(f_buf_addr, h_buf.get(), h_size);
  std::memcpy(&f_buf_addr[h_size], b_buf.get(), b_size);
  return f_buf;
}

void Session::send_msg(std::shared_ptr<std::uint8_t> f_buf,
                       std::size_t f_size) {
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

void Session::read_body(message::MessageType msg_type, std::size_t b_size) {
  switch (msg_type) {
    case message::MessageType::REQUEST_SAVE_VALUE:
      read_save_value_request(b_size);
      break;
    case message::MessageType::REQUEST_GET_VALUES:
      read_get_values_request(b_size);
      break;
    default:
      std::cerr << "unknow request " << msg_type << "\n";
      send_status_response("unknow error", message::ResponseStatus::FAILED);
      break;
  }
}

void Session::read_save_value_request(std::size_t b_size) {
  auto self(shared_from_this());
  auto body_buff{std::shared_ptr<std::uint8_t>(
      new std::uint8_t[b_size], std::default_delete<std::uint8_t[]>())};
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
                     // FIXME(denisacostaq@gmail.com)" color
                     Variable var{sv.variable(), "color"};
                     auto vv{VarValue{var, sv.value(), 0}};
                     auto err{da_->add_variable_value(std::move(vv))};
                     if (IDataAccess::Err::Ok == err) {
                       send_status_response("Ok", message::ResponseStatus::OK);
                     } else {
                       std::cerr << "database error\n";
                       send_status_response("Failed to save value",
                                            message::ResponseStatus::FAILED);
                     }
                   } else {
                     std::cerr << "reading body_buf" << ec.message() << "\n";
                   }
                 });
}

void Session::read_get_values_request(std::size_t b_size) {
  auto self(shared_from_this());
  auto body_buff{std::shared_ptr<std::uint8_t>{
      new std::uint8_t[b_size], std::default_delete<std::uint8_t[]>()}};
  ba::async_read(socket_, ba::buffer(body_buff.get(), b_size),
                 [this, self, body_buff, b_size](boost::system::error_code ec,
                                                 size_t length) {
                   if (!ec && length == b_size) {
                     message::GetValues gv{};
                     gv.ParseFromArray(body_buff.get(),
                                       static_cast<int>(b_size));
                     auto res{da_->fetch_variable_values(gv.variable())};
                     if (std::get<1>(res) == IDataAccess::Err::Ok) {
                       send_values_response(std::move(std::get<0>(res)));
                     } else {
                       std::cerr << "database error\n";
                       send_status_response("Failed to get values",
                                            message::ResponseStatus::FAILED);
                     }
                   } else {
                     std::cerr << "reading body_buf" << ec.message() << "\n";
                   }
                 });
}

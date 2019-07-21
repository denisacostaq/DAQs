/*! \brief This file have the implementation for Client class.
    \file client.cc
    \author Alvaro Denis <denisacostaq@gmail.com>
    \date 6/28/2019

    \copyright
    \attention <h1><center><strong>COPYRIGHT &copy; 2019 </strong>
    [<strong>denisacostaq</strong>][denisacostaq-URL].
    All rights reserved.</center></h1>
    \attention This file is part of [<strong>DAQs</strong>][DAQs-URL].

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
    [DAQs-URL]: http://github.com/denisacostaq/apirestgenerator
   "APIRestGenerator based on golang revel and gorm"
 */
#include "client.h"

#include <sstream>
#include <thread>

#include <QtCore/QDebug>

#include "messages.pb.h"

Client::Client(const QString& host, std::uint16_t port, QObject* parent)
    : QObject(parent), socket_{parent}, host_{host}, port_{port} {
  QObject::connect(&socket_, &QTcpSocket::bytesWritten,
                   [](std::uint64_t l) { qDebug() << "writen" << l; });

  QObject::connect(&socket_, &QTcpSocket::connected,
                   [=]() { qDebug() << "connected"; });
  QObject::connect(
      &socket_,
      QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
      [this](QAbstractSocket::SocketError err) {
        qDebug() << err << socket_.errorString();
      });
  QObject::connect(&socket_, &QIODevice::readyRead, this, &Client::onReadyRead);
  QObject::connect(&socket_, &QTcpSocket::connected, this, &Client::connected);
  //  QObject::connect(&socket_, &QTcpSocket::disconnected,
  //                   [this]() { this->connected_ = false; });
}

void Client::onReadyRead() {
  QObject::disconnect(&socket_, &QIODevice::readyRead, this,
                      &Client::onReadyRead);
  std::unique_ptr<char, std::function<void(char*)>> readyReadDeferReconnect{
      new char(), [this](char* i) {
        delete i;
        QObject::connect(&socket_, &QIODevice::readyRead, this,
                         &Client::onReadyRead);
      }};
  size_t mh_size{0};
  {
    message::MetaHeader mh{};
    mh.set_headersize(1);
    mh_size = mh.ByteSizeLong();
  }
  message::MetaHeader mh{};
  std::unique_ptr<char[]> mh_data{new char[mh_size]};
  auto readed0{socket_.read(mh_data.get(), static_cast<qint64>(mh_size))};
  if (static_cast<std::size_t>(readed0) != mh_size) {
    qDebug() << "meta header error, trying to read" << mh_size << "but readed"
             << readed0;
    return;
  }
  mh.ParseFromArray(mh_data.get(), static_cast<int>(mh_size));
  message::Header h{};
  std::unique_ptr<char[]> h_data{new char[mh.headersize()]};
  auto readed1{
      socket_.read(h_data.get(), static_cast<qint64>(mh.headersize()))};
  if (static_cast<std::size_t>(readed1) != mh.headersize()) {
    qDebug() << "header error, trying to read" << mh.headersize()
             << "but readed" << readed1;
    return;
  }
  h.ParseFromArray(h_data.get(), static_cast<int>(mh.headersize()));
  if (h.bodysize() > std::numeric_limits<int>::max()) {
    qDebug() << "h.bodysize() do not fit in the ParseFromArray arguments"
             << h.bodysize();
    return;
  }
  if (h.msg_type() == message::RESPONSE_FAILURE) {
    message::Failure resp{};
    std::unique_ptr<char[]> data_r{new char[h.bodysize()]};
    auto readed2{socket_.read(data_r.get(), static_cast<qint64>(h.bodysize()))};
    if (readed2 != static_cast<qint64>(h.bodysize())) {
      qDebug() << "body error, trying to read" << h.bodysize() << "but readed"
               << readed2;
      return;
    }
    resp.ParseFromArray(data_r.get(), static_cast<int>(readed2));
    emit responseReceived(resp.status(), resp.msg().c_str());
  } else if (h.msg_type() == message::RESPONSE_VALUES) {
    message::ValuesResponse valsResp{};
    std::unique_ptr<char[]> data_r{new char[h.bodysize()]};
    qint64 readed{0};
    do {
      qint64 r{socket_.read(&data_r.get()[readed],
                            static_cast<qint64>(h.bodysize()) - readed)};
      if (r == 0 && !socket_.waitForReadyRead()) {
        // FIXME(denisacostaq@gmail.com): uint64 to int64
        qDebug() << "body error, trying to read" << h.bodysize() - readed + r
                 << "but there is no more data";
        return;
      } else if (r == -1) {
        // FIXME(denisacostaq@gmail.com): uint64 to int64
        qDebug() << "body error, trying to read" << h.bodysize() - readed
                 << "but readed" << readed;
        return;
      }
      readed += r;
      // FIXME(denisacostaq@gmail.com): uint64 to int64
    } while (readed != static_cast<qint64>(h.bodysize()));
    valsResp.ParseFromArray(data_r.get(), static_cast<int>(readed));
    ::google::protobuf::RepeatedPtrField<::message::VarValue> const* const
        p_vals{valsResp.mutable_values()};
    std::vector<VarValue> vals{};
    vals.reserve(static_cast<decltype(vals)::size_type>(p_vals->size()));
    std::for_each(p_vals->begin(), p_vals->end(),
                  [&vals](const ::message::VarValue& val) {
                    // FIXME(denisacostaq@gmail.com)" color
                    Variable variable{val.name(), "color"};
                    VarValue v{variable, val.value(), val.timestamp()};
                    vals.push_back(std::move(v));
                  });
    emit valuesReceived(vals);
  }
}

void Client::connect() { socket_.connectToHost(host_, port_); }

void Client::send_var_val(const QString& var_name, double value) {
  message::SaveValue sv{};
  sv.set_variable(var_name.toStdString());
  sv.set_value(value);
  message::Header hdr{};
  hdr.set_msg_type(message::REQUEST_SAVE_VALUE);
  hdr.set_bodysize(sv.ByteSizeLong());
  std::ostringstream out{};
  message::MetaHeader mh{};
  mh.set_headersize(hdr.ByteSizeLong());
  mh.SerializeToOstream(&out);
  hdr.SerializeToOstream(&out);
  sv.SerializeToOstream(&out);
  QByteArray bytes{out.str().c_str(),
                   static_cast<int>(mh.ByteSizeLong() + hdr.ByteSizeLong() +
                                    sv.ByteSizeLong())};
  socket_.write(bytes);
}

void Client::request_var_values(
    const QString& var_name, const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) {
  message::GetValues gv{};
  gv.set_variable(var_name.toStdString());
  gv.set_start(start.time_since_epoch().count());
  gv.set_end(end.time_since_epoch().count());
  message::Header hdr{};
  hdr.set_msg_type(message::REQUEST_GET_VALUES);
  hdr.set_bodysize(gv.ByteSizeLong());
  std::ostringstream out{};
  message::MetaHeader mh{};
  mh.set_headersize(hdr.ByteSizeLong());
  mh.SerializeToOstream(&out);
  hdr.SerializeToOstream(&out);
  gv.SerializeToOstream(&out);
  QByteArray bytes{out.str().c_str(),
                   static_cast<int>(mh.ByteSizeLong() + hdr.ByteSizeLong() +
                                    gv.ByteSizeLong())};
  socket_.write(bytes);
}

bool Client::is_connected() const {
  return socket_.openMode() != QIODevice::NotOpen;
}

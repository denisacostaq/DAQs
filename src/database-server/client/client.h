/*! \brief This file have the interface for Client class.
    \file client.h
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
#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

#include <messages.pb.h>

#include "src/database-server/data-source/idatamodel.h"

class Client : public QObject {
  Q_OBJECT
 public:
  Client(const QString &host, std::uint16_t port, QObject *parent = nullptr);

  void connect();

  void send_var_val(const QString &var_name, double value);
  void request_var_values(const QString &var_name,
                          const std::chrono::system_clock::time_point &start,
                          const std::chrono::system_clock::time_point &end);

  ~Client() { socket_.close(); }

 signals:
  // FIXME(denisacostaq@gmail.com): expensive copy
  void valuesReceived(const std::vector<IDataModel::VarValue> &vals);
  void responseReceived(message::ResponseStatus status, QString response);
  void connected();

 private slots:
  void onReadyRead();

 private:
  bool is_connected() const;
  QTcpSocket socket_;
  const QString host_;
  std::uint16_t port_;
};

#endif  // CLIENT_H

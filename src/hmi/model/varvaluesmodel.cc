/*! @brief This file have the implementation for VarValuesModel class.
    @file varvaluesmodel.cc
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
#include "src/hmi/model/varvaluesmodel.h"

#include <algorithm>
#include <random>

#include <QtCore/QTimer>

#include "src/database-server/client/client.h"

VarValuesModel::VarValuesModel(Client *cl, QObject *parent)
    : QObject{parent},
      m_cl{cl},
      m_now{std::chrono::system_clock::now()},
      m_vals{},
      m_qml_vals{QQmlListProperty<VarValueModel>(
          this, &m_vals, &add_val, &vals_size, &val_at, &clear_vals)} {
  QObject::connect(m_cl, &Client::connected,
                   []() { qDebug() << "connected recived"; });
  QObject::connect(
      m_cl, &Client::valuesReceived, [this](const std::vector<VarValue> &vals) {
        m_vals.clear();
        m_vals.reserve(vals.size());
        for (const auto &val : vals) {
          m_vals.emplace_back(VarValueModel{val.val(), val.timestamp()});
        }
        if (!m_vals.empty()) {
          emit valsChanged();
        }
      });
  QTimer *m_wTimer{new QTimer{this}};
  m_wTimer->setInterval(6000);
  m_wTimer->setSingleShot(true);
  QObject::connect(m_wTimer, &QTimer::timeout, this,
                   [this]() { m_cl->request_var_values("temp"); });
  m_wTimer->start();
}

Q_INVOKABLE void VarValuesModel::getValues(QString var, qint64 s, qint64 e) {
  std::chrono::time_point<std::chrono::system_clock> satart{
      std::chrono::duration<qint64, std::milli>{s}};
  std::chrono::time_point<std::chrono::system_clock> end{
      std::chrono::duration<qint64, std::milli>{e}};
  m_cl->request_var_values(var, satart, end);
}

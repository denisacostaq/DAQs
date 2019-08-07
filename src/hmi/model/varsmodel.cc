/*! @brief This file have the implementation for VarsModel class.
    @file varsmodel.cc
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 7/27/2019

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
#include "src/hmi/model/varsmodel.h"

#include "src/database-server/client/client.h"

VarsModel::VarsModel(Client* cl, QObject* parent)
    : QObject{parent},
      m_vars{},
      m_qml_vars{QQmlListProperty<VarModel>(this, this, &add_var, &vars_size,
                                            &var_at, &clear_vars)},
      m_cl{cl} {
  for (int i = 0; i < 1000; ++i) {
    QString color{};
    switch (i % 5) {
      case 0:
        color = "black";
        break;
      case 1:
        color = "blue";
        break;
      case 2:
        color = "yellow";
        break;
      case 3:
        color = "green";
        break;
      case 4:
        color = "cyan";
        break;
    }
    m_vars.push_back(new VarModel{QString{"aa %1"}.arg(i), color, this});
  }
  emit varsChanged();
  QObject::connect(m_cl, &Client::variablesReceived,
                   [this](const std::vector<Variable>& vars) {
                     clear();
                     std::transform(
                         vars.begin(), vars.end(), std::back_inserter(m_vars),
                         [this](const Variable& var) {
                           QColor color{};
                           color.setNamedColor(var.color().c_str());
                           return new VarModel{var.name().c_str(), color, this};
                         });
                     emit varsChanged();
                   });
  QObject::connect(
      m_cl, &Client::responseReceived,
      [this](message::MessageType* prev_msg, message::ResponseStatus status,
             const QString& msg) {
        if (status == message::ResponseStatus::OK) {
          if (prev_msg && *prev_msg == message::MessageType::REQUEST_ADD_VAR) {
            qDebug() << "if (prev_msg && *prev_msg == "
                        "message::MessageType::REQUEST_ADD_VAR) {";
            m_cl->request_vars();
          }
        } else {
          qDebug() << message::ResponseStatus_Name(status).c_str() << msg;
        }
      });
}

void VarsModel::add_var(decltype(VarsModel::m_vars)::value_type var) {
  Variable variable{var->name().toStdString(),
                    var->color().name().toStdString()};
  m_cl->send_var(variable);
}

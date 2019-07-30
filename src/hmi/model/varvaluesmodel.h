/*! @brief This file have the interface for VarValuesModel class.
    @file varvaluesmodel.h
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
#ifndef HMI_MODEL_VARVALUESMODEL
#define HMI_MODEL_VARVALUESMODEL

#include <chrono>
#include <thread>

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtQml/QQmlListProperty>

#include "src/database-server/client/client.h"
#include "src/hmi/model/varvaluemodel.h"

class VarValuesModel : public QObject {
  Q_OBJECT
 public:
  Q_PROPERTY(QQmlListProperty<VarValueModel> vals READ getVals)
  VarValuesModel(QObject *parent = nullptr);

  QQmlListProperty<VarValueModel> getVals() { return m_qml_vals; }
  Q_INVOKABLE void getValues(QString var, qint64 s, qint64 e);

 signals:
  void valsChanged();

 private:
  Client *m_cl;  // FIXME(denisacostaq@gmail.com): RAII even delete.
  std::chrono::system_clock::time_point m_now;
  std::vector<VarValueModel> m_vals;
  QQmlListProperty<decltype(m_vals)::value_type> m_qml_vals;
  static void add_val(decltype(m_qml_vals) *, decltype(m_vals)::value_type *) {
    qDebug() << "unsupported operation, so ignored";
  }
  static decltype(m_vals)::value_type *val_at(decltype(m_qml_vals) *property,
                                              int index) {
    return &(reinterpret_cast<decltype(m_vals) *>(property->data)->at(index));
  }
  static int vals_size(decltype(m_qml_vals) *property) {
    return reinterpret_cast<decltype(m_vals) *>(property->data)->size();
  }
  static void clear_vals(decltype(m_qml_vals) *) {
    qDebug() << "unsupported operation, so ignored";
  }
};

#endif  // HMI_MODEL_VARVALUESMODEL

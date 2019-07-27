/*! @brief This file have the interface for HistoricData class.
    @file historicdata.h
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
#ifndef HISTORICDATA_H
#define HISTORICDATA_H

#include <chrono>
#include <thread>

#include <QQmlListProperty>
#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QVector>

#include "src/database-server/client/client.h"
#include "src/hmi/model/varvaluemodel.h"

class HistoricData : public QObject {
  Q_OBJECT
 public:
  Q_PROPERTY(QQmlListProperty<VarValueModel> vals READ getVals)
  HistoricData(QObject *parent = nullptr);

  QQmlListProperty<VarValueModel> getVals() {
    return m_qml_vals;
  }

 signals:
  void valsChanged();

 private:
  Client *m_cl;  // FIXME(denisacostaq@gmail.com): RAII even delete.
  std::chrono::system_clock::time_point m_now;
  QVector<VarValueModel *> m_vals;
  QQmlListProperty<VarValueModel> m_qml_vals;
  static void appendList(QQmlListProperty<VarValueModel> *property,
                         VarValueModel *note) {
    reinterpret_cast<decltype(m_vals) *>(property->data)->push_back(note);
  }
  static VarValueModel *cardAt(QQmlListProperty<VarValueModel> *property,
                               int index) {
    return reinterpret_cast<decltype(m_vals) *>(property->data)->at(index);
  }
  static int listSize(QQmlListProperty<VarValueModel> *property) {
    return reinterpret_cast<decltype(m_vals)*>(property->data)->size();
  }
  static void clearListPtr(QQmlListProperty<VarValueModel> *property) {
    reinterpret_cast<decltype(m_vals)*>(property->data)->clear();
  }
};

#endif  // HISTORICDATA_H

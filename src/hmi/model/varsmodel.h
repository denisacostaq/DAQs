/*! @brief This file have the interface for VarsModel class.
    @file varsmodel.h
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
#ifndef HMI_MODEL_VARSMODEL_H
#define HMI_MODEL_VARSMODEL_H

#include <vector>

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>

#include "src/hmi/model/varmodel.h"

class VarsModel : public QObject {
  Q_OBJECT
 public:
  Q_PROPERTY(QQmlListProperty<VarModel> vars READ getVars NOTIFY varsChanged)
 public:
  explicit VarsModel(QObject *parent = nullptr);
  QQmlListProperty<VarModel> getVars() { return m_qml_vars; }

 signals:
  void varsChanged();
 public slots:
 private:
  std::vector<VarModel> m_vars;
  QQmlListProperty<decltype(m_vars)::value_type> m_qml_vars;
  static void add_var(decltype(m_qml_vars) *property,
                      decltype(m_vars)::value_type *var) {
    VarModel v{var->name(), "var->color()"};
    reinterpret_cast<decltype(m_vars) *>(property->data)
        ->push_back(std::move(v));
  }
  static decltype(m_vars)::value_type *var_at(decltype(m_qml_vars) *property,
                                              int index) {
    return &(reinterpret_cast<decltype(m_vars) *>(property->data)->at(index));
  }
  static int vars_size(decltype(m_qml_vars) *property) {
    return reinterpret_cast<decltype(m_vars) *>(property->data)->size();
  }
  static void clear_vars(decltype(m_qml_vars) *) {
    //    qDebug() << "unsupported operation, so ignored";
  }
};

#endif  // HMI_MODEL_VARSMODEL_H

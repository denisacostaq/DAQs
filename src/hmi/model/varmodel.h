/*! @brief This file have the interface for VarModel class.
    @file varmodel.h
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
#ifndef HMI_MODEL_VARMODEL_H
#define HMI_MODEL_VARMODEL_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QColor>

class VarModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
 public:
  explicit VarModel(const QString &name, const QColor &color,
                    QObject *parent = nullptr);
  explicit VarModel(QObject *parent = nullptr);
  VarModel(VarModel &&other)
      : QObject{other.parent()}, m_name{other.m_name}, m_color{other.m_color} {}

  inline const QString &name() const noexcept { return m_name; }
  inline void setName(const QString &name) {
    if (name != m_name) {
      m_name = name;
      emit nameChanged(m_name);
    }
  }
  inline const QColor &color() const noexcept { return m_color; }
  inline void setColor(const QColor &color) {
    if (color != m_color) {
      m_color = color;
      emit colorChanged(m_color);
    }
  }

 signals:
  void nameChanged(QString);
  void colorChanged(QColor);

 public slots:

 private:
  QString m_name;
  QColor m_color;
};
#endif  // HMI_MODEL_VARMODEL_H

/*! @brief This file have the interface for VarValueModel class.
    @file varvaluemodel.h
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
#ifndef HMI_VARVALUEMODEL_H
#define HMI_VARVALUEMODEL_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

class VarValueModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(double val READ val NOTIFY valCHanged)
  Q_PROPERTY(QDateTime timestamp READ timestamp)
 public:
  explicit VarValueModel(QObject *parent = nullptr);
  explicit VarValueModel(double val, std::uint64_t timestamp,
                         QObject *parent = nullptr);
  inline double val() const noexcept { return m_val; }
  inline const QDateTime &timestamp() const noexcept { return m_timestamp; }

 signals:
  void valCHanged(double);
 public slots:

 private:
  double m_val;
  QDateTime m_timestamp;
};

#endif  // HMI_VARVALUEMODEL_H

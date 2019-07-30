/*! @brief This file have the startup/seupt for the HMI application.
    @file main.cc
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
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtWidgets/QApplication>

#include "src/hmi/model/varmodel.h"
#include "src/hmi/model/varsmodel.h"
#include "src/hmi/model/varvaluemodel.h"
#include "src/hmi/model/varvaluesmodel.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app{argc, argv};
  qmlRegisterType<VarModel>("com.github.denisacostaq.daqs", 1, 0, "VarModel");
  qmlRegisterUncreatableType<VarValueModel>(
      "com.github.denisacostaq.daqs", 1, 0, "VarValueModel",
      "Can not create var value instance in QML, use as no editable property");
  QQmlApplicationEngine engine{};
  engine.rootContext()->setContextProperty("dataLayer", new VarValuesModel{});
  engine.rootContext()->setContextProperty("varsModel", new VarsModel{});
  engine.load(QUrl{QStringLiteral("qrc:/main.qml")});
  if (engine.rootObjects().isEmpty()) {
    return -1;
  }
  return app.exec();
}

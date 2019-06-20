/*! \brief This file have the implementation for SQLiteWrapper class.
    \file sqlitewrapper.cc
    \author Alvaro Denis <denisacostaq@gmail.com>
    \date 6/19/2019

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
    [DAQs-URL]: https://github.com/denisacostaq/DAQs "DAQs"
 */
#include "src/database-server/sqlitewrapper.h"

#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include <sqlite3.h>

SQLiteWrapper::SQLiteWrapper(const std::string &db_path) : IStorage() {
  int err = sqlite3_open(db_path.c_str(), &db_);
  if (err != SQLITE_OK) {
    std::cerr << sqlite3_errmsg(db_) << "\n";
    std::cerr << std::strerror(sqlite3_system_errno(db_)) << "\n";
    throw std::string("Can't open database " + db_path);
  } else {
    std::clog << "Opened database successfully\n";
  }
}

SQLiteWrapper::~SQLiteWrapper() { sqlite3_close(db_); }

IStorage::Err SQLiteWrapper::create_scheme() {
  std::vector<std::string> stataments;
  stataments.reserve(2);
  std::string sql =
      "CREATE TABLE VARIABLE("
      "ID   INTEGER  PRIMARY KEY AUTOINCREMENT,"
      "NAME CHAR(50) NOT NULL);";
  stataments.push_back(sql);
  sql =
      "CREATE TABLE VARIABLE_VALUE("
      "ID          INTEGER  PRIMARY KEY AUTOINCREMENT,"
      "VAL         REAL,"
      "TIMESTAMP   DATETIME DEFAULT CURRENT_TIMESTAMP,"
      "VARIABLE_ID INT,"
      "FOREIGN KEY (VARIABLE_ID) REFERENCES VARIABLE(ID));";
  stataments.push_back(sql);
  for (const auto &s : stataments) {
    char *err = nullptr;
    int rc = sqlite3_exec(db_, s.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
      std::cerr<< "SQL error: " << err << "\n";
      sqlite3_free(err);
      return Err::Failed;
    } else {
      fprintf(stdout, "Table created successfully\n");
    }
  }
  return Err::Ok;
}

IStorage::Err SQLiteWrapper::add_variable(const std::string &name) {
  std::string sql =
      sqlite3_mprintf("INSERT INTO VARIABLE(NAME) VALUES('%q')", name.c_str());
  char *err = nullptr;
  int res = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
  if (res != SQLITE_OK) {
    std::cerr << "Can not inser " << name << ". " << err << "\n";
    sqlite3_free(err);
    return Err::Failed;
  }
  std::clog << "Insertio ok\n";
  return Err::Ok;
}

IStorage::Err SQLiteWrapper::add_variable_value(const std::string &var_name,
                                                float var_value) {
  return Err::Ok;
}

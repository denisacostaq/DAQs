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
#include <string>
#include <vector>

#include <sqlite3.h>

SQLiteWrapper::SQLiteWrapper(const std::string &db_path) : IDataModel() {
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

IDataModel::Err SQLiteWrapper::create_scheme() {
  std::vector<std::string> stataments;
  stataments.reserve(2);
  std::string sql =
      "CREATE TABLE VARIABLE("
      "ID   INTEGER  PRIMARY KEY AUTOINCREMENT,"
      "NAME CHAR(50) NOT NULL UNIQUE);";
  stataments.push_back(sql);
  sql =
      "CREATE TABLE VARIABLE_VALUE("
      "ID          INTEGER  PRIMARY KEY AUTOINCREMENT,"
      "VAL         DOUBLE   NOT NULL,"
      "TIMESTAMP   INTEGER NOT NULL,"
      "VARIABLE_ID INT      NOT NULL,"
      "FOREIGN KEY (VARIABLE_ID) REFERENCES VARIABLE(ID));";  // FIXME(denisacostaq@gmail.com): add constrints
  stataments.push_back(sql);
  for (const auto &s : stataments) {
    char *err = nullptr;
    int rc = sqlite3_exec(db_, s.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
      std::cerr << "SQL error: " << err << "\n";
      sqlite3_free(err);
      return Err::Failed;
    } else {
      std::clog << "Table created successfully\n";
    }
  }
  return Err::Ok;
}

IDataModel::Err SQLiteWrapper::add_variable(const std::string &name) {
  std::string sql =
      sqlite3_mprintf("INSERT INTO VARIABLE(NAME) VALUES('%q')", name.c_str());
  char *err = nullptr;
  int res = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
  if (res != SQLITE_OK) {
    std::cerr << "Can not insert " << name << ". " << err << "\n";
    sqlite3_free(err);
    return Err::Failed;
  }
  std::clog << "Insertio ok\n";
  return Err::Ok;
}

IDataModel::Err SQLiteWrapper::add_variable_value(const std::string &var_name,
                                                  double var_value) {
  char *err_msg = nullptr;
  std::string sql = sqlite3_mprintf(
      "INSERT INTO VARIABLE_VALUE(VAL, TIMESTAMP, VARIABLE_ID) VALUES(%f, %ld, "
      "(SELECT ID FROM "
      "VARIABLE WHERE NAME = '%q'))",
      var_value, std::chrono::system_clock::now().time_since_epoch().count(),
      var_name.c_str());
  if (sqlite3_exec(db_, sql.c_str(), nullptr, this, &err_msg) != SQLITE_OK) {
    std::cerr << "error " << err_msg << "\n";
    sqlite3_free(err_msg);
    return Err::Failed;
  }
  return Err::Ok;
}

IDataModel::Err SQLiteWrapper::fetch_variable_values(
    const std::string &var_name,
    const std::function<void(double value)> &send_vale) {
  char *err_msg = nullptr;
  std::string query = sqlite3_mprintf(
      "SELECT VAL FROM VARIABLE_VALUE WHERE VARIABLE_ID = (SELECT ID FROM "
      "VARIABLE WHERE NAME = '%q')",
      var_name.c_str());
  if (sqlite3_exec(
          db_, query.c_str(),
          +[](void *callback, int argc, char **argv, char **azColName) {
            for (int i = 0; i < argc; i++) {
              if (strcmp("VAL", azColName[i]) == 0) {
                double val = 0.;
                try {
                  size_t processed = 0;
                  val = std::stod(argv[i], &processed);
                } catch (std::invalid_argument e) {
                  std::cerr << e.what();
                  return -1;
                } catch (std::out_of_range e) {
                  std::cerr << e.what();
                  return -1;
                }
                (*static_cast<std::function<void(double)> *>(callback))(val);
                return 0;
              }
            }
            return -1;
          },
          const_cast<std::function<void(double)> *>(&send_vale),
          &err_msg) != SQLITE_OK) {
    std::cerr << "error " << err_msg << "\n";
    sqlite3_free(err_msg);
    return Err::Failed;
  }
  return Err::Ok;
}

IDataModel::Err SQLiteWrapper::fetch_variable_values_in_date_period(
    const std::string &var_name,
    const std::chrono::system_clock::time_point &start_date,
    const std::chrono::system_clock::time_point &end_date,
    const std::function<void(double value)> &send_vale) {
  char *err_msg = nullptr;
  const std::int64_t sd = start_date.time_since_epoch().count();
  const std::int64_t ed = end_date.time_since_epoch().count();
  std::string query = sqlite3_mprintf(
      "SELECT VAL FROM VARIABLE_VALUE WHERE VARIABLE_ID = (SELECT ID FROM "
      "VARIABLE WHERE NAME = '%q') AND TIMESTAMP >= %ld AND TIMESTAMP <= %ld;",
      var_name.c_str(), sd, ed);
  if (sqlite3_exec(
          db_, query.c_str(),
          +[](void *callback, int argc, char **argv, char **azColName) {
            for (int i = 0; i < argc; i++) {
              if (strcmp("VAL", azColName[i]) == 0) {
                double val = 0.;
                try {
                  size_t processed = 0;
                  val = std::stod(argv[i], &processed);
                } catch (std::invalid_argument e) {
                  std::cerr << e.what();
                  return -1;
                } catch (std::out_of_range e) {
                  std::cerr << e.what();
                  return -1;
                }
                (*static_cast<std::function<void(double)> *>(callback))(val);
                return 0;
              }
            }
            return -1;
          },
          const_cast<std::function<void(double)> *>(&send_vale),
          &err_msg) != SQLITE_OK) {
    std::cerr << "error " << err_msg << "\n";
    sqlite3_free(err_msg);
    return Err::Failed;
  }
  return Err::Ok;
}

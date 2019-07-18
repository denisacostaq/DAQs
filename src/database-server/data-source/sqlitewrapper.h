/*! \brief This file have the interface for SQLiteWrapper class.
    \file sqlitewrapper.h
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
#ifndef DATABASESERVER_DATAMODEL_SQLITEWRAPPER_H
#define DATABASESERVER_DATAMODEL_SQLITEWRAPPER_H

#include <string>

#include "src/database-server/data-source/idatasource.h"

class sqlite3;
class SQLiteWrapper : public IDataSource {
 public:
  /**
   * @brief SQLiteWrapper create a sqlite3 connection.
   * @throw exception if any error happen.
   * @param db_path the path to the sqlite3 db file
   * @sa IDataSource
   */
  explicit SQLiteWrapper(const std::string& db_path);

  /**
   * @brief ~SQLiteWrapper release the sqlite3 connection.
   * @sa SQLiteWrapper
   */
  ~SQLiteWrapper() override;

  /**
   * @brief create_scheme creates the database schema for sqlite3
   * @details a table with posible variables, a table with variable values and a
   * relation betwen the two (a variable have many variable values).
   * @return Err::Ok on success.
   * @sa IDataSource::create_scheme
   */
  Err create_scheme() noexcept override;

  /**
   * @brief add_variable add an entry to the variable table.
   * @param var variable info.
   * @return Err::Ok on success.
   * @sa IDataSource::add_variable
   */
  Err add_variable(const Variable& var) noexcept override;

  /**
   * @brief add_variable_value add an entry ti the vriable value table, related
   * to an entry in the variable table.
   * @param var variable value info to be inserted.
   * @return Err::Ok on success.
   * @sa IDataSource::add_variable_value
   */
  Err add_variable_value(VarValue&& var) noexcept override;

  /**
   * @brief fetch_variable_values get all values related to a variable.
   * @param var_name variable to get related values from.
   * @param send_vale get values one at a time from this callback.
   * @return Err::Ok on success.
   * @sa IDataSource::fetch_variable_values
   */
  Err fetch_variable_values(const std::string& var_name,
                            const std::function<void(VarValue&&, size_t index)>&
                                send_vale) noexcept override;

  /**
   * @brief count_variable_values count the number of values for a given
   * variable.
   * @param var_name variable to count related values from.
   * @param send_count get the values amount from this callback.
   * @return Err::Ok on success.
   */
  Err count_variable_values(
      const std::string& var_name,
      const std::function<void(size_t count)>& send_count) noexcept override;

  /**
   * @brief fetch_variable_values get all values related to a variable in a date
   * range.
   * @param var_name variable to get related values from.
   * @param start_data begin of the date range.
   * @param end_date end of the date range.
   * @param send_vale get values one at a time from this callback.
   * @return Err::Ok on success.
   */
  Err fetch_variable_values(
      const std::string& var_name,
      const std::chrono::system_clock::time_point& start_data,
      const std::chrono::system_clock::time_point& end_date,
      const std::function<void(VarValue&& val, size_t index)>&
          send_vale) noexcept override;

  /**
   * @brief count_variable_values count the number of values for a given
   * variable in a date range.
   * @param var_name variable to count related values from.
   * @param start_data begin of the date range.
   * @param end_date end of the date range.
   * @param send_count get the values amount from this callback.
   * @return Err::Ok on success.
   */
  virtual Err count_variable_values(
      const std::string& var_name,
      const std::chrono::system_clock::time_point& start_date,
      const std::chrono::system_clock::time_point& end_date,
      const std::function<void(size_t count)>& send_count) noexcept override;

 private:
  sqlite3* db_;
};

#endif  // DATABASESERVER_DATAMODEL_SQLITEWRAPPER_H

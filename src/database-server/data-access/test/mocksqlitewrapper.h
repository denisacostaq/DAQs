/*! @brief This file have the sqlitewrapper mock implementation.
    @file mocksqlitewrapper.h
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 8/5/2019

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
#ifndef MOCKSQLITEWRAPPER_H
#define MOCKSQLITEWRAPPER_H

#include <gmock/gmock.h>

#include "src/database-server/data-source/idatasource.h"

class MockIDataSource : public IDataSource {
 public:
  MOCK_METHOD0(mock_create_scheme, Err());
  Err create_scheme() noexcept override { return mock_create_scheme(); }

  MOCK_METHOD1(mock_add_variable, Err(const Variable& variable));
  Err add_variable(const Variable& variable) noexcept override {
    return mock_add_variable(variable);
  }

  MOCK_METHOD1(mock_add_variable_value, Err(const VarValue& var_value));
  Err add_variable_value(const VarValue& var_value) noexcept override {
    return mock_add_variable_value(var_value);
  }

  MOCK_METHOD1(mock_fetch_variables,
               Err(const std::function<void(const Variable& var, size_t index)>&
                       send_vale));
  Err fetch_variables(
      const std::function<void(const Variable& var, size_t index)>&
          send_vale) noexcept override {
    return mock_fetch_variables(send_vale);
  }

  MOCK_METHOD2(mock_fetch_variable_values,
               Err(const std::string& var_name,
                   const std::function<void(const VarValue& val, size_t index)>&
                       send_vale));
  Err fetch_variable_values(
      const std::string& var_name,
      const std::function<void(const VarValue& val, size_t index)>&
          send_vale) noexcept override {
    return mock_fetch_variable_values(var_name, send_vale);
  }

  MOCK_METHOD2(mock_count_variable_values,
               Err(const std::string& var_name,
                   const std::function<void(size_t count)>& send_count));
  Err count_variable_values(
      const std::string& var_name,
      const std::function<void(size_t count)>& send_count) noexcept override {
    return mock_count_variable_values(var_name, send_count);
  }

  MOCK_METHOD4(mock_fetch_variable_values,
               Err(const std::string& var_name,
                   const std::chrono::system_clock::time_point& start_date,
                   const std::chrono::system_clock::time_point& end_date,
                   const std::function<void(const VarValue&& val,
                                            size_t index)>& send_vale));
  Err fetch_variable_values(
      const std::string& var_name,
      const std::chrono::system_clock::time_point& start_date,
      const std::chrono::system_clock::time_point& end_date,
      const std::function<void(const VarValue& val, size_t index)>&
          send_vale) noexcept override {
    return mock_fetch_variable_values(var_name, start_date, end_date,
                                      send_vale);
  }

  MOCK_METHOD4(mock_count_variable_values,
               Err(const std::string& var_name,
                   const std::chrono::system_clock::time_point& start_date,
                   const std::chrono::system_clock::time_point& end_date,
                   const std::function<void(size_t count)>& send_count));
  Err count_variable_values(
      const std::string& var_name,
      const std::chrono::system_clock::time_point& start_date,
      const std::chrono::system_clock::time_point& end_date,
      const std::function<void(size_t count)>& send_count) noexcept override {
    return mock_count_variable_values(var_name, start_date, end_date,
                                      send_count);
  }
};

#endif  // MOCKSQLITEWRAPPER_H

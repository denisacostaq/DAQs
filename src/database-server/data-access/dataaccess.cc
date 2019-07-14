/*! @brief This file have the implementation for DataAccess class.
    @file dataaccess.cc
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 6/22/2019

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
#include "src/database-server/data-access/dataaccess.h"

#include <iostream>

DataAccess::DataAccess(IDataSource* ds) noexcept : IDataAccess{}, ds_{ds} {}

IDataAccess::Err DataAccess::add_variable(const std::string& name) noexcept {
  return ds_->add_variable(name) == IDataSource::Err::Ok ? Err::Ok
                                                         : Err::Failed;
}

IDataAccess::Err DataAccess::add_variable_value(VarValue&& var) noexcept {
  return ds_->add_variable_value(std::move(var)) == IDataSource::Err::Ok
             ? Err::Ok
             : Err::Failed;
}

std::tuple<std::vector<VarValue>, IDataAccess::Err>
DataAccess::fetch_variable_values(const std::string& var_name,
                                  size_t max_len) noexcept {
  std::vector<VarValue> values{};
  if (max_len != std::numeric_limits<decltype(max_len)>::infinity()) {
    try {
      values.reserve(max_len);
    } catch (const std::length_error& e) {
      std::cerr << e.what() << "\n";
      auto v{std::vector<VarValue>{}};
      auto ret{std::make_tuple(std::move(v), Err::InvalidArgument)};
      return std::move(ret);
    }
  }
  if (ds_->fetch_variable_values(var_name, [&values](VarValue&& val) {
        values.push_back(std::move(val));
      }) != IDataSource::Err::Ok) {
    auto v{std::vector<VarValue>{}};
    auto ret{std::make_tuple(std::move(v), Err::Failed)};
    return std::move(ret);
  }
  values.shrink_to_fit();
  auto ret{std::make_tuple(std::move(values), Err::Ok)};
  return std::move(ret);
}

std::tuple<std::vector<VarValue>, IDataAccess::Err>
DataAccess::fetch_variable_values(
    const std::string& var_name,
    const std::chrono::system_clock::time_point& start_date,
    const std::chrono::system_clock::time_point& end_date,
    size_t max_len) noexcept {
  std::vector<VarValue> values{};
  if (max_len != std::numeric_limits<decltype(max_len)>::infinity()) {
    try {
      values.reserve(max_len);
    } catch (const std::length_error& e) {
      std::cerr << e.what() << "\n";
      auto v{std::vector<VarValue>{}};
      auto ret{std::make_tuple(std::move(v), Err::InvalidArgument)};
      return std::move(ret);
    }
  }
  if (ds_->fetch_variable_values(var_name, start_date, end_date,
                                 [&values](VarValue&& val) {
                                   values.push_back(std::move(val));
                                 }) != IDataSource::Err::Ok) {
    auto v{std::vector<VarValue>{}};
    auto ret{std::make_tuple(std::move(v), Err::Failed)};
    return std::move(ret);
  }
  values.shrink_to_fit();
  auto ret{std::make_tuple(std::move(values), Err::Ok)};
  return std::move(ret);
}

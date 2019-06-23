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
#include "src/data-access/dataaccess.h"

#include <iostream>

DataAccess::DataAccess(IDataModel* dm) noexcept : IDataAccess{}, dm_{dm} {}

IDataAccess::Err DataAccess::add_variable(const std::string& name) noexcept {
  return dm_->add_variable(name) == IDataModel::Err::Ok ? Err::Ok : Err::Failed;
}

IDataAccess::Err DataAccess::add_variable_value(const std::string& var_name,
                                                double var_value) noexcept {
  return dm_->add_variable_value(var_name, var_value) == IDataModel::Err::Ok
             ? Err::Ok
             : Err::Failed;
}

std::tuple<std::vector<double>, IDataAccess::Err>
DataAccess::fetch_variable_values(const std::string& var_name,
                                  size_t max_len) noexcept {
  std::vector<double> values;
  if (max_len != std::numeric_limits<decltype(max_len)>::infinity()) {
    try {
      values.reserve(max_len);
    } catch (const std::length_error& e) {
      std::cerr << e.what() << "\n";
      return std::make_tuple(std::vector<double>{}, Err::InvalidArgument);
    }
  }
  if (dm_->fetch_variable_values(var_name, [&values](double val) {
        values.push_back(val);
      }) != IDataModel::Err::Ok) {
    return std::make_tuple(std::vector<double>{}, Err::Failed);
  }
  values.shrink_to_fit();
  return std::make_tuple(values, Err::Ok);
}

std::tuple<std::vector<double>, IDataAccess::Err>
DataAccess::fetch_variable_values_in_date_period(
    const std::string& var_name,
    const std::chrono::system_clock::time_point& start_date,
    const std::chrono::system_clock::time_point& end_date,
    size_t max_len) noexcept {
  std::vector<double> values;
  if (max_len != std::numeric_limits<decltype(max_len)>::infinity()) {
    try {
      values.reserve(max_len);
    } catch (const std::length_error& e) {
      std::cerr << e.what() << "\n";
      return std::make_tuple(std::vector<double>{}, Err::InvalidArgument);
    }
  }
  if (dm_->fetch_variable_values_in_date_period(var_name, start_date, end_date,
                                                [&values](double val) {
                                                  values.push_back(val);
                                                }) != IDataModel::Err::Ok) {
    return std::make_tuple(std::vector<double>{}, Err::Failed);
  }
  values.shrink_to_fit();
  return std::make_tuple(values, Err::Ok);
}

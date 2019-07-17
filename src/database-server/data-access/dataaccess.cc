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

#include <algorithm>
#include <cmath>

DataAccess::DataAccess(IDataSource* ds) noexcept : IDataAccess{}, ds_{ds} {}

IDataAccess::Err DataAccess::add_variable(const Variable& var) noexcept {
  return ds_->add_variable(var) == IDataSource::Err::Ok ? Err::Ok : Err::Failed;
}

IDataAccess::Err DataAccess::add_variable_value(VarValue&& var) noexcept {
  return ds_->add_variable_value(std::move(var)) == IDataSource::Err::Ok
             ? Err::Ok
             : Err::Failed;
}

IDataAccess::Err DataAccess::compress(const std::vector<VarValue>& in_vals,
                                      std::vector<VarValue>* out_vals,
                                      size_t max_len) noexcept {
  try {
    out_vals->reserve(max_len);
  } catch (const std::length_error& e) {
    std::cerr << e.what() << "\n";
    return IDataAccess::Err::InvalidArgument;
  }
  auto apply_compression{[&in_vals, &out_vals](size_t chunk_size,
                                               size_t in_vals_offset,
                                               size_t chunk_ammont) {
    auto begin{in_vals.begin()};
    std::advance(begin, in_vals_offset);
    auto mid{in_vals.begin()};
    std::advance(mid, in_vals_offset + chunk_size / 2);
    auto end{in_vals.begin()};
    std::advance(end, in_vals_offset + chunk_size);
    for (size_t i{0}; i < chunk_ammont; ++i, std::advance(begin, chunk_size),
                                        std::advance(end, chunk_size),
                                        std::advance(mid, chunk_size)) {
      double sum{.0};
      std::for_each(begin, end,
                    [&sum](const VarValue& val) { sum += val.val(); });
      VarValue val{};
      val.set_val(sum / chunk_size);
      val.set_timestamp(mid->timestamp());
      out_vals->emplace_back(std::move(val));
    }
  }};
  auto ceil_chunk_size{static_cast<size_t>(
      std::ceil(static_cast<double>(in_vals.size()) / max_len))};
  auto ceil_ammount{(in_vals.size() % max_len) ? in_vals.size() % max_len
                                               : in_vals.size() / max_len};
  apply_compression(ceil_chunk_size, 0, ceil_ammount);
  auto floor_chunk_sieze{in_vals.size() / max_len};
  auto floor_ammount{max_len - ceil_ammount};
  apply_compression(floor_chunk_sieze, ceil_chunk_size * ceil_ammount,
                    floor_ammount);
  return Err::Ok;
}

std::tuple<std::vector<VarValue>, IDataAccess::Err>
DataAccess::fetch_variable_values(const std::string& var_name,
                                  size_t max_len) noexcept {
  std::vector<VarValue> tmp_values{};
  if (ds_->fetch_variable_values(var_name,
                                 [&tmp_values](VarValue&& val, size_t index) {
                                   tmp_values.push_back(std::move(val));
                                 }) != IDataSource::Err::Ok) {
    return std::make_tuple(std::vector<VarValue>{}, Err::Failed);
  }
  if (max_len != std::numeric_limits<decltype(max_len)>::infinity() &&
      tmp_values.size() != max_len) {
    std::vector<VarValue> values{};
    auto err{compress(tmp_values, &values, max_len)};
    if (err != Err::Ok) {
      return std::make_tuple(std::vector<VarValue>{}, Err::Ok);
    }
    return std::make_tuple(std::move(values), Err::Ok);
  }
  return std::make_tuple(std::move(tmp_values), Err::Ok);
}

std::tuple<std::vector<VarValue>, IDataAccess::Err>
DataAccess::fetch_variable_values(
    const std::string& var_name,
    const std::chrono::system_clock::time_point& start_date,
    const std::chrono::system_clock::time_point& end_date,
    size_t max_len) noexcept {
  std::vector<VarValue> tmp_values{};
  if (ds_->fetch_variable_values(var_name, start_date, end_date,
                                 [&tmp_values](VarValue&& val, size_t index) {
                                   tmp_values.push_back(std::move(val));
                                 }) != IDataSource::Err::Ok) {
    return std::make_tuple(std::vector<VarValue>{}, Err::Failed);
  }
  if (max_len != std::numeric_limits<decltype(max_len)>::infinity() &&
      tmp_values.size() != max_len) {
    std::vector<VarValue> values{};
    auto err{compress(tmp_values, &values, max_len)};
    if (err != Err::Ok) {
      return std::make_tuple(std::vector<VarValue>{}, Err::Ok);
    }
    return std::make_tuple(std::move(values), Err::Ok);
  }
  return std::make_tuple(std::move(tmp_values), Err::Ok);
}

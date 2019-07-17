/*! @brief This file have the implementation for DataAccessTest class.
    @file dataaccesstest.cc
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 7/16/2019

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

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "src/database-server/data-access/dataaccess.h"

//FIXME(denisacostaq@gmail.com): improve all tests based on WithModValues3
TEST(DataCompression, WithoutModSize) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  vals.reserve(10);
  for (int i{0}; i < 10; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, 34.2, static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, 5));
  EXPECT_EQ(5, compresed.size());
}

TEST(DataCompression, WithModSize) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  constexpr size_t vals_size{10};
  vals.reserve(10);
  for (size_t i{0}; i < vals_size; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, 34.2, static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  const size_t cmprs_size{5};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, cmprs_size));
  EXPECT_EQ(cmprs_size, compresed.size());
}

TEST(DataCompression, WithoutModValues1) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  constexpr size_t vals_size{10};
  vals.reserve(vals_size);
  for (size_t i{0}; i < vals_size; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, static_cast<double>(i + 1),
                 static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  const size_t cmprs_size{5};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, cmprs_size));
  EXPECT_EQ(cmprs_size, compresed.size());
  for (size_t i{0}; i < compresed.size(); ++i) {
    EXPECT_DOUBLE_EQ(compresed[i].val(), i * 2 + 1.5);
  }
}

TEST(DataCompression, WithoutModValues2) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  vals.reserve(100);
  for (int i{0}; i < 100; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, static_cast<double>(i + 1),
                 static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  const size_t cmprs_size{10};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, cmprs_size));
  EXPECT_EQ(cmprs_size, compresed.size());
  double piv{.0};
  for (size_t i{1}; i <= cmprs_size; ++i) {
    piv += i;
  }
  for (size_t i{0}; i < compresed.size(); ++i) {
    EXPECT_DOUBLE_EQ(cmprs_size * i + piv / cmprs_size, compresed[i].val());
  }
}

TEST(DataCompression, WithModValues1) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  constexpr size_t vals_ammount{8};
  vals.reserve(vals_ammount);
  for (size_t i{0}; i < vals_ammount; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, static_cast<double>(i + 1),
                 static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  constexpr size_t cmprs_size{5};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, cmprs_size));
  EXPECT_EQ(cmprs_size, compresed.size());
  constexpr size_t mod{vals_ammount % cmprs_size};
  for (size_t i{0}; i < mod; ++i) {
    EXPECT_DOUBLE_EQ(i * 2 + 1.5, compresed[i].val());
  }
  for (size_t i{mod}; i < cmprs_size; ++i) {
    EXPECT_DOUBLE_EQ(vals[mod + i].val(), compresed[i].val());
  }
}

TEST(DataCompression, WithModValues2) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  constexpr size_t vals_ammount{97};
  vals.reserve(vals_ammount);
  for (size_t i{0}; i < vals_ammount; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, static_cast<double>(i + 1),
                 static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  constexpr size_t cmprs_size{10};
  constexpr size_t mod{vals_ammount % cmprs_size};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, cmprs_size));
  EXPECT_EQ(cmprs_size, compresed.size());
  double piv{.0};
  for (size_t i{1}; i <= cmprs_size; ++i) {
    piv += i;
  }
  for (size_t i{0}; i < mod; ++i) {
    EXPECT_DOUBLE_EQ(cmprs_size * i + piv / cmprs_size, compresed[i].val());
  }
  double last{.0};
  for (size_t i{0}; i < mod; ++i) {
    last += vals_ammount - mod + i;
  }
  EXPECT_DOUBLE_EQ(last / mod, compresed.back().val());
}

TEST(DataCompression, WithModValues3) {
  DataAccess da{nullptr};
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  std::vector<VarValue> vals;
  constexpr size_t vals_ammount{74};
  vals.reserve(vals_ammount);
  for (size_t i{0}; i < vals_ammount; ++i) {
    std::chrono::system_clock::now();
    auto now{std::chrono::system_clock::now()};
    auto timestamp{std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch())};
    VarValue val{var, static_cast<double>(i + 1),
                 static_cast<std::uint64_t>(timestamp.count())};
    vals.emplace_back(std::move(val));
  }
  std::vector<VarValue> compresed{};
  constexpr size_t cmprs_size{10};
  EXPECT_EQ(IDataAccess::Err::Ok, da.compress(vals, &compresed, cmprs_size));
  EXPECT_EQ(cmprs_size, compresed.size());
  constexpr size_t mod{vals_ammount % cmprs_size};
  const double ceil{std::ceil(static_cast<double>(vals_ammount) / cmprs_size)};
  constexpr double floor{vals_ammount / cmprs_size};
  double piv{.0};
  for (size_t i{1}; i <= ceil; ++i) {
    piv += i;
  }
  for (size_t i{0}; i < mod; ++i) {
    EXPECT_DOUBLE_EQ(ceil * i + piv / ceil, compresed[i].val());
  }
  for (size_t i{mod}; i < cmprs_size; ++i) {
    double sum{.0};
    for (int j{1}; j <= floor; ++j) {
      sum += mod * ceil + ((i - mod) * floor) + j;
    }
    EXPECT_DOUBLE_EQ(sum / floor, compresed[i].val());
  }
}

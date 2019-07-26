/*! @brief This file have the implementation for sqlitedatamodel integration
   tests
    @file sqlitedatasource_integration_test.cc
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 6/22/2019

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

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "src/database-server/data-source/sqlitewrapper.h"
#include "src/database-server/data-source/test/testutil.h"

TEST(NotInitializedSchema, CanNotAddVariable) {
  IDataSource* ds = nullptr;
  try {
    ds = new SQLiteWrapper{get_random_sqlite_file_path()};
  } catch (const std::string& msg) {
    std::cerr << msg << "\n";
  } catch (...) {
    std::cerr << "Unextpected error\n";
  }
  EXPECT_NE(nullptr, ds);
  // FIXME(denisacostaq@gmail.com): "color"
  Variable var{"temp", "color"};
  EXPECT_NE(IDataSource::Err::Ok, ds->add_variable(var));
  delete ds;
}

TEST(OpenDb, CanOpenDb) {
  auto path = get_random_sqlite_file_path();
  EXPECT_NO_THROW(SQLiteWrapper{path});
}

TEST(OpenDb, CanNotOpenDb) {
  EXPECT_THROW(SQLiteWrapper("/pepe"), std::string);
}

class SQLiteWrapperTest : public ::testing::Test {
 protected:
  SQLiteWrapperTest() : ds_{nullptr} {}
  void SetUp() override {
    try {
      ds_ = new SQLiteWrapper{get_random_sqlite_file_path()};
    } catch (const std::string& msg) {
      std::cerr << msg << "\n";
    } catch (...) {
      std::cerr << "Unextpected error\n";
    }
    EXPECT_NE(nullptr, ds_);
    EXPECT_EQ(IDataSource::Err::Ok, ds_->create_scheme());
  }

  void TearDown() override { delete ds_; }

  IDataSource* ds_ = nullptr;
};

TEST_F(SQLiteWrapperTest, AddVariable) {
  // TODO(denisacostaq@gmail.com): "color"
  Variable var1{"var1", "color"};
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(var1));
  // TODO(denisacostaq@gmail.com): "color"
  Variable var2{"var1", "color"};
  EXPECT_NE(IDataSource::Err::Ok, ds_->add_variable(var2));
  // TODO(denisacostaq@gmail.com): "color"
  Variable var3{"var2", "color"};
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(var3));
}

TEST_F(SQLiteWrapperTest, AddVariableValue) {
  // FIXME(denisacostaq@gmail.com)" color
  Variable variable("var1", "color");
  VarValue var{variable, 23.1, 0};
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(variable));
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(var)));
  // FIXME(denisacostaq@gmail.com)" color
  Variable varNone{"varNone", "color"};
  EXPECT_NE(IDataSource::Err::Ok,
            ds_->add_variable_value(VarValue{varNone, 13.1, 0}));
  VarValue var2{variable, 21.1, 0};
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(var2)));
}

TEST_F(SQLiteWrapperTest, RetrieveVariableValue) {
  // FIXME(denisacostaq@gmail.com)" color
  Variable var1{"var1", "color"};
  VarValue varValue1{var1, 0, 0};
  // FIXME(denisacostaq@gmail.com)" color
  Variable varNone{"varNone", "color"};
  VarValue varValueNone{varNone, 0, 0};
  std::vector<double> var1OrgValues{23.1, 21.1};
  std::vector<double> varNoneOrgValues{13.1, 13.2, 3.32};
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(var1));
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(varNone));
  for (auto v : var1OrgValues) {
    auto t = varValue1.DeepCopy();
    auto vv{varValue1.DeepCopy()};
    vv.set_val(v);
    EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(vv)));
  }
  for (auto v : varNoneOrgValues) {
    auto vv{varValueNone.DeepCopy()};
    vv.set_val(v);
    EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(vv)));
  }
  std::vector<VarValue> var1Values, varNoneValues;
  auto callback = [](std::vector<VarValue>* container, VarValue&& val) mutable {
    container->push_back(std::move(val));
  };
  auto var1ValuesCallback =
      std::bind(callback, &var1Values, std::placeholders::_1);
  auto varNoneValuesCallback =
      std::bind(callback, &varNoneValues, std::placeholders::_1);
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->fetch_variable_values(var1.name(), var1ValuesCallback));
  EXPECT_EQ(var1OrgValues.size(), var1Values.size());
  for (auto v : var1OrgValues) {
    EXPECT_NE(std::find_if(
                  var1Values.cbegin(), var1Values.cend(),
                  [v](const VarValue& var1Val) { return var1Val.val() == v; }),
              var1Values.end());
  }
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->fetch_variable_values(varNone.name(), varNoneValuesCallback));
  EXPECT_EQ(varNoneOrgValues.size(), varNoneValues.size());
  for (auto v : varNoneOrgValues) {
    EXPECT_NE(std::find_if(
                  varNoneValues.cbegin(), varNoneValues.cend(),
                  [v](const VarValue& var1Val) { return var1Val.val() == v; }),
              varNoneValues.end());
  }
}

TEST_F(SQLiteWrapperTest, RetrieveVariableValueInDateRanges) {
  // FIXME(denisacostaq@gmail.com)" color
  Variable var1{"var1", "color"};
  const int ammount{100};
  std::vector<double> var1OrgValues{};
  var1OrgValues.reserve(100);
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(var1));
  for (int i = 0; i < ammount; ++i) {
    var1OrgValues.push_back(i);
  }
  std::vector<std::chrono::system_clock::time_point> laps;
  laps.push_back(std::chrono::system_clock::now());
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  for (size_t i = 0; i < var1OrgValues.size(); ++i) {
    VarValue vvar1{var1, var1OrgValues[i], 0};
    EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(vvar1)));
    if (i == 30 || i == 60 || i == 90) {
      laps.push_back(std::chrono::system_clock::now());
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }
  laps.push_back(std::chrono::system_clock::now());
  int count{0};
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->fetch_variable_values(
                var1.name(), laps[0], laps[1],
                // TODO(denisacostaq@gmail.com): use index and/or count_
                [&count](VarValue&&, size_t index) { ++count; }));
  EXPECT_EQ(31, count);
  count = 0;
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->fetch_variable_values(
                var1.name(), laps[1], laps[2],
                // TODO(denisacostaq@gmail.com): use index and/or count_
                [&count](VarValue&&, size_t index) { ++count; }));
  EXPECT_EQ(30, count);
  count = 0;
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->fetch_variable_values(
                var1.name(), laps[2], laps[3],
                // TODO(denisacostaq@gmail.com): use index and/or count_
                [&count](VarValue&&, size_t index) { ++count; }));
  EXPECT_EQ(30, count);
  count = 0;
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->fetch_variable_values(
                var1.name(), laps[3], laps[4],
                // TODO(denisacostaq@gmail.com): use index and/or count_
                [&count](VarValue&&, size_t index) { ++count; }));
  EXPECT_EQ(9, count);
}

TEST_F(SQLiteWrapperTest, CountVariableValues) {
  // FIXME(denisacostaq@gmail.com)" color
  Variable var1{"var1", "color"};
  VarValue varValue1{var1, 0, 0};
  // FIXME(denisacostaq@gmail.com)" color
  Variable varNone{"varNone", "color"};
  VarValue varValueNone{varNone, 0, 0};
  std::vector<double> var1OrgValues{23.1, 21.1};
  std::vector<double> varNoneOrgValues{13.1, 13.2, 3.32};
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(var1));
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(varNone));
  for (auto v : var1OrgValues) {
    auto t = varValue1.DeepCopy();
    auto vv{varValue1.DeepCopy()};
    vv.set_val(v);
    EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(vv)));
  }
  for (auto v : varNoneOrgValues) {
    auto vv{varValueNone.DeepCopy()};
    vv.set_val(v);
    EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(vv)));
  }
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->count_variable_values(var1.name(),
                                       [&var1OrgValues](std::size_t count) {
                                         EXPECT_EQ(var1OrgValues.size(), count);
                                       }));
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->count_variable_values(
                varNone.name(), [&varNoneOrgValues](std::size_t count) {
                  EXPECT_EQ(varNoneOrgValues.size(), count);
                }));
}

TEST_F(SQLiteWrapperTest, CountVariableValuesInRange) {
  // FIXME(denisacostaq@gmail.com)" color
  Variable var1{"var1", "color"};
  const int ammount{100};
  std::vector<double> var1OrgValues{};
  var1OrgValues.reserve(100);
  EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable(var1));
  for (int i = 0; i < ammount; ++i) {
    var1OrgValues.push_back(i);
  }
  std::vector<std::chrono::system_clock::time_point> laps;
  laps.push_back(std::chrono::system_clock::now());
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  for (size_t i = 0; i < var1OrgValues.size(); ++i) {
    VarValue vvar1{var1, var1OrgValues[i], 0};
    EXPECT_EQ(IDataSource::Err::Ok, ds_->add_variable_value(std::move(vvar1)));
    if (i == 30 || i == 60 || i == 90) {
      laps.push_back(std::chrono::system_clock::now());
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }
  laps.push_back(std::chrono::system_clock::now());
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->count_variable_values(
                var1.name(), laps[0], laps[1],
                [](std::size_t count) { EXPECT_EQ(31, count); }));
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->count_variable_values(
                var1.name(), laps[1], laps[2],
                [](std::size_t count) { EXPECT_EQ(30, count); }));
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->count_variable_values(
                var1.name(), laps[2], laps[3],
                [](std::size_t count) { EXPECT_EQ(30, count); }));
  EXPECT_EQ(IDataSource::Err::Ok,
            ds_->count_variable_values(
                var1.name(), laps[3], laps[4],
                [](std::size_t count) { EXPECT_EQ(9, count); }));
}

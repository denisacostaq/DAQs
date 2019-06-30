/*! @brief This file have the implementation for sqlitedatamodel integration
   tests
    @file sqlitedatamodel_integration_test.cc
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

#include "src/database-server/data-model/sqlitewrapper.h"
#include "src/database-server/data-model/test/testutil.h"

TEST(NotInitializedSchema, CanNotAddVariable) {
  IDataModel* dm = nullptr;
  try {
    dm = new SQLiteWrapper(get_random_sqlite_file_path());
  } catch (const std::string& msg) {
    std::cerr << msg << "\n";
  } catch (...) {
    std::cerr << "Unextpected error\n";
  }
  EXPECT_NE(nullptr, dm);
  EXPECT_NE(IDataModel::Err::Ok, dm->add_variable("temp"));
  delete dm;
}

TEST(OpenDb, CanOpenDb) {
  auto path = get_random_sqlite_file_path();
  EXPECT_NO_THROW(SQLiteWrapper(path.c_str()));
}

TEST(OpenDb, CanNotOpenDb) {
  EXPECT_THROW(SQLiteWrapper("/pepe"), std::string);
}

class SQLiteWrapperTest : public ::testing::Test {
 protected:
  SQLiteWrapperTest() : dm_{nullptr} {}
  void SetUp() override {
    try {
      dm_ = new SQLiteWrapper(get_random_sqlite_file_path());
    } catch (const std::string& msg) {
      std::cerr << msg << "\n";
    } catch (...) {
      std::cerr << "Unextpected error\n";
    }
    EXPECT_NE(nullptr, dm_);
    EXPECT_EQ(IDataModel::Err::Ok, dm_->create_scheme());
  }

  void TearDown() override { delete dm_; }

  IDataModel* dm_ = nullptr;
};

TEST_F(SQLiteWrapperTest, AddVariable) {
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable("var1"));
  EXPECT_NE(IDataModel::Err::Ok, dm_->add_variable("var1"));
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable("var2"));
}

TEST_F(SQLiteWrapperTest, AddVariableValue) {
  IDataModel::VarValue var{"var1", 23.1};
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable(var.name));
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable_value(var));
  EXPECT_NE(IDataModel::Err::Ok,
            dm_->add_variable_value(IDataModel::VarValue{"varNone", 13.1}));
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->add_variable_value(IDataModel::VarValue{"var1", 21.1}));
}

TEST_F(SQLiteWrapperTest, RetrieveVariableValue) {
  IDataModel::VarValue var1{"var1"};
  IDataModel::VarValue varNone{"varNone"};
  std::vector<double> var1OrgValues{23.1, 21.1};
  std::vector<double> varNoneOrgValues{13.1, 13.2, 3.32};
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable(var1.name));
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable(varNone.name));
  for (auto v : var1OrgValues) {
    var1.val = v;
    EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable_value(var1));
  }
  for (auto v : varNoneOrgValues) {
    varNone.val = v;
    EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable_value(varNone));
  }
  std::vector<IDataModel::VarValue> var1Values, varNoneValues;
  IDataModel::VarValue varCopy{};
  auto callback = [varCopy](std::vector<IDataModel::VarValue>* container,
                     const IDataModel::VarValue& val) mutable {
    container->push_back(val);
  };
  auto var1ValuesCallback =
      std::bind(callback, &var1Values, std::placeholders::_1);
  auto varNoneValuesCallback =
      std::bind(callback, &varNoneValues, std::placeholders::_1);
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->fetch_variable_values(var1.name, var1ValuesCallback));
  EXPECT_EQ(var1OrgValues.size(), var1Values.size());
  for (auto v : var1OrgValues) {
    EXPECT_NE(std::find_if(var1Values.cbegin(), var1Values.cend(),
                        [v](const IDataModel::VarValue& var1Val) {
                          return var1Val.val == v;
                        }),
              var1Values.end());
  }
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->fetch_variable_values(varNone.name, varNoneValuesCallback));
  EXPECT_EQ(varNoneOrgValues.size(), varNoneValues.size());
  for (auto v : varNoneOrgValues) {
    EXPECT_NE(std::find_if(varNoneValues.cbegin(), varNoneValues.cend(),
                        [v](const IDataModel::VarValue& var1Val) {
                          return var1Val.val == v;
                        }),
              varNoneValues.end());
  }
}

TEST_F(SQLiteWrapperTest, RetrieveVariableValueInDateRanges) {
  IDataModel::VarValue var1{"var1"};
  const int ammount = 100;
  std::vector<double> var1OrgValues;
  var1OrgValues.reserve(100);
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable(var1.name));
  for (int i = 0; i < ammount; ++i) {
    var1OrgValues.push_back(i);
  }
  std::vector<std::chrono::system_clock::time_point> laps;
  laps.push_back(std::chrono::system_clock::now());
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  for (size_t i = 0; i < var1OrgValues.size(); ++i) {
    var1.val = var1OrgValues[i];
    EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable_value(var1));
    if (i == 30 || i == 60 || i == 90) {
      laps.push_back(std::chrono::system_clock::now());
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }
  laps.push_back(std::chrono::system_clock::now());
  int count = 0;
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->fetch_variable_values(
                var1.name, laps[0], laps[1],
                [&count](const IDataModel::VarValue&) { ++count; }));
  EXPECT_EQ(31, count);
  count = 0;
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->fetch_variable_values(
                var1.name, laps[1], laps[2],
                [&count](const IDataModel::VarValue&) { ++count; }));
  EXPECT_EQ(30, count);
  count = 0;
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->fetch_variable_values(
                var1.name, laps[2], laps[3],
                [&count](const IDataModel::VarValue&) { ++count; }));
  EXPECT_EQ(30, count);
  count = 0;
  EXPECT_EQ(IDataModel::Err::Ok,
            dm_->fetch_variable_values(
                var1.name, laps[3], laps[4],
                [&count](const IDataModel::VarValue&) { ++count; }));
  EXPECT_EQ(9, count);
}

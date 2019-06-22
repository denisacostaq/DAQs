/*! \brief This file have the implementation for sqlitedatamodel integration tests
    \file sqlitedatamodel_integration_test.cc
    \author Alvaro Denis <denisacostaq@gmail.com>
    \date 6/22/2019

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

#include <gtest/gtest.h>

#include "src/database-server/sqlitewrapper.h"
#include "src/database-server/test/testutil.h"


class SQLiteWrapperTest : public ::testing::Test {
 protected:
  void SetUp() override {
    dm_ = new SQLiteWrapper(get_random_sqlite_file_path());
    EXPECT_EQ(IDataModel::Err::Ok, dm_->create_scheme());
  }

  void TearDown() override {
    delete dm_;
  }

  IDataModel* dm_ = nullptr;
};

TEST_F(SQLiteWrapperTest, AddVariable) {
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable("var1"));
  EXPECT_NE(IDataModel::Err::Ok, dm_->add_variable("var1"));
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable("var2"));
}

TEST_F(SQLiteWrapperTest, AddVariableValue) {
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable("var1"));
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable_value("var1", 23.1));
  EXPECT_NE(IDataModel::Err::Ok, dm_->add_variable_value("varNone", 13.1));
  EXPECT_EQ(IDataModel::Err::Ok, dm_->add_variable_value("var1", 21.1));
}

TEST(NotInitializedSchema, CanNotAddVariable) {
  IDataModel* dm = nullptr;
   try {
     dm = new SQLiteWrapper(get_random_sqlite_file_path());
   } catch (std::string msg) {
     std::cerr << msg << "\n";
   } catch (...) {
     std::cerr << "Unextpected error\n";
   }
  EXPECT_NE(IDataModel::Err::Ok, dm->add_variable("temp"));
  delete dm;
}

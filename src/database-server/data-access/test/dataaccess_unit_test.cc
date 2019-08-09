/*! @brief This file have the data access unit test.
    @file dataaccess_unit_test.cc
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

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "src/database-server/data-access/dataaccess.h"
#include "src/database-server/data-access/test/mocksqlitewrapper.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

bool operator==(const Variable& v1, const Variable& v2) {
  return v1.name() == v2.name() && v1.color() == v2.color();
}
bool operator==(const VarValue& v1, const VarValue& v2) {
  return v1.val() == v2.val() && v1.timestamp() == v2.timestamp() &&
         v1.variable() == v2.variable();
}

std::ostream& operator<<(std::ostream& stream, const Variable& var) {
  return stream << "Variable (name=" << var.name() << " color=" << var.color()
                << ")";
}
std::ostream& operator<<(std::ostream& stream, const VarValue& var) {
  return stream << "VarVal (val=" << var.val()
                << " timestamp=" << var.timestamp()
                << "variable=" << var.variable() << ")";
}

TEST(DataAccessShouldHandleDataSourceResponses, add_variable) {
  MockIDataSource mds{};
  EXPECT_CALL(mds, mock_add_variable(_))
      .WillOnce(Return(IDataSource::Err::Ok))
      .WillOnce(Return(IDataSource::Err::Failed));
  DataAccess da{&mds};
  EXPECT_EQ(IDataAccess::Err::Ok, da.add_variable(Variable{}));
  EXPECT_EQ(IDataAccess::Err::Failed, da.add_variable(Variable{}));
}

TEST(DataAccessShouldHandleDataSourceResponses, add_variable_value) {
  MockIDataSource mds{};
  EXPECT_CALL(mds, mock_add_variable_value(_))
      .WillOnce(Return(IDataSource::Err::Ok))
      .WillOnce(Return(IDataSource::Err::Failed));
  DataAccess da{&mds};
  EXPECT_EQ(IDataAccess::Err::Ok, da.add_variable_value(VarValue{}));
  EXPECT_EQ(IDataAccess::Err::Failed, da.add_variable_value(VarValue{}));
}

TEST(DataAccessShouldHandleDataSourceResponses, fetch_variables) {
  MockIDataSource mds{};
  EXPECT_CALL(mds, mock_fetch_variables(_))
      .WillOnce(Return(IDataSource::Err::Ok))
      .WillOnce(Return(IDataSource::Err::Failed));
  DataAccess da{&mds};
  EXPECT_EQ(std::make_tuple(std::vector<Variable>{}, IDataAccess::Err::Ok),
            da.fetch_variables());
  EXPECT_EQ(std::make_tuple(std::vector<Variable>{}, IDataAccess::Err::Failed),
            da.fetch_variables());
}

TEST(DataAccessShouldHandleDataSourceResponses, fetch_variable_values) {
  MockIDataSource mds{};
  EXPECT_CALL(mds, mock_fetch_variable_values(_, _))
      .WillOnce(Return(IDataSource::Err::Ok))
      .WillOnce(Return(IDataSource::Err::Failed));
  DataAccess da{&mds};
  EXPECT_EQ(std::make_tuple(std::vector<VarValue>{}, IDataAccess::Err::Ok),
            da.fetch_variable_values("", 0));
  EXPECT_EQ(std::make_tuple(std::vector<VarValue>{}, IDataAccess::Err::Failed),
            da.fetch_variable_values("", 0));
}

TEST(DataAccessShouldHandleDataSourceResponses,
     fetch_variable_values_in_range) {
  MockIDataSource mds{};
  EXPECT_CALL(mds, mock_fetch_variable_values(_, _, _, _))
      .WillOnce(Return(IDataSource::Err::Ok))
      .WillOnce(Return(IDataSource::Err::Failed));
  DataAccess da{&mds};
  auto tp{std::chrono::system_clock::now()};
  EXPECT_EQ(std::make_tuple(std::vector<VarValue>{}, IDataAccess::Err::Ok),
            da.fetch_variable_values("", tp, tp, 0));
  EXPECT_EQ(std::make_tuple(std::vector<VarValue>{}, IDataAccess::Err::Failed),
            da.fetch_variable_values("", tp, tp, 0));
}

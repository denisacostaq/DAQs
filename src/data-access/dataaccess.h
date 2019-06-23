/*! @brief This file have the interface for DataAccess class.
    @file dataaccess.h
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
#ifndef DATAACCESS_H
#define DATAACCESS_H

#include "src/data-access/idataaccess.h"

#include "src/data-model/idatamodel.h"

class DataAccess : public IDataAccess {
 public:
  DataAccess(IDataModel* dm) noexcept;

  /**
   * @brief add_variable add a new variable.
   * @param name variable name.
   * @return Ok on success.
   * @sa IDataAccess::add_variable
   */
  Err add_variable(const std::string& name) noexcept override;

  /**
   * @brief add_variable_value add a new value for a given variable.
   * @param var_name variable name.
   * @param var_value variable value.
   * @return Ok on success.
   * @sa IDataAccess::add_variable_value
   */
  Err add_variable_value(const std::string& var_name,
                         double var_value) noexcept override;

  /**
   * @brief fetch_variable_values get values for a given variable.
   * @param var_name variable name.
   * @param max_len TODO(denisacostaq@gmail.com): not implemented yet
   * @return a vector of values if any and an error code.
   * @sa IDataAccess::fetch_variable_values
   */
  std::tuple<std::vector<double>, Err> fetch_variable_values(
      const std::string& var_name, size_t max_len) noexcept override;

  /**
   * @brief fetch_variable_values_in_date_period get values for a given variable
   * in a period.
   * @param var_name variable name.
   * @param start_date start date.
   * @param end_date end date
   * @param max_len TODO(denisacostaq@gmail.com): not implemented yet
   * @return a vector of values if any and an error code.
   * @sa IDataAccess::fetch_variable_values_in_date_period
   */
  std::tuple<std::vector<double>, Err> fetch_variable_values_in_date_period(
      const std::string& var_name,
      const std::chrono::system_clock::time_point& start_date,
      const std::chrono::system_clock::time_point& end_date,
      size_t max_len) noexcept override;

 private:
  IDataModel* dm_;
};
#endif  // DATAACCESS_H

/*! @brief This file have the interface for the IDataModel class.
    @file IDataModel.h
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 6/19/2019

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
#ifndef DATABASE_SERVER_IDATAMODEL_H
#define DATABASE_SERVER_IDATAMODEL_H

#include <chrono>
#include <functional>
#include <string>

/**
 * @brief The IDataModel class is an interface for the data model
 * @details A variable is related for example for sensors, like temperature,
 * luminosity, ... A value is for example the numerica value for a
 * temperature sensor.
 * @sa add_variable_value, add_variable_value
 */
class IDataModel {
 public:
  enum class Err { Ok, Failed };

  IDataModel() = default;
  virtual ~IDataModel() = default;
  IDataModel(const IDataModel&) = delete;
  IDataModel& operator=(const IDataModel&) = delete;
  IDataModel(IDataModel&&) = default;
  IDataModel& operator=(IDataModel&&) = default;

  /**
   * @brief create_scheme create data storage persistence, e.g. schema for a
   * relational database
   * @return Err::Ok on succes
   */
  virtual Err create_scheme() = 0;

  /**
   * @brief add_variable add a variable to be tracket for the system (by default
   * the only supported variabe value type is double)
   * @param name of the variable
   * @return Err::Ok on succes
   * @sa add_variable_value
   */
  virtual Err add_variable(const std::string& name) = 0;

  /**
   * @brief add_variable_value add a new value for the variable.
   * @param var_name variable in which this value will be added.
   * @param var_value value of the variable.
   * @return Err::Ok on succes
   * @sa add_variable
   */
  virtual Err add_variable_value(const std::string& var_name,
                                 double var_value) = 0;

  /**
   * @brief fetch_variable_values get all values of a given variables
   * @param var_name variable name to get the values from
   * @param send_vale the values will be send in this callback, one at a time
   * @return Err::Ok on succes
   */
  virtual Err fetch_variable_values(
      const std::string& var_name,
      const std::function<void(double value)>& send_vale) = 0;

  /**
   * @brief fetch_variable_values_in_date_period  get all values of a given
   * variables in a date range
   * @param var_name variable name to get the values from
   * @param start_date begin of the date range
   * @param end_date end of the date range
   * @param send_vale the values will be send in this callback, one at a time
   * @return Err::Ok on succes
   */
  virtual Err fetch_variable_values_in_date_period(
      const std::string& var_name,
      const std::chrono::system_clock::time_point& start_date,
      const std::chrono::system_clock::time_point& end_date,
      const std::function<void(double value)>& send_vale) = 0;
};

#endif  //  DATABASE_SERVER_IDATAMODEL_H

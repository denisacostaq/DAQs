/*! @brief This file have the interface for VarValue class.
    @file varvalue.h
    @author Alvaro Denis <denisacostaq@gmail.com>
    @date 7/13/2019

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
#ifndef VARVALUE_H
#define VARVALUE_H

#include "src/database-server/data-model/variable.h"

class VarValue {
 public:
  VarValue() = default;
  VarValue(const Variable &variable, double val, std::uint64_t ts);
  VarValue(Variable &&variable, double &&val, std::uint64_t &&ts);
  VarValue(VarValue &&var) noexcept;

  const Variable &variable() const noexcept;
  void set_vaiable(Variable &&var) noexcept;
  double val() const noexcept;
  void set_val(double val) noexcept;
  std::uint64_t timestamp() const noexcept;
  void set_timestamp(std::uint64_t ts) noexcept;
  const std::string &name() const noexcept;

  VarValue DeepCopy() {
    VarValue v{};
    v.val_ = this->val_;
    v.variable_ = this->variable_;
    v.timestamp_ = this->timestamp_;
    return std::move(v);
  }

 private:
  Variable variable_;
  double val_;
  std::uint64_t timestamp_;
};

#endif  // VARVALUE_H

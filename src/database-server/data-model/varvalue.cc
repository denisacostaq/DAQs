/*! @brief This file have the implementation for VarValue class.
    @file varvalue.cc
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
#include "src/database-server/data-model/varvalue.h"

#include <utility>

VarValue::VarValue(const Variable &variable, double val, std::uint64_t ts)
    : variable_{variable}, val_{val}, timestamp_{ts} {}

VarValue::VarValue(Variable &&variable, double &&val, std::uint64_t &&ts)
    : variable_{variable}, val_{val}, timestamp_{ts} {}

const Variable &VarValue::variable() const noexcept { return variable_; }

void VarValue::set_vaiable(Variable &&var) noexcept {
  variable_ = std::move(var);
}

double VarValue::val() const noexcept { return val_; }

void VarValue::set_val(double val) noexcept { val_ = val; }

std::uint64_t VarValue::timestamp() const noexcept { return timestamp_; }

void VarValue::set_timestamp(std::uint64_t ts) noexcept { timestamp_ = ts; }

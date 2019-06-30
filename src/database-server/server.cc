/*! \brief This file have the implementation for server.
    \file server.c
    \author Alvaro Denis <denisacostaq@gmail.com>
    \date 6/28/2019

    \copyright
    \attention <h1><center><strong>COPYRIGHT &copy; 2019 </strong>
    [<strong>denisacostaq</strong>][denisacostaq-URL].
    All rights reserved.</center></h1>
    \attention This file is part of
   [<strong>APIRestGenerator</strong>][APIRestGenerator-URL].

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
    [APIRestGenerator-URL]: http://github.com/denisacostaq/apirestgenerator
   "APIRestGenerator based on golang revel and gorm"
 */

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <boost/asio.hpp>

#include <messages.pb.h>

#include "src/database-server/data-access/dataaccess.h"
#include "src/database-server/data-model/sqlitewrapper.h"
#include "src/database-server/session.h"

namespace ba = boost::asio;
using ba::ip::tcp;

class server {
 public:
  server(ba::io_context& io_context, std::uint16_t port,
         const std::string& db_file)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        dm_{new SQLiteWrapper(db_file)},
        da_{new DataAccess(dm_)} {
    if (dm_->create_scheme() == IDataModel::Err::Ok) {
      da_->add_variable("temp");
    } else {
      throw std::string{"Unable to create schema."};
    }
    do_accept();
  }

  ~server() {
    delete da_;
    delete dm_;
  }

 private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<Session>(std::move(socket), da_)->start();
          } else {
            std::cerr << "acepting " << ec.message() << "\n";
          }
          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  IDataModel* dm_;
  IDataAccess* da_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }
    ba::io_context io_context;
    server s(io_context, std::atoi(argv[1]), "/tmp/sql.db");
    io_context.run();
  } catch (const std::string& e) {
    std::cerr << "Exception: " << e << "\n";
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  } catch (...) {
    std::cerr << "Unknow exception\n";
  }
  return 0;
}

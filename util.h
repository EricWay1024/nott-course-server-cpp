//
// Created by eric on 13/06/22.
//

#ifndef NOTTCOURSESERVER_UTIL_H
#define NOTTCOURSESERVER_UTIL_H

#include <SQLiteCpp/SQLiteCpp.h>
#include "crow.h"

namespace util {
    crow::json::wvalue select(SQLite::Statement&, char);
    crow::json::wvalue selectOne(SQLite::Statement&, char);
} // util

#endif //NOTTCOURSESERVER_UTIL_H

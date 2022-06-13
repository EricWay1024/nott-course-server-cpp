//
// Created by eric on 13/06/22.
//

#include "util.h"

namespace util {
    crow::json::wvalue select(SQLite::Statement& query) {
        std::vector<crow::json::wvalue> rows = {};
        int columnNum = query.getColumnCount();
        while (query.executeStep()) {
            auto row = crow::json::wvalue();
            for (int i = 0; i < columnNum; i++) {
                row[query.getColumnName(i)] = query.getColumn(i).getString();
            }
            rows.push_back(row);
        }
        return rows;
    }

    crow::json::wvalue selectOne(SQLite::Statement& query) {
        int columnNum = query.getColumnCount();
        auto row = crow::json::wvalue();
        if (query.executeStep()) {
            for (int i = 0; i < columnNum; i++) {
                row[query.getColumnName(i)] = query.getColumn(i).getString();
            }
        }
        return row;
    }
} // util
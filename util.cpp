//
// Created by eric on 13/06/22.
//

#include "util.h"

namespace util {
    std::vector<std::string> getObjCols(char type) {
        static std::vector<std::string> courseObjCols = {
                "requisites",
                "corequisites",
                "additionalRequirements",
                "class",
                "assessment",
                "belongsTo",
        };
        static std::vector<std::string> planObjCols = {
                "modules",
        };
        if (type == 'c') return courseObjCols;
        else return planObjCols;
    }


    crow::json::wvalue select(SQLite::Statement& query, char type) {
        std::vector<crow::json::wvalue> rows = {};
        int columnNum = query.getColumnCount();
        std::vector<std::string> objCols = getObjCols(type);
        while (query.executeStep()) {
            auto row = crow::json::wvalue();
            for (int i = 0; i < columnNum; i++) {
                std::string columnName = query.getColumnName(i);
                if (std::find(objCols.begin(), objCols.end(), columnName) != objCols.end()) {
                    row[columnName] = crow::json::load(query.getColumn(i).getString());
                }
                else {
                    row[columnName] = query.getColumn(i).getString();
                }
            }
            rows.push_back(row);
        }
        return rows;
    }

    crow::json::wvalue selectOne(SQLite::Statement& query, char type) {
        int columnNum = query.getColumnCount();
        auto row = crow::json::wvalue();
        std::vector<std::string> objCols = getObjCols(type);
        if (query.executeStep()) {
            for (int i = 0; i < columnNum; i++) {
                std::string columnName = query.getColumnName(i);
                if (std::find(objCols.begin(), objCols.end(), columnName) != objCols.end()) {
                    row[columnName] = crow::json::load(query.getColumn(i).getString());
                }
                else {
                    row[columnName] = query.getColumn(i).getString();
                }
            }
        }
        return row;
    }
} // util
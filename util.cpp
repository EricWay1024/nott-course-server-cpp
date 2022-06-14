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


    void convertObject(crow::json::wvalue& object, char type) {
        const std::vector<std::string> courseObjCols = {
            "convenor",
            "requisites",
            "additionalRequirements",
            "courseWebLinks",
            "class",
            "assessment",
            "belongsTo",
        };
        const std::vector<std::string> planObjCols = {
            "school",
            "planAccreditation",
            "subjectBenchmark",
            "modules",
            "courseWeightings",
            "degreeCalculationModel",
        };
        std::vector<std::string> columns = type == 'c' ? courseObjCols : planObjCols;
        auto resRead = crow::json::load(object.dump());
        for (auto &column: columns) {
            if (object[column].t() != crow::json::type::Null) {
                object[column] = crow::json::load(resRead[column].s());
            }
        }
    }
} // util
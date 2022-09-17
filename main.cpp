#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <SQLiteCpp/SQLiteCpp.h>
#include "crow.h"
#include "crow/middlewares/cors.h"
#include "util.h"

int main () {
    SQLite::Database db("data.db", SQLite::OPEN_READONLY);
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global();

    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    CROW_ROUTE(app, "/api/course")([&db](const crow::request& req){
        crow::json::wvalue res;
        if (req.url_params.get("code") == nullptr) {
            res["error"] = "No course code was provided.";
        }

        else {
            std::string code = req.url_params.get("code");
            SQLite::Statement query(db, "SELECT * FROM course WHERE code = ?");
            query.bind(1, code);
            res = util::selectOne(query, 'c');
            if (res.t() == crow::json::type::Null) {
                res["error"] = "No course found.";
            }
        }
        return res;
    });

    CROW_ROUTE(app, "/api/courses").methods("POST"_method)([&db](const crow::request& req){
        auto reqBody = crow::json::load(req.body);
        crow::json::wvalue res;
        if (reqBody.has("codes")) {
            auto codes = reqBody["codes"];
            auto codeList = codes.lo();
            std::stringstream os;
            for (size_t i = 0; i < codeList.size(); i++) {
                if (i > 0) {
                    os << " UNION ";
                }
                os <<  "SELECT code, title, level, offering, credits, semester, assessment "
                       "FROM course WHERE code = ?";
            }
            SQLite::Statement query(db, os.str());
            for (size_t i = 0; i < codeList.size(); i++) {
                query.bind(i + 1, codeList[i].s());
            }
            res = util::select(query, 'c');
        }
        return crow::json::wvalue(res);
    });

    CROW_ROUTE(app, "/api/plan")([&db](const crow::request& req){
        crow::json::wvalue res;
        if (req.url_params.get("code") == nullptr) {
            SQLite::Statement query(db, "SELECT degreeType, title, academicPlanCode, ucasCode "
                                        "FROM plan ORDER BY title");
            res = util::select(query, 'p');
        }

        else {
            std::string code = req.url_params.get("code");
            SQLite::Statement query(db, "SELECT * FROM plan WHERE academicPlanCode = ?");
            query.bind(1, code);
            res = util::selectOne(query, 'p');
            if (res.t() == crow::json::type::Null) {
                res["error"] = "No plan found.";
            }
        }
        return res;
    });

    CROW_ROUTE(app, "/api/query/plan").methods("POST"_method)([&db](const crow::request& req){
        crow::json::wvalue res;
        auto reqBody = crow::json::load(req.body);

        if (!reqBody) {
            res["error"] = "No query was provided.";
        }

        std::ostringstream os;
        os << "SELECT degreeType, title, academicPlanCode, ucasCode FROM plan WHERE ";

        if (reqBody.has("code")) {
            os << "academicPlanCode LIKE ? OR ucasCode LIKE ? ";
            SQLite::Statement query(db, os.str());
            for (int i = 1; i <= 2; i++) {
                query.bind(i, "%" + (std::string)reqBody["code"].s() + "%");
            }
            res = util::select(query, 'p');
        }

        else if (reqBody.has("title") && reqBody.has("degreeType")) {
            os << "title LIKE ? AND degreeType = ? ";
            os << "ORDER BY title";
            SQLite::Statement query(db, os.str());
            query.bind(1, "%" + (std::string)reqBody["title"].s() + "%");
            query.bind(2, reqBody["degreeType"].s());
            res = util::select(query, 'p');
        }

        else if (reqBody.has("keyword")) {
            os << "academicPlanCode LIKE ? OR ucasCode LIKE ? OR title LIKE ? ";
            os << "ORDER BY title";
            SQLite::Statement query(db, os.str());
            for (int i = 1; i <= 3; i++) {
                query.bind(i, "%" + (std::string)reqBody["keyword"].s() + "%");
            }
            res = util::select(query, 'p');
        }

        else {
            res["error"] = "Wrong parameters. Please provide code or (title and degreeType).";
        }

        return res;
    });


    CROW_ROUTE(app, "/api/query/course").methods("POST"_method)([&db](const crow::request& req){
        crow::json::wvalue res;
        auto reqBody = crow::json::load(req.body);

        if (!reqBody) {
            res["error"] = "No query was provided.";
        }

        std::ostringstream os;
        os << "SELECT code, title, level, offering, credits, semester FROM course WHERE ";

        if (reqBody.has("code")) {
            os << "code LIKE ? ";
            SQLite::Statement query(db, os.str());
            query.bind(1, "%" + (std::string)reqBody["code"].s() + "%");
            res = util::select(query, 'c');
        }

        else if (reqBody.has("title")) {
            os << "title LIKE ? ";
            os << "ORDER BY code";
            SQLite::Statement query(db, os.str());
            query.bind(1, "%" + (std::string)reqBody["title"].s() + "%");
            res = util::select(query, 'c');
        }

        else if (reqBody.has("credits") && reqBody.has("level") && reqBody.has("offering") && reqBody.has("semester")) {
            bool first = true;
            std::vector<std::string> columns = {"credits", "level", "offering", "semester"};

            for (auto& column: columns) {
                size_t s = reqBody[column].size();
                for (size_t i = 0; i < s; i++) {
                    if (i == 0) {
                        if (!first) os << " AND ";
                        os << " ( ";
                    }
                    else { os << " OR "; }
                    os << column << " = ? ";
                    if (i == s - 1) { os << " ) "; }
                }
                first &= (s == 0);
            }

            os << " ORDER BY code";
            SQLite::Statement query(db, os.str());

            int cnt = 1;
            for (auto &column: columns) {
                auto values = reqBody[column];
                auto valuesVec = values.lo();
                for (auto &u: valuesVec) query.bind(cnt++, u.s());
            }

            res = util::select(query, 'c');
        }

        else {
            res["error"] = "Wrong parameters. "
                           "Please provide: code or title or (credits and level and offering and semester).";
        }

        return res;
    });

    app.port(18080).multithreaded().run();

    return EXIT_SUCCESS;
}

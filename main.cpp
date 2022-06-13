#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <SQLiteCpp/SQLiteCpp.h>
#include "crow.h"
#include "crow/middlewares/cors.h"
#include "util.h"


#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER
namespace SQLite
{
/// definition of the assertion handler enabled when SQLITECPP_ENABLE_ASSERT_HANDLER is defined in the project (CMakeList.txt)
void assertion_failed(const char* apFile, const long apLine, const char* apFunc, const char* apExpr, const char* apMsg)
{
    // Print a message to the standard error output stream, and abort the program.
    std::cerr << apFile << ":" << apLine << ":" << " error: assertion failed (" << apExpr << ") in " << apFunc << "() with message \"" << apMsg << "\"\n";
    std::abort();
}
}
#endif

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
            res = util::selectOne(query);
            if (res.t() == crow::json::type::Null) {
                res["error"] = "No course found.";
            }
        }
        return res;
    });

    CROW_ROUTE(app, "/api/plan")([&db](const crow::request& req){
        crow::json::wvalue res;
        if (req.url_params.get("code") == nullptr) {
            SQLite::Statement query(db, "SELECT title, academicPlanCode, ucasCode FROM plan");
            res = util::select(query);
//            res["error"] = "No plan code was provided.";
        }

        else {
            std::string code = req.url_params.get("code");
            SQLite::Statement query(db, "SELECT * FROM plan WHERE academicPlanCode = ?");
            query.bind(1, code);
            res = util::selectOne(query);
            if (res.t() == crow::json::type::Null) {
                res["error"] = "No plan found.";
            }
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
            os << "code = ?";
            SQLite::Statement query(db, os.str());
            query.bind(1, reqBody["code"].s());
            res = util::select(query);
        }

        else if (reqBody.has("title")) {
            os << "title LIKE ?";
            os << " ORDER BY code";
            SQLite::Statement query(db, os.str());
            query.bind(1, "%" + (std::string)reqBody["title"].s() + "%");
            res = util::select(query);
        }

        else if (reqBody.has("credits") && reqBody.has("level") && reqBody.has("offering") && reqBody.has("semester")) {
            bool first = true;
            std::vector<std::string> columns = {"credits", "level", "offering", "semester"};

            for (auto& column: columns) {
                auto s = reqBody[column].size();
                for (size_t i = 0; i < s; i++) {
                    if (i == 0) {
                        if (!first) os << " AND ";
                        os << "(" << column << " = ? ";
                    }
                    else { os << " OR " << column << " = ?"; }
                    if (i == s - 1) { os << ")"; }
                }
                first &= (s == 0);
            }

            os << " ORDER BY code";
            std::string osstr = os.str();
            std::cout << osstr << "\n";
            SQLite::Statement query(db, osstr);

            int cnt = 1;
            for (auto &column: columns) {
                auto values = reqBody[column];
                auto valuesVec = values.lo();
                for (auto &u: valuesVec) query.bind(cnt++, u.s());
            }

            res = util::select(query);
        }

        else {
            res["error"] = "Wrong parameters. "
                           "Please provide: code or title or (credits and level and offering and semester).";
        }

        return res;
    });

    app.port(18080).multithreaded().run();

//    try
//    {
//        // Open a database file in read only mode
//        std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";
//
//        // Check the results : expect two row of result
//        auto res = util::selectOne(db, "SELECT credits, level FROM course LIMIT 3");
//        std::cout << res.dump() << "\n";
//    }
//    catch (std::exception& e)
//    {
//        std::cout << "SQLite exception: " << e.what() << std::endl;
//        return EXIT_FAILURE; // unexpected error : exit the example program
//    }
//
//    std::cout << "everything ok, quitting\n";

    return EXIT_SUCCESS;
}

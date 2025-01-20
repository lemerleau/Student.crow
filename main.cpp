//
//  main.cpp
//  student.crow
//
//  Created by Nono Saha Cyrille Merleau on 17.01.25.
//

#include <iostream>
#include <crow.h>
#include <regex>
#include <sqlite3.h>
#include <boost/lexical_cast.hpp>
using namespace std;

class Student {

public:
    int studentID;
    std::string firstName;
    std::string lastName;

public: Student(){} ;
    
// Defining our constructor
public: Student(string firstName, string lastName) {     // Constructor
    //this->studentID = studentID ;
    this->firstName = firstName;
    this->lastName = lastName;
  }
    
    void print_student(){
        std::cout <<"Student("<<this->studentID<<","<<this->firstName<<","<<this->lastName<<")\n";
    }

};

// We initialise our database by creating the tables
void createTables(sqlite3* db) {
    char* errorMessage;

    const char* createStudentTableSQL = "CREATE TABLE IF NOT EXISTS student ("
       "id INTEGER PRIMARY KEY AUTO_INCREMENT,"
       "firstname TEXT NULL,"
       "lastname TEXT NOT NULL);";

   if (sqlite3_exec(db, createStudentTableSQL, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
       cerr << "Error creating Student table: " << errorMessage << endl;
       sqlite3_free(errorMessage);
       return;
   }
}

void register_student(Student& student) {
    sqlite3* db;
    sqlite3_open("student.db", &db);

    sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr);
    string sql = "INSERT INTO student (firstname, lastname) VALUES ('" + student.firstName + "', '" + student.lastName + "');";

    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        cerr << "Error registering Student: " << sqlite3_errmsg(db) << endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    }
    else {
        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    }

    sqlite3_close(db);
}

crow::json::wvalue get_students() {
    sqlite3* db;
    sqlite3_stmt* stmt;

    sqlite3_open("student.db", &db);

    string sql = "SELECT * FROM student ORDER by id;"; // Note that here, we hardcode our SQL
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    crow::json::wvalue students;
    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int student_id = sqlite3_column_int(stmt, 0);
        string firstname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string lastname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        students[index]["student_id"] = student_id;
        students[index]["firstname"] = firstname;
        students[index]["lastname"] = lastname;
        
        index++;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return students;
}

int main(int argc, const char * argv[]) {
    
    sqlite3* db;
    if (sqlite3_open("student.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    createTables(db);

    sqlite3_close(db);
    
    
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "<h1> Welcome to the Student registration System.... </h1>";
    });
    
    CROW_ROUTE(app, "/register_student").methods("POST"_method)([](const crow::request& req) {
        auto data = crow::json::load(req.body);
        if (!data || !data.has("firstName") || !data.has("lastName")) {
            return crow::response(400, "Invalid data");
        }

        //Get the data from the request's body
        string firstname = data["firstName"].s();
        string lastname = data["lastName"].s();
        int id = 3;

        // Note that you can define an isValidName function and uncomment this for validation
        //if (!isValidName(firstname)) {
        //    return crow::response(400, "Invalid name");
        //}

        Student student(firstname, lastname);

        register_student(student);
        
        crow::json::wvalue response_data;
        response_data["message"] = "Student Successfully Registered";

        return crow::response(200, response_data);
        });

    CROW_ROUTE(app, "/students").methods("GET"_method)([](const crow::request& req) {
        return crow::response(200, get_students().dump());
        });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
    
    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "data/work_with_db.hpp"

int main()
{
    try
    {
        std::vector<std::string> fields = {"ID int NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                           "NAME varchar(30)",
                                           "SURNAME varchar(30)"
                                          };
        std::map<std::string, std::vector<std::string>> table;
        Data data("localhost", "worker", "12Err##tur#-3425TTu", "test_data");

        data.createTable("People", fields);
        data.createTable("Humans", fields);
        data.createTable("Ages", "ID int NOT NULL PRIMARY KEY AUTO_INCREMENT, OLD int");

        table.emplace("Humans", fields);

        data.createTables(table);

        std::map<std::string, bool> drop;

        drop.emplace("People", false);
        drop.emplace("Humans", false);
        drop.emplace("Ages", false);

        data.dropTable("People");
        data.dropTable(std::string("Numbers"));
        data.dropTables(drop);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
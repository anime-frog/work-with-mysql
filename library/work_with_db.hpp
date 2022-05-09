#ifndef WORK_WITH_DB_HPP
#define WORK_WITH_DB_HPP

#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <mysql/mysql.h>

class Data
{
protected:
    MYSQL mysql;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL_FIELD *field = nullptr;

public:
    Data();
    Data(std::string host, std::string username, std::string password, std::string dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);
    Data(const char *host, const char *username, const char *password, const char *dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);
    ~Data();

    void connectToDatabase(std::string host, std::string username, std::string password, std::string dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);
    void connectToDatabase(const char *host, const char *username, const char *password, const char *dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);

    void selectDatabase(std::string dataname);
    void selectDatabase(const char *dataname);

    void insertValues(std::string table, std::string field, std::string value);
    void insertValues(const char *table, const char *field, const char *value);

    template <class T>
    void insertValues(std::string table, std::string field, T values);

    template <class T>
    void insertValues(const char *table, const char *field, T values);

    std::string getData(std::string table, std::string fields = "*", std::string specialKeys = "");
    const char *getData(const char *table, const char *fields = "*", const char *specialKeys = "");

    template <class T1, class T2 = std::vector<std::string>>
    T1 getData(T1 tables, T2 fields = {}, bool getColumns = false, const char *specialKeys = "");

    void createTable(std::string table, std::string fields /* useKeys*/);
    void createTable(const char *table, const char *fields /* useKeys*/);

    template <class T>
    void createTable(std::string table, T fields);

    template <class T>
    void createTable(const char* table, T fields);

    template <class MAP>
    void createTables(MAP tablesAndFields);

    void dropTable(std::string table, bool withKeys = false);
    void dropTable(const char *table, bool withKeys = false);

    template <class T>
    void dropTables(T tables, bool withKeys);

    template <class MAP>
    void dropTables(MAP tablesAndFlags);
};

Data::Data()
{
    if (!mysql_init(&mysql))
        throw std::invalid_argument(mysql_error(&mysql));
}

Data::Data(std::string host, std::string username, std::string password, std::string dataname, int port, const char *usocks, unsigned long int flags)
{
    if (!mysql_init(&mysql))
        throw std::invalid_argument(mysql_error(&mysql));

    if (!mysql_real_connect(&mysql, host.c_str(), username.c_str(), password.c_str(), dataname.c_str(), port, usocks, flags))
        throw std::invalid_argument(mysql_error(&mysql));
}

Data::Data(const char *host, const char *username, const char *password, const char *dataname, int port, const char *usocks, unsigned long int flags)
{
    if (!mysql_init(&mysql))
        throw std::invalid_argument(mysql_error(&mysql));

    if (!mysql_real_connect(&mysql, host, username, password, dataname, port, usocks, flags))
        throw std::invalid_argument(mysql_error(&mysql));
}

Data::~Data()
{
    if (res)
        mysql_free_result(res);

    res = nullptr;

    mysql_close(&mysql);
}

void Data::connectToDatabase(std::string host, std::string username, std::string password, std::string dataname, int port, const char *usocks, unsigned long int flags)
{
    if (!mysql_real_connect(&mysql, host.c_str(), username.c_str(), password.c_str(), dataname.c_str(), port, usocks, flags))
        throw std::invalid_argument(mysql_error(&mysql));
}

void Data::connectToDatabase(const char *host, const char *username, const char *password, const char *dataname, int port, const char *usocks, unsigned long int flags)
{
    if (!mysql_real_connect(&mysql, host, username, password, dataname, port, usocks, flags))
        throw std::invalid_argument(mysql_error(&mysql));
}

void Data::selectDatabase(std::string dataname)
{
    if (!mysql_select_db(&mysql, dataname.c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

void Data::selectDatabase(const char *dataname)
{
    if (!mysql_select_db(&mysql, dataname))
        throw std::invalid_argument(mysql_error(&mysql));
}

void Data::insertValues(std::string table, std::string field, std::string value)
{
    std::stringstream stream;

    stream << "INSERT INTO " << table << "(" << field << ") VALUES (" << value << ")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

void Data::insertValues(const char *table, const char *field, const char *value)
{
    std::stringstream stream;

    stream << "INSERT INTO " << table << "(" << field << ") VALUES (\"" << value << "\")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class T>
void Data::insertValues(std::string table, std::string field, T values)
{
    std::stringstream stream;

    for (auto v = std::begin(values); v != std::end(values); v++)
    {

        stream << "INSERT INTO " << table << "(" << field << ") VALUES (\"" << *v << "\")";

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class T>
void Data::insertValues(const char *table, const char *field, T values)
{
    for (auto v = std::begin(values); v != std::end(values); v++)
    {
        std::stringstream stream;

        stream << "INSERT INTO " << table << "(" << field << ") VALUES (\"" << *v << "\")";

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));
    }
}

std::string Data::getData(std::string table, std::string fields, std::string specialKeys)
{
    std::stringstream stream;

    stream << "SELECT " << fields << " FROM " << table;

    if (specialKeys != "")
        stream << " " << specialKeys;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));

    stream.str("");

    if (!(res = mysql_store_result(&mysql)))
        throw std::invalid_argument(mysql_error(&mysql));

    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < mysql_num_fields(res); i++)
            stream << row[i] << '\t';

        stream << '\n';
    }

    mysql_free_result(res);

    res = nullptr;

    return stream.str();
}

const char *Data::getData(const char *table, const char *fields, const char *specialKeys)
{
    std::stringstream stream;

    stream << "SELECT " << fields << " FROM " << table;

    if (std::strcmp(specialKeys, ""))
        stream << ' ' << specialKeys;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));

    stream.str("");

    if (!(res = mysql_store_result(&mysql)))
        throw std::invalid_argument(mysql_error(&mysql));

    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < mysql_num_fields(res); i++)
            stream << row[i] << '\t';

        stream << '\n';
    }

    mysql_free_result(res);

    res = nullptr;

    return stream.str().c_str();
}

template<class T1, class T2>
T1 Data::getData(T1 tables, T2 fields, bool getColumns, const char *specialKeys)
{
    std::stringstream stream;
    std::vector<std::string> data;
    auto t = std::begin(tables);

    for (int i = 0; t != std::end(tables); i++, t++)
    {
        auto fv = std::begin(fields) + i;

        if (fields.size())
            stream << "SELECT " << *fv << " FROM " << *t;
        else
            stream << "SELECT * FROM " << *t;
            
        if (std::strcmp(specialKeys, ""))
            stream << ' ' << specialKeys;

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");

        if (!(res = mysql_store_result(&mysql)))
            throw std::invalid_argument(mysql_error(&mysql));

        if (getColumns)
        {
            while ((field = mysql_fetch_field(res)))
                    stream << field->name << '\t';

            stream << '\n';
        }

        while ((row = mysql_fetch_row(res)))
        {
            for (int j = 0; j < mysql_num_fields(res); j++)
                stream << row[j] << '\t';

            stream << '\n';
        }

        mysql_free_result(res);

        res = nullptr;
        stream << '\n';

        data.push_back(stream.str());

        stream.str("");
    }

    return data;
}

void Data::createTable(std::string table, std::string fields /* useKeys*/)
{
    std::stringstream stream;

    stream << "CREATE TABLE IF NOT EXISTS " << table << "(" << fields << ")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

void Data::createTable(const char *table, const char *fields /* useKeys*/)
{
    std::stringstream stream;

    stream << "CREATE TABLE IF NOT EXISTS " << table << "(" << fields << ")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class T>
void Data::createTable(std::string table, T fields)
{
    std::stringstream stream;

    stream << "CREATE TABLE IF NOT EXISTS " << table << "(";

    for (auto it = std::begin(fields); it != std::end(fields); ++it)
    {
        stream << *it;

        if (it + 1 == std::end(fields))
        {
            stream << ")";

            break;
        }

        stream << ", ";
    }

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class T>
void Data::createTable(const char *table, T fields)
{
    std::stringstream stream;

    stream << "CREATE TABLE IF NOT EXISTS " << table << "(";

    for (auto it = std::begin(fields); it != std::end(fields); ++it)
    {
        stream << *it;

        if (it + 1 == std::end(fields))
        {
            stream << ")";

            break;
        }

        stream << ", ";
    }

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class MAP>
void Data::createTables(MAP tablesAndFields)
{
    std::stringstream stream;

    for (auto itp : tablesAndFields)
    {

        stream << "CREATE TABLE IF NOT EXISTS " << itp.first << "(";

        for (auto it = std::begin(itp.second); it != std::end(itp.second); ++it)
        {
            stream << *it;

            if (it + 1 == std::end(itp.second))
            {
                stream << ")";

                break;
            }

            stream << ", ";
        }

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

void Data::dropTable(std::string table, bool withKeys)
{
    std::stringstream stream;

    if (withKeys)
        if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS=0"))
            throw std::invalid_argument(mysql_error(&mysql));

    stream << "DROP TABLE IF EXISTS " << table;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));

    if (withKeys)
        if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS=1"))
            throw std::invalid_argument(mysql_error(&mysql));
}

void Data::dropTable(const char *table, bool withKeys)
{
    std::stringstream stream;

    if (withKeys)
        if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS=0"))
            throw std::invalid_argument(mysql_error(&mysql));

    stream << "DROP TABLE IF EXISTS " << table;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));

    if (withKeys)
        if (mysql_query(&mysql, "SET FOREIGN_KEY IF EXISTS _CHECKS=1"))
            throw std::invalid_argument(mysql_error(&mysql));
}

template <class T>
void Data::dropTables(T tables, bool withKeys)
{
    std::stringstream stream;

    if (withKeys)
        if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS=0"))
            throw std::invalid_argument(mysql_error(&mysql));

    for (auto it : tables)
    {
        stream << "DROP TABLE IF EXISTS " << it;

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }

    if (withKeys)
        if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS=1"))
            throw std::invalid_argument(mysql_error(&mysql));
}

template <class MAP>
void Data::dropTables(MAP tablesAndFlags)
{
    std::stringstream stream;

    for (auto itp : tablesAndFlags)
    {
        if (itp.second)
            if (mysql_query(&mysql, "SET FOREIGN_KEY_CHECKS=0"))
                throw std::invalid_argument(mysql_error(&mysql));

        stream << "DROP TABLE IF EXISTS " << itp.first;

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");

        if (itp.second)
            if (mysql_query(&mysql, "SET FOREIGN_KEY IF EXISTS _CHECKS=1"))
                throw std::invalid_argument(mysql_error(&mysql));
    }
}

#endif

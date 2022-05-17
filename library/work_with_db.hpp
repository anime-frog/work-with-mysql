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

    std::string lastData = "";

    template <class Str = const char*>
    void getDataReq(Str table, Str fields = "*", Str specialKeys = "");

public:
    Data();
    Data(std::string host, std::string username, std::string password, std::string dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);
    Data(const char *host, const char *username, const char *password, const char *dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);
    ~Data();

    /* ************************** */

    void connectToDatabase(std::string host, std::string username, std::string password, std::string dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);
    void connectToDatabase(const char *host, const char *username, const char *password, const char *dataname, int port = 8000, const char *usocks = NULL, unsigned long int flags = 0);

    /* ************************** */

    void selectDatabase(std::string dataname);
    void selectDatabase(const char *dataname);

    /* ************************** */

    template <class Str>
    void insertValues(Str table, Str field, Str value);

    template <class Str, class V = std::vector<std::string>>
    void insertValues(Str table, Str field, V value);

    template <class Str, class F = std::vector<std::string>, class V = std::vector<std::string>>
    void insertValues(Str table, F field, V value);

    /* ************************** */

    template <class Str>
    void deleteValues(Str table);

    template <class Str>
    void deleteValues(Str table, Str field, Str value);

    template <class Str, class T>
    void deleteValues(Str table, Str field, T values);

    /* ************************** */

    template <class Str>
    void updateValues(Str table, Str field, Str beforeValue, Str afterValue);

    template <class Str>
    void updateValues(Str table, Str setField, Str whereField, Str beforeValue, Str afterValue);

    template <class Str, class Arr = std::vector<std::string>>
    void updateValues(Str table, Arr fields, Arr beforeValues, Arr afterValues);

    template <class Str, class Arr = std::vector<std::string>>
    void updateValues(Str table, Arr setFields, Arr whereFields, Arr beforeValues, Arr afterValues);

    /* ************************** */

    template <class Str = const char*>
    const char *getDataFromTable(Str table, Str fields = "*", Str specialKeys = "");

    template <class T1 = std::vector<std::string>, class T2 = std::vector<std::string>, class Str = const char*>
    T1 getDataFromTables(T1 tables, T2 fields = {}, bool getColumns = false, Str specialKeys = "");

    /* ************************** */

    template <class Str>
    void createTable(Str table, Str fields /* useKeys*/);

    template <class Str, class T>
    void createTable(Str table, T fields);

    template <class MAP>
    void createTables(MAP tablesAndFields);

    /* ************************** */

    template <class Str>
    void dropTable(Str table, bool withKeys = false);

    template <class T>
    void dropTables(T tables, bool withKeys);

    template <class MAP>
    void dropTables(MAP tablesAndFlags);

    /* ************************** */

    template <class Str>
    void addField(Str table, Str field, Str attributes);

    template <class Str, class Arr = std::vector<std::string>>
    void addFields(Str table, Arr fields, Arr attributes);

    /* ************************** */

    template <class Str>
    void dropField(Str table, Str field);

    template <class Str, class Arr = std::vector<std::string>>
    void dropFields(Str table, Arr fields);

    template <class Map>
    void dropFields(Map tableFields);

    /* ************************** */

    template <class Str>
    void modifyField(Str table, Str field, Str attributes);

    template <class Str, class Arr = std::vector<std::string>>
    void modifyField(Str table, Arr fields, Arr attributes);
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

template <class Str>
void Data::insertValues(Str table, Str field, Str value)
{
    std::stringstream stream;

    stream << "INSERT INTO " << table << "(" << field << ") VALUES (" << value << ")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class V>
void Data::insertValues(Str table, Str field, V value)
{
    std::stringstream stream;

    for (auto it : value)
    {
        stream << "INSERT INTO " << table << "(" << field << ") VALUES (\"" << it << "\")";

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class Str, class F, class V>
void Data::insertValues(Str table, F field, V value)
{
    std::stringstream stream;

    stream << "INSERT INTO " << table << "(";

    auto it1 = std::begin(field);

    while (it1 + 1 != std::end(field))
        stream << *it1++ << ", ";

    stream << *it1 << ") VALUES (";

    auto it2 = std::begin(value);

    while (it2 + 1 != std::end(value))
        stream << "\"" << *it2++ << "\", ";

    stream << "\"" << *it2 << "\")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class F = std::vector<const char *>, class V = std::vector<const char *>>
void insertValues(const char *table, F field, V value);

template <class Str>
void Data::deleteValues(Str table)
{
    std::stringstream stream;

    stream << "DELETE FROM " << table;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str>
void Data::deleteValues(Str table, Str field, Str value)
{
    std::stringstream stream;

    stream << "DELETE FROM " << table << " WHERE " << field << " = \"" << value << "\"";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class T>
void Data::deleteValues(Str table, Str field, T values)
{
    std::stringstream stream;

    for (auto v = std::begin(values); v != std::end(values); v++)
    {
        stream << "DELETE FROM " << table << " WHERE " << field << " = \"" << v << "\"";

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class Str>
void Data::updateValues(Str table, Str field, Str beforeValue, Str afterValue)
{
    std::stringstream stream;

    stream << "UPDATE " << table << " SET " << field << " = \"" << afterValue << "\" WHERE " << field << " = \"" << beforeValue << "\"";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str>
void Data::updateValues(Str table, Str setField, Str whereField, Str beforeValue, Str afterValue)
{
    std::stringstream stream;

    stream << "UPDATE " << table << " SET " << setField << " = \"" << afterValue << "\" WHERE " << whereField << " = \"" << beforeValue << "\"";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class Arr>
void Data::updateValues(Str table, Arr fields, Arr beforeValues, Arr afterValues)
{
    std::stringstream stream;

    auto itF = std::begin(fields);
    auto itBV = std::begin(beforeValues);
    auto itAV = std::begin(afterValues);

    while (itF != std::end(fields) && itBV != std::end(beforeValues) && itAV != std::end(afterValues))
    {
        stream << "UPDATE " << table << " SET " << *itF << " = \"" << *itAV++ << "\" WHERE " << *itF++ << " = \"" << *itBV++ << "\"";

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class Str, class Arr>
void Data::updateValues(Str table, Arr setFields, Arr whereFields, Arr beforeValues, Arr afterValues)
{
    std::stringstream stream;

    auto itSF = std::begin(setFields);
    auto itWF = std::begin(whereFields);
    auto itBV = std::begin(beforeValues);
    auto itAV = std::begin(afterValues);

    while (itSF != std::end(setFields) && itWF != std::end(whereFields) && itBV != std::end(beforeValues) && itAV != std::end(afterValues))
    {
        stream << "UPDATE " << table << " SET " << *itSF++ << " = \"" << *itAV++ << "\" WHERE " << *itWF++ << " = \"" << *itBV++ << "\"";

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class Str>
void Data::getDataReq(Str table, Str fields, Str specialKeys)
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

    lastData = stream.str();
}

template <class Str>
const char *Data::getDataFromTable(Str table, Str fields, Str specialKeys)
{
    getDataReq(table, fields, specialKeys);

    return lastData.c_str();
}

    template <class T1, class T2, class Str>
T1 Data::getDataFromTables(T1 tables, T2 fields, bool getColumns, Str specialKeys)
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

template <class Str>
void Data::createTable(Str table, Str fields /* useKeys*/)
{
    std::stringstream stream;

    stream << "CREATE TABLE IF NOT EXISTS " << table << "(" << fields << ")";

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class T>
void Data::createTable(Str table, T fields)
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
template <class Str>
void Data::dropTable(Str table, bool withKeys)
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

template <class Str>
void Data::addField(Str table, Str field, Str attributes)
{
    std::stringstream stream;

    stream << "ALTER TABLE " << table << " ADD " << field << ' ' << attributes;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class Arr>
void Data::addFields(Str table, Arr fields, Arr attributes)
{
    std::stringstream stream;

    auto itF = std::begin(fields);
    auto itA = std::begin(attributes);

    while (itF && itA)
    {
        stream << "ALTER TABLE " << table << " ADD " << *itF++ << ' ' << *itA++;

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class Str>
void Data::dropField(Str table, Str field)
{
    std::stringstream stream;

    stream << "ALTER TABLE " << table << " DROP COLUMN " << field;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class Arr>
void Data::dropFields(Str table, Arr fields)
{
    std::stringstream stream;

    for (auto it : fields)
    {
        stream << "ALTER TABLE " << table << " DROP COLUMN " << it;

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
}

template <class Map>
void Data::dropFields(Map tableFields)
{
    std::stringstream stream;

    for (auto mit : tableFields)
    {
        for (auto it : *mit->second)
        {
            stream << "ALTER TABLE " << *mit->first << " DROP COLUMN " << it;

            if (mysql_query(&mysql, stream.str().c_str()))
                throw std::invalid_argument(mysql_error(&mysql));

            stream.str("");
        }
    }
}

template <class Str>
void Data::modifyField(Str table, Str field, Str attributes)
{
    std::stringstream stream;

    stream << "ALTER TABLE " << table << " MODIFY " << field << ' ' << attributes;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class Str, class Arr>
void Data::modifyField(Str table, Arr fields, Arr attributes)
{
    std::stringstream stream;

    auto itF = std::begin(fields);
    auto itA = std::begin(attributes);

    while (itF && itA)
    {
        stream << "ALTER TABLE " << table << " MODIFY " << *itF << ' ' << *itA;

        if (mysql_query(&mysql, stream.str().c_str()))
            throw std::invalid_argument(mysql_error(&mysql));

        stream.str("");
    }
    
}

#endif

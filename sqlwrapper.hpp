#ifndef SQLWRAPPER_HPP
#define SQLWRAPPER_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <exception>
#include <mysql/mysql.h>

namespace
{
    MYSQL mysql;
    MYSQL_RES *res = nullptr;
    MYSQL_ROW row;
    MYSQL_FIELD *field = nullptr;

    void inputTypes(std::stringstream &stream, const char *rep1l, const char *repl2) {}

    template <class firstArg, class... Args>
    void inputTypes(std::stringstream &stream, const char *repl1, const char *repl2, firstArg fArg, Args... args)
    {
        stream << repl1 << fArg << repl1 << repl2;

        inputTypes(stream, repl1, repl2, args...);
    }

    bool checkTemplate()
    {
        return false;
    }

    template <class... Args>
    bool checkTemplate(Args... args)
    {
        return true;
    }
};

namespace SQLWrap
{
    namespace attrs
    {
        const char *primaryKey = "PRIMARY KEY";
        const char *unique = "UNIQUE";
        const char *autoIncrement = "AUTO_INCREMENT";
        const char *nullValue = "NULL";
        const char *notNullValue = "NOT NULL";
        const char *And = "AND";
        const char *Or = "OR";

        template <class checkType, class... checkList>
        std::string Check(checkType checkValue, checkList... args)
        {
            std::stringstream stream;

            stream << "CHECK (" << checkValue << ' ';

            inputTypes(stream, "", " ", args...);

            std::string result = stream.str();

            result[result.size() - 1] = ')';

            return result;
        }
    };

    namespace types
    {
        const char *boolean = "BOOL";
        const char *integer = "INT";
        const char *bigInt = "BIGINT";
        const char *smallInt = "SMALLINT";
        const char *tinyInt = "TINYINT";

        std::string floatType(size_t countIntegers, size_t precision)
        {
            return "FLOAT(" + std::to_string(countIntegers) + ',' + std::to_string(precision) + ')';
        }

        std::string doubleType(size_t countIntegers, size_t precision)
        {
            return "DOUBLE(" + std::to_string(countIntegers) + ',' + std::to_string(precision) + ')';
        }

        std::string decimal(size_t countIntegers, size_t precision)
        {
            return "DECIMAL(" + std::to_string(countIntegers) + ',' + std::to_string(precision) + ')';
        }

        std::string numeric(size_t countIntegers, size_t precision)
        {
            return "NUMERIC(" + std::to_string(countIntegers) + ',' + std::to_string(precision) + ')';
        }

        std::string charType(size_t size)
        {
            return "VARCHAR(" + std::to_string(size) + ')';
        }

        std::string varchar(size_t size)
        {
            return "CHAR(" + std::to_string(size) + ')';
        }

        const char *text = "TEXT";
        const char *mediumText = "MEDIUMTEXT";
        const char *longText = "LONGTEXT";
        const char *blob = "BLOB";
        const char *mediumBlob = "MEDIUMBLOB";
        const char *longBlob = "LONGBLOB";

        const char *date = "DATE";
        const char *time = "TIME";
        const char *datatime = "DATATIME";
        const char *timestamp = "TIMESTAMP";

        std::string year(size_t form)
        {
            return "YEAR(" + std::to_string(form) + ")";
        }
    };

    template <class fieldName, class valueType>
    std::string Pair(fieldName field, valueType value, std::string specSign = "=")
    {
        std::stringstream stream;

        stream << field << specSign << '\"' << value << '\"';

        return stream.str();
    }

    template <class pairValue, class... pairList>
    std::string Set(pairValue value, pairList... args)
    {
        std::stringstream stream;

        stream << value << ", ";

        inputTypes(stream, "", ", ", args...);

        std::string result = stream.str();

        result[result.size() - 2] = ' ';

        return result;
    }

    template <class fieldType, class... Args>
    struct Value
    {
        std::string result;

        Value(fieldType value, Args... args)
        {
            std::stringstream stream;

            stream << "(\"" << value << "\", ";

            inputTypes(stream, "\"", ", ", args...);

            result = stream.str();

            result[result.size() - 2] = ')';
        }
    };

    template <class fieldName, class fieldType, class... Args>
    struct Field
    {
        std::string result;
        fieldName name;
        fieldType type;

        Field(fieldName name, fieldType type, Args... args)
        {
            std::stringstream stream;

            stream << name << ' ' << type << ' ';

            inputTypes(stream, "", " ", args...);

            result = stream.str();

            result[result.size() - 1] = ',';
            this->name = name;
            this->type = type;
        }

        Field(fieldType value, Args... args)
        {
            std::stringstream stream;

            stream << '(' << value << ", ";

            inputTypes(stream, "", ", ", args...);

            result = stream.str();

            result[result.size() - 1] = ')';
        }
    };

    template <class fieldType, class... Args>
    struct FieldInput
    {
        std::string result;

        FieldInput(fieldType value, Args... args)
        {
            std::stringstream stream;

            stream << '(' << value << ", ";

            inputTypes(stream, "", ", ", args...);

            result = stream.str();

            result[result.size() - 2] = ')';
        }
    };

    template <class fieldName, class... Args>
    struct FieldOutput
    {
        std::string result;

        FieldOutput(fieldName value, Args... args)
        {
            std::stringstream stream;

            stream << "" << value << ", ";

            inputTypes(stream, "", ", ", args...);

            result = stream.str();

            result[result.size() - 2] = ' ';
        }
    };

    void connectToDatabase
    (
        const char *host,
        const char *username,
        const char *password,
        const char *dataname,
        int port = 8000,
        const char *usocks = NULL,
        unsigned long int flags = 0
    )
    {
        if (!mysql_init(&mysql))
            throw std::logic_error(mysql_error(&mysql));

        if (!mysql_real_connect(&mysql, host, username, password, dataname, port, usocks, flags))
            throw std::invalid_argument(mysql_error(&mysql));
    }

    class Table
    {
        std::string tableName;
        std::map<std::string, std::string> fieldsInTable;
        std::vector<std::string> values;
        std::vector<std::string> constraints;

        void createTable(std::stringstream &stream) {}

        template <class fieldName, class fieldType, class... Args, class... Fields>
        void createTable
        (
            std::stringstream &stream,
            Field<fieldName, fieldType, Args...> f,
            Fields... fields
        );

        void insertValues(std::stringstream &stream) {}

        template <class fieldType, class... Args, class... ValueList>
        void insertValues
        (
            std::stringstream &stream,
            Value<fieldType, Args...> v,
            ValueList... args
        );

        void deleteValues(std::stringstream &stream) {}

        template <class String, class... Args>
        void deleteValues
        (
            std::stringstream &stream,
            String condition,
            Args... args
        );

    public:
        template <class fieldName, class fieldType, class... Args, class... Fields>
        Table
        (
            std::string tableName,
            Field<fieldName, fieldType, Args...> f,
            Fields... fields
        );

        template <class fieldName, class... Args, class... conditionList>
        std::map<int, std::vector<std::string>> selectValues
        (
            FieldOutput<fieldName, Args...> f,
            conditionList... args
        );

        template <class fieldType, class... Args, class... ValueList>
        void insertValues
        (
            FieldInput<fieldType, Args...> f,
            ValueList... valueList
        );

        template <class String, class... Args>
        void deleteValues
        (
            String condition,
            Args... args
        );

        template <class forUpdate, class... conditionList>
        void updateValues
        (
            forUpdate values, 
            conditionList... args
        );

        void addConstraint(std::string nameConstraint, std::string arg);

        void dropTable();
    };
};

template <class fieldName, class fieldType, class... Args, class... Fields>
void SQLWrap::Table::createTable(
    std::stringstream &stream,
    Field<fieldName, fieldType, Args...> f,
    Fields... fields)
{
    stream << f.result << ' ';
    fieldsInTable.emplace(f.name, f.result);

    createTable(stream, fields...);
}

template <class fieldType, class... Args, class... ValueList>
void SQLWrap::Table::insertValues(
    std::stringstream &stream,
    Value<fieldType, Args...> v,
    ValueList... args)
{
    values.push_back(v.result);

    insertValues(stream, args...);
}

template <class String, class... Args>
void SQLWrap::Table::deleteValues(
    std::stringstream &stream,
    String condition,
    Args... args)
{
    stream << condition << ' ';

    deleteValues(stream, args...);
}

template <class forUpdate, class... conditionList>
void SQLWrap::Table::updateValues(
    forUpdate values,
    conditionList... args)
{
    std::stringstream stream;

    stream << "UPDATE " << tableName << " SET " << values << " WHERE ";

    inputTypes(stream, "", ", ", args...);

    std::string command = stream.str();

    command[command.size() - 2] = ' ';

    if (mysql_query(&mysql, command.c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class fieldName, class fieldType, class... Args, class... Fields>
SQLWrap::Table::Table(
    std::string tableName,
    Field<fieldName, fieldType, Args...> f,
    Fields... fields)
{
    std::stringstream stream;

    this->tableName = tableName;

    stream << "CREATE TABLE IF NOT EXISTS " << tableName << '(' << f.result << ' ';
    fieldsInTable.emplace(f.name, f.result);

    createTable(stream, fields...);

    std::string command = stream.str();

    command.resize(command.size() - 1);
    command[command.size() - 1] = ')';

    if (mysql_query(&mysql, command.c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

template <class fieldName, class... Args, class... conditionList>
std::map<int, std::vector<std::string>> SQLWrap::Table::selectValues(
    FieldOutput<fieldName, Args...> f,
    conditionList... args)
{
    std::stringstream stream;

    stream << "SELECT " << f.result << "FROM " << tableName;

    if (checkTemplate(args...))
    {
        stream << " WHERE ";

        inputTypes(stream, "", " ", args...);
    }

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));

    if (!(res = mysql_store_result(&mysql)))
        throw std::invalid_argument(mysql_error(&mysql));

    int j = 0;
    std::vector<std::string> temp;
    std::map<int, std::vector<std::string>> data;

    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < mysql_num_fields(res); i++)
            temp.push_back(row[i]);

        data.emplace(j++, temp);

        temp.clear();
    }

    mysql_free_result(res);

    res = nullptr;

    return data;
}

template <class fieldType, class... Args, class... ValueList>
void SQLWrap::Table::insertValues(
    FieldInput<fieldType, Args...> f,
    ValueList... valueList)
{
    std::stringstream stream;

    stream << f.result;

    insertValues(stream, valueList...);

    for (auto it : values)
    {
        std::string command = "INSERT INTO " + tableName + stream.str() + " VALUES " + it;

        if (mysql_query(&mysql, command.c_str()))
            throw std::invalid_argument(mysql_error(&mysql));
    }

    values.clear();
}

template <class String, class... Args>
void SQLWrap::Table::deleteValues(
    String condition,
    Args... args)
{
    std::stringstream stream;

    stream << "DELETE FROM " << tableName << " WHERE " << condition << ' ';

    deleteValues(stream, args...);

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

void SQLWrap::Table::addConstraint(std::string nameConstraint, std::string arg)
{
    std::stringstream stream;

    stream << "ALTER TABLE " << tableName << " ADD CONSTRAINT "
           << nameConstraint << ' ' << arg;

    if (mysql_query(&mysql, stream.str().c_str()))
        throw std::invalid_argument(mysql_error(&mysql));

    constraints.push_back(nameConstraint);
}

void SQLWrap::Table::dropTable()
{
    std::string command = "DROP TABLE " + tableName;

    if (mysql_query(&mysql, command.c_str()))
        throw std::invalid_argument(mysql_error(&mysql));
}

#endif // SQLWRAPPER_HPP

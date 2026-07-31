#include "database.hpp"
#include <string>

class Tests {
public:
    Tests();

    template <typename T>
    void ExpectEqual(const T &expected, const T &actual, const std::string &message = "", const bool printable = true);
    void TestDatabaseFunctions();
    void TestTokenizer();
    void TestSplitter();
    void TestAll();

private:
    Database db;
};
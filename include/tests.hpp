#include <string>

namespace Tests {
    template <typename T>
    static void ExpectEqual(const T &expected, const T &actual, const std::string &message = "", const bool printable = true);
    void TestDatabaseFunctions();
    void TestTokenizer();
    void TestAll();
};
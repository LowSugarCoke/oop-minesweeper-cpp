#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <exception>

struct TestCase {
    std::string name;
    std::function<void()> run;
};

class TestRegistry {
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }

    void registerTestCase(const std::string& name, std::function<void()> run) {
        testCases.push_back({name, run});
    }

    const std::vector<TestCase>& getTestCases() const {
        return testCases;
    }

private:
    std::vector<TestCase> testCases;
    TestRegistry() = default;
};

struct TestRegisterHelper {
    TestRegisterHelper(const std::string& name, std::function<void()> run) {
        TestRegistry::getInstance().registerTestCase(name, run);
    }
};

#define TEST_CONCAT_INNER(x, y) x##y
#define TEST_CONCAT(x, y) TEST_CONCAT_INNER(x, y)

#define TEST_CASE(name) \
    static void TEST_CONCAT(test_func_, __LINE__)(); \
    static TestRegisterHelper TEST_CONCAT(test_helper_, __LINE__)(name, TEST_CONCAT(test_func_, __LINE__)); \
    static void TEST_CONCAT(test_func_, __LINE__)()

class TestAssertionException : public std::exception {
private:
    std::string msg;
public:
    TestAssertionException(const std::string& msg) : msg(msg) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

inline void assert_true(bool condition, const std::string& message, const std::string& expr) {
    if (!condition) {
        throw TestAssertionException("Assertion failed: " + message + " (Expression: " + expr + ")");
    }
}

#define REQUIRE(cond) assert_true((cond), "Expected true", #cond)

template<typename T, typename U>
void assert_eq(const T& actual, const U& expected, const std::string& actualStr, const std::string& expectedStr) {
    if (actual != expected) {
        std::stringstream ss;
        ss << "Expected: " << expected << " (" << expectedStr << "), Actual: " << actual << " (" << actualStr << ")";
        throw TestAssertionException(ss.str());
    }
}

#define REQUIRE_EQ(actual, expected) assert_eq((actual), (expected), #actual, #expected)

#endif // TEST_FRAMEWORK_H

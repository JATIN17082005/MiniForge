#ifndef MINIFORGE_TESTS_TEST_FRAMEWORK_HPP
#define MINIFORGE_TESTS_TEST_FRAMEWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <cstddef>
#include <cstdint>

namespace miniforge::test {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry reg;
        return reg;
    }

    void addTest(const std::string& name, std::function<void()> func) {
        tests_.push_back({name, std::move(func)});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;

        std::cout << "========================================" << std::endl;
        std::cout << " Running " << tests_.size() << " test suites / cases" << std::endl;
        std::cout << "========================================" << std::endl;

        for (const auto& test : tests_) {
            currentTestName_ = test.name;
            currentTestFailed_ = false;
            std::cout << "[ RUN      ] " << test.name << std::endl;
            try {
                test.func();
            } catch (const std::exception& ex) {
                std::cout << "[ EXCEPTION] " << test.name << " threw: " << ex.what() << std::endl;
                currentTestFailed_ = true;
            } catch (...) {
                std::cout << "[ EXCEPTION] " << test.name << " threw unknown exception" << std::endl;
                currentTestFailed_ = true;
            }

            if (currentTestFailed_) {
                std::cout << "[  FAILED  ] " << test.name << std::endl;
                failed++;
            } else {
                std::cout << "[       OK ] " << test.name << std::endl;
                passed++;
            }
        }

        std::cout << "========================================" << std::endl;
        std::cout << " Test Summary: " << passed << " passed, " << failed << " failed out of " << tests_.size() << std::endl;
        std::cout << "========================================" << std::endl;

        return failed == 0 ? 0 : 1;
    }

    void markFailed() {
        currentTestFailed_ = true;
    }

    bool isCurrentTestFailed() const {
        return currentTestFailed_;
    }

private:
    std::vector<TestCase> tests_;
    std::string currentTestName_;
    bool currentTestFailed_{false};
};

struct TestRegistrar {
    TestRegistrar(const std::string& name, std::function<void()> func) {
        TestRegistry::instance().addTest(name, std::move(func));
    }
};

#define TEST_CASE(name) \
    static void name(); \
    static ::miniforge::test::TestRegistrar registrar_##name(#name, name); \
    static void name()

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "  Assertion failed: (" #cond ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
            ::miniforge::test::TestRegistry::instance().markFailed(); \
            return; \
        } \
    } while (0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(val1, val2) \
    do { \
        auto _v1 = (val1); \
        auto _v2 = (val2); \
        if (!(_v1 == _v2)) { \
            std::cerr << "  Assertion failed: (" #val1 " == " #val2 ") [" \
                      << _v1 << " != " << _v2 << "] at " << __FILE__ << ":" << __LINE__ << std::endl; \
            ::miniforge::test::TestRegistry::instance().markFailed(); \
            return; \
        } \
    } while (0)

#define ASSERT_NE(val1, val2) \
    do { \
        auto _v1 = (val1); \
        auto _v2 = (val2); \
        if (_v1 == _v2) { \
            std::cerr << "  Assertion failed: (" #val1 " != " #val2 ") [" \
                      << _v1 << " == " << _v2 << "] at " << __FILE__ << ":" << __LINE__ << std::endl; \
            ::miniforge::test::TestRegistry::instance().markFailed(); \
            return; \
        } \
    } while (0)

} // namespace miniforge::test

#endif // MINIFORGE_TESTS_TEST_FRAMEWORK_HPP


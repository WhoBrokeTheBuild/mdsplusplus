
#include <mdsplus.hpp>
using namespace mdsplus;

#include <gtest/gtest.h>

#define DEFINE_SCALAR_TEST(TYPE, EXPR, VALUE)                \
    TEST(NumericConversion, TYPE)                            \
    {                                                        \
        TYPE::ctype value = VALUE;                         \
                                                             \
        TYPE data(value);                                    \
                                                             \
        ASSERT_EQ(data.getData<Int8>().getValue(),   value); \
        ASSERT_EQ(data.getData<Int16>().getValue(),  value); \
        ASSERT_EQ(data.getData<Int32>().getValue(),  value); \
        ASSERT_EQ(data.getData<Int64>().getValue(),  value); \
        ASSERT_EQ(data.getData<UInt8>().getValue(),  value); \
        ASSERT_EQ(data.getData<UInt16>().getValue(), value); \
        ASSERT_EQ(data.getData<UInt32>().getValue(), value); \
        ASSERT_EQ(data.getData<UInt64>().getValue(), value); \
                                                             \
        ASSERT_EQ(data.decompile(), EXPR);                   \
    }

DEFINE_SCALAR_TEST(Int8,    "123B",  123);
DEFINE_SCALAR_TEST(Int16,   "123W",  123);
DEFINE_SCALAR_TEST(Int32,   "123",   123);
DEFINE_SCALAR_TEST(Int64,   "123Q",  123);
DEFINE_SCALAR_TEST(UInt8,   "123BU", 123);
DEFINE_SCALAR_TEST(UInt16,  "123WU", 123);
DEFINE_SCALAR_TEST(UInt32,  "123LU", 123);
DEFINE_SCALAR_TEST(UInt64,  "123QU", 123);
DEFINE_SCALAR_TEST(Float32, "123.",  123);
DEFINE_SCALAR_TEST(Float64, "123D0", 123);

template<typename T1, typename T2>
bool CompareVectors(const std::vector<T1>& vec1, const std::vector<T2>& vec2)
{
    if (vec1.size() != vec2.size()) {
        return false;
    }

    for (size_t i = 0; i < vec1.size(); ++i) {
        // TODO: Better way to compare across types?
        if (vec1[i] != T1(vec2[i]) || T2(vec1[i]) != vec2[i]) {
            return false;
        }
    }

    return true;
}

#define DEFINE_ARRAY_TEST(TYPE, EXPR, ...)                                           \
    TEST(NumericConversion, TYPE)                                                    \
    {                                                                                \
        std::vector<TYPE::ctype> value = { __VA_ARGS__ };                          \
                                                                                     \
        TYPE data(value);                                                            \
                                                                                     \
        ASSERT_TRUE(CompareVectors(data.getData<Int8Array>().getValues(), value));   \
        ASSERT_TRUE(CompareVectors(data.getData<Int16Array>().getValues(), value));  \
        ASSERT_TRUE(CompareVectors(data.getData<Int32Array>().getValues(), value));  \
        ASSERT_TRUE(CompareVectors(data.getData<Int64Array>().getValues(), value));  \
        ASSERT_TRUE(CompareVectors(data.getData<UInt8Array>().getValues(), value));  \
        ASSERT_TRUE(CompareVectors(data.getData<UInt16Array>().getValues(), value)); \
        ASSERT_TRUE(CompareVectors(data.getData<UInt32Array>().getValues(), value)); \
        ASSERT_TRUE(CompareVectors(data.getData<UInt64Array>().getValues(), value)); \
                                                                                     \
        ASSERT_EQ(data.decompile(), EXPR);                                           \
    }

DEFINE_ARRAY_TEST(Int8Array,    "Byte([1,2,3,123])",                  1, 2, 3, 123)
DEFINE_ARRAY_TEST(Int16Array,   "Word([1,2,3,123])",                  1, 2, 3, 123);
DEFINE_ARRAY_TEST(Int32Array,   "[1,2,3,123]",                        1, 2, 3, 123);
DEFINE_ARRAY_TEST(Int64Array,   "[1Q,2Q,3Q,123Q]",                    1, 2, 3, 123);
DEFINE_ARRAY_TEST(UInt8Array,   "Byte_Unsigned([1,2,3,123])",         1, 2, 3, 123);
DEFINE_ARRAY_TEST(UInt16Array,  "Word_Unsigned([1,2,3,123])",         1, 2, 3, 123);
DEFINE_ARRAY_TEST(UInt32Array,  "Long_Unsigned([1LU,2LU,3LU,123LU])", 1, 2, 3, 123);
DEFINE_ARRAY_TEST(UInt64Array,  "[1QU,2QU,3QU,123QU]",                1, 2, 3, 123);
DEFINE_ARRAY_TEST(Float32Array, "[1.,2.,3.,123.]",                    1, 2, 3, 123);
DEFINE_ARRAY_TEST(Float64Array, "[1D0,2D0,3D0,123D0]",                1, 2, 3, 123);

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
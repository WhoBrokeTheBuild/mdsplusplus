
#include <mdsplus.hpp>
using namespace mdsplus;

#include <gtest/gtest.h>

TEST(Local, Reconnect)
{
    Connection c("local");
    c.reconnect();
}

TEST(Local, Scalar)
{
    Connection c("local");
    
    ASSERT_EQ(c.get("123B"), Int8(123));
    ASSERT_EQ(c.get("123W"), Int16(123));
    ASSERT_EQ(c.get("123L"), Int32(123));
    ASSERT_EQ(c.get("123Q"), Int64(123));

    ASSERT_EQ(c.get("123BU"), UInt8(123));
    ASSERT_EQ(c.get("123WU"), UInt16(123));
    ASSERT_EQ(c.get("123LU"), UInt32(123));
    ASSERT_EQ(c.get("123QU"), UInt64(123));

    ASSERT_EQ(c.get("123.456F0"), Float32(123.456));
    ASSERT_EQ(c.get("123.456D0"), Float64(123.456));
}

TEST(Local, Array)
{
    Connection c("local");
    
    ASSERT_EQ(c.get("Byte([1, 2, 3])"), Int8Array({1, 2, 3}));
    ASSERT_EQ(c.get("Word([1, 2, 3])"), Int16Array({1, 2, 3}));
    ASSERT_EQ(c.get("Long([1, 2, 3])"), Int32Array({1, 2, 3}));
    ASSERT_EQ(c.get("Quadword([1, 2, 3])"), Int64Array({1, 2, 3}));

    ASSERT_EQ(c.get("Byte_Unsigned([1, 2, 3])"), UInt8Array({1, 2, 3}));
    ASSERT_EQ(c.get("Word_Unsigned([1, 2, 3])"), UInt16Array({1, 2, 3}));
    ASSERT_EQ(c.get("Long_Unsigned([1, 2, 3])"), UInt32Array({1, 2, 3}));
    ASSERT_EQ(c.get("Quadword_Unsigned([1, 2, 3])"), UInt64Array({1, 2, 3}));

    ASSERT_EQ(c.get("[1.2F0, 3.4F0, 5.6F0]"), Float32Array({1.2, 3.4, 5.6}));
    ASSERT_EQ(c.get("[1.2D0, 3.4D0, 5.6D0]"), Float64Array({1.2, 3.4, 5.6}));
}

TEST(Local, Object)
{
    Connection c("local");

    ASSERT_EQ(c.getObject("BUILD_PARAM('value', 'help', 'validation')"),
        Param("value", "help", "validation"));

    ASSERT_EQ(c.getObject("BUILD_SIGNAL('value', 'raw', 'dimension')"),
        Signal("value", "raw", "dimension"));

    ASSERT_EQ(c.getObject("BUILD_SIGNAL('value', 'raw', 'dim1', 'dim2', 'dim3')"),
        Signal("value", "raw", "dim1", "dim2", "dim3"));

    ASSERT_EQ(c.getObject("BUILD_DIM('window', 'axis')"),
        Dimension("window", "axis"));

    // TODO: Function

    // TODO: Fix
    // ASSERT_EQ(c.getObject("BUILD_RANGE('begin', 'end', 'delta')"),
    //     Range("begin", "end", "delta"));

    ASSERT_EQ(c.getObject("BUILD_WITH_UNITS('value', 'units')"),
        WithUnits("value", "units"));

    ASSERT_EQ(c.getObject("BUILD_WITH_ERROR('value', 'error')"),
        WithError("value", "error"));

    // Apparently you can't == an APD
    ASSERT_EQ(c.get("SerializeOut(LIST(*, 1, 'b', 0xC))"),
        List(1, "b", 0xC).serialize());

}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
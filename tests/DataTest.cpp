
#include <mdsplus.hpp>
using namespace mdsplus;

#include <gtest/gtest.h>

TEST(Data, Constructors)
{
    auto data = Int32(5552368);

    // This should not compile as the copy is deleted
    // auto dataCopy = data;

    auto dataClone = data.clone();

    ASSERT_EQ(data.getValue(), dataClone.getValue());
}

TEST(Data, Metadata)
{
    auto value = Int32(5552368);
    auto units = String("units");
    auto error = String("error");
    auto help = String("help");
    auto validation = String("validation");

    auto data = Param(WithError(WithUnits(value, units), error), help, validation);

    ASSERT_EQ(data.getData(), value);
    ASSERT_EQ(data.getUnits(), units);
    ASSERT_EQ(data.getError(), error);
    ASSERT_EQ(data.getHelp(), help);
    ASSERT_EQ(data.getValidation(), validation);

    ASSERT_EQ(data.getData<Int32>(), value);
    ASSERT_EQ(data.getUnits<String>(), units);
    ASSERT_EQ(data.getError<String>(), error);
    ASSERT_EQ(data.getHelp<String>(), help);
    ASSERT_EQ(data.getValidation<String>(), validation);

    // Could std::move(data)
    ASSERT_EQ(data.convert<Int32>(), value);
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
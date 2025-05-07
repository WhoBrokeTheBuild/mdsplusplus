
#include <mdsplus.hpp>
using namespace mdsplus;

#include <gtest/gtest.h>

TEST(Data, Constructors)
{
    // std::vector<Float32> raw;
    // raw.reserve(200);

    // for (int i = 0; i < 200; ++i) {
    //     raw.emplace_back(i);
    // }

    // Range ctr(0, Opcode::Missing, raw.size(), raw);

    // 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5
    Range range(0, 5, 0.5);
    
    Range range2(Int32(0), Int32(5), Float64(0.5));
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
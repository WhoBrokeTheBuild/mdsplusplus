
#include <mdsplus.hpp>
using namespace mdsplus;

#include <gtest/gtest.h>

#include <filesystem>

class TreeFixture : public ::testing::Test
{
protected:

    static inline const char * TREE_NAME = "mdspp";
    static inline const char * TREE_NAME_UPPER = "MDSPP";
    static inline const char * TREE_PATH_ENV = "mdspp_path";
    static inline int SHOT = 42;

    std::vector<std::string> _envList;
    std::vector<std::filesystem::path> _tempdirList;

    const std::string MakeTempDir()
    {
        char tempdir[1024];

        // e.g. /tmp/mdspp-XXXXXX
        snprintf(
            tempdir,
            sizeof(tempdir),
            "%s%cmdspp-XXXXXX",
            std::filesystem::temp_directory_path().c_str(),
            std::filesystem::path::preferred_separator
        );

        // e.g. /tmp/mdspp-123ABC
        mkdtemp(tempdir);

        _tempdirList.push_back(tempdir);
        return tempdir;
    }

    void SetUp() override
    {
        unsetenv("default_tree_path");

        auto tempdir = MakeTempDir();
        setenv(TREE_PATH_ENV, tempdir.c_str(), true);
        _envList.push_back(TREE_PATH_ENV);

        Tree tree(TREE_NAME, SHOT, Mode::New);

        tree.addNode("A", Usage::Any);
        tree.addNode("A:B", Usage::Numeric).putRecord(Int32(12345));
        tree.addNode("A:B:C", Usage::Text).putRecord(String("Hello, World!"));
        tree.addNode("A:B:C:D", Usage::Signal).putRecord(Signal(Int32Array({ 1, 2, 3 }), nullptr, Int32Array({ 0, 1, 2 })));

        // tree.addTag("Z", tree.getNode("A.B.C.D"));

        tree.addNode("SCALAR", Usage::Structure);
        tree.addNode("SCALAR:B", Usage::Numeric).putRecord(Int8(-42));
        tree.addNode("SCALAR:W", Usage::Numeric).putRecord(Int16(-42));
        tree.addNode("SCALAR:L", Usage::Numeric).putRecord(Int32(-42));
        tree.addNode("SCALAR:Q", Usage::Numeric).putRecord(Int64(-42));
        tree.addNode("SCALAR:BU", Usage::Numeric).putRecord(UInt8(42));
        tree.addNode("SCALAR:WU", Usage::Numeric).putRecord(UInt16(42));
        tree.addNode("SCALAR:LU", Usage::Numeric).putRecord(UInt32(42));
        tree.addNode("SCALAR:QU", Usage::Numeric).putRecord(UInt64(42));
        tree.addNode("SCALAR:FLOAT", Usage::Numeric).putRecord(Float32(4.2f));
        tree.addNode("SCALAR:DOUBLE", Usage::Numeric).putRecord(Float64(0.00042));

        tree.addNode("ARRAY", Usage::Structure);
        tree.addNode("ARRAY:B", Usage::Numeric).putRecord(Int8Array({ -3, -2, -1, 0, 1, 2, 3 }));
        tree.addNode("ARRAY:W", Usage::Numeric).putRecord(Int16Array({ -3, -2, -1, 0, 1, 2, 3 }));
        tree.addNode("ARRAY:L", Usage::Numeric).putRecord(Int32Array({ -3, -2, -1, 0, 1, 2, 3 }));
        tree.addNode("ARRAY:Q", Usage::Numeric).putRecord(Int64Array({ -3, -2, -1, 0, 1, 2, 3 }));
        tree.addNode("ARRAY:BU", Usage::Numeric).putRecord(UInt8Array({ 0, 1, 2, 3 }));
        tree.addNode("ARRAY:WU", Usage::Numeric).putRecord(UInt16Array({ 0, 1, 2, 3 }));
        tree.addNode("ARRAY:LU", Usage::Numeric).putRecord(UInt32Array({ 0, 1, 2, 3 }));
        tree.addNode("ARRAY:QU", Usage::Numeric).putRecord(UInt64Array({ 0, 1, 2, 3 }));
        tree.addNode("ARRAY:FLOAT", Usage::Numeric).putRecord(Float32Array({ -0.3f, -0.2f, -0.1f, 0.0f, 0.1f, 0.2f, 0.3f }));
        tree.addNode("ARRAY:DOUBLE", Usage::Numeric).putRecord(Float64Array({ -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3 }));

        tree.addNode("RECORD", Usage::Structure);
        tree.addNode("RECORD:ADD", Usage::Numeric).putRecord(Function(OPC_ADD, tree.getNode("A:B"), 42));

        tree.write();
    }

    void TearDown() override
    {
        for (const auto& tempdir : _tempdirList) {
            if (!tempdir.empty()) {
                std::filesystem::remove_all(tempdir);
            }
        }
        
        for (const auto& env : _envList) {
            unsetenv(env.c_str());
        }
    }
};

TEST(Tree, OpenFailure)
{
    const char * FAKE_TREE_NAME = "mdsppfake";
    const int SHOT = -1;

    unsetenv("default_tree_path");
    unsetenv("mdsppfake_path");

    // Open a nonexistent tree with no path defined
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::ReadOnly), TreeNoPath);
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::Normal), TreeNoPath);
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::Edit), TreeOpenReadWriteFailed);
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::New), TreeCreateFailed);
    
    // Open a nonexistent tree with a path defined
    setenv("mdsppfake_path", ".", true);
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::ReadOnly), TreeOpenReadOnlyFailed);
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::Normal), TreeOpenReadOnlyFailed);
    ASSERT_THROW(Tree(FAKE_TREE_NAME, SHOT, Mode::Edit), TreeOpenReadWriteFailed);
    // TODO: Mode::New with insufficient permissions
}

TEST_F(TreeFixture, Open)
{
    Tree tree(TREE_NAME, SHOT, Mode::ReadOnly);
    ASSERT_TRUE(tree.isOpen());
    ASSERT_EQ(tree.getTreeName(), TREE_NAME_UPPER);
    ASSERT_EQ(tree.getShot(), SHOT);

    tree.reopen();
    ASSERT_TRUE(tree.isOpen());
    ASSERT_EQ(tree.getTreeName(), TREE_NAME_UPPER);
    ASSERT_EQ(tree.getShot(), SHOT);
    
    tree.close();
    tree.open(TREE_NAME, SHOT, Mode::Normal);
    ASSERT_TRUE(tree.isOpen());
    ASSERT_EQ(tree.getTreeName(), TREE_NAME_UPPER);
    ASSERT_EQ(tree.getShot(), SHOT);
}

TEST_F(TreeFixture, GetNode)
{
    Tree tree(TREE_NAME, SHOT, Mode::ReadOnly);

    ASSERT_EQ(tree.getNode("A").getFullPath(), "\\MDSPP::TOP:A");

    // Test with both : and . separators
    ASSERT_EQ(tree.getNode("A:B").getFullPath(), "\\MDSPP::TOP:A:B");
    ASSERT_EQ(tree.getNode("A.B").getFullPath(), "\\MDSPP::TOP:A:B");
}

TEST_F(TreeFixture, GetScalar)
{
    Tree tree(TREE_NAME, SHOT, Mode::ReadOnly);

    ASSERT_EQ(tree.getNode("SCALAR:B").getData(), Int8(-42));
    ASSERT_EQ(tree.getNode("SCALAR:W").getData(), Int16(-42));
    ASSERT_EQ(tree.getNode("SCALAR:L").getData(), Int32(-42));
    ASSERT_EQ(tree.getNode("SCALAR:Q").getData(), Int64(-42));
    ASSERT_EQ(tree.getNode("SCALAR:BU").getData(), UInt8(42));
    ASSERT_EQ(tree.getNode("SCALAR:WU").getData(), UInt16(42));
    ASSERT_EQ(tree.getNode("SCALAR:LU").getData(), UInt32(42));
    ASSERT_EQ(tree.getNode("SCALAR:QU").getData(), UInt64(42));
    ASSERT_EQ(tree.getNode("SCALAR:FLOAT").getData(), Float32(4.2f));
    ASSERT_EQ(tree.getNode("SCALAR:DOUBLE").getData(), Float64(0.00042));
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
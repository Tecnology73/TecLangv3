#include <map>
#include "gtest/gtest.h"
#include "../compiler/TypeCoercion.h"

const std::map<std::string, TypeBase *> types = {
    {"bool", new TypeDefinition({}, "bool")},
    {"i8", new TypeDefinition({}, "i8")},
    {"i16", new TypeDefinition({}, "i16")},
    {"i32", new TypeDefinition({}, "i32")},
    {"i64", new TypeDefinition({}, "i64")},
    {"double", new TypeDefinition({}, "double")},
    {"Account", new TypeDefinition({}, "Account")},
};

int main(int argc, char** argv) {
    types.at("bool")->isValueType = true;
    types.at("i8")->isValueType = true;
    types.at("i16")->isValueType = true;
    types.at("i32")->isValueType = true;
    types.at("i64")->isValueType = true;
    types.at("double")->isValueType = true;

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(TeclangTest, Basic) {
    const std::map<std::pair<std::string, std::string>, bool> tests = {
        {{"bool", "bool"}, true},
        {{"bool", "i8"}, true},
        {{"bool", "i16"}, true},
        {{"bool", "i32"}, true},
        {{"bool", "i64"}, true},
        {{"bool", "double"}, true},
        {{"i8", "bool"}, false},
        {{"i8", "i8"}, true},
        {{"i8", "i16"}, true},
        {{"i8", "i32"}, true},
        {{"i8", "i64"}, true},
        {{"i8", "double"}, true},
        {{"i16", "bool"}, false},
        {{"i16", "i8"}, false},
        {{"i16", "i16"}, true},
        {{"i16", "i32"}, true},
        {{"i16", "i64"}, true},
        {{"i16", "double"}, true},
        {{"i32", "bool"}, false},
        {{"i32", "i8"}, false},
        {{"i32", "i16"}, false},
        {{"i32", "i32"}, true},
        {{"i32", "i64"}, true},
        {{"i32", "double"}, true},
        {{"i64", "bool"}, false},
        {{"i64", "i8"}, false},
        {{"i64", "i16"}, false},
        {{"i64", "i32"}, false},
        {{"i64", "i64"}, true},
        {{"i64", "double"}, true},
        {{"double", "bool"}, false},
        {{"double", "i8"}, false},
        {{"double", "i16"}, false},
        {{"double", "i32"}, false},
        {{"double", "i64"}, false},
        {{"double", "double"}, true},
        {{"Account", "bool"}, false},
        {{"Account", "i8"}, false},
        {{"Account", "i16"}, false},
        {{"Account", "i32"}, false},
        {{"Account", "i64"}, false},
        {{"Account", "double"}, false},
        {{"Account", "Account"}, true},
    };

    for (auto const& [key, value]: tests) {
        auto [type1, type2] = key;
        auto result = TypeCoercion::isTypeCompatible(types.at(type1), types.at(type2));

        EXPECT_EQ(result, value) << "TypeCoercion::isTypeCompatible(" << type1 << ", " << type2 << ") failed";
    }
}

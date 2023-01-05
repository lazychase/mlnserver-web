#pragma once

#include <cstdint>
#include <string>
#include <struct_mapping/struct_mapping.h>
#include <vector>
#include <unordered_map>

namespace mapStruct::table {

    BEGIN_STRUCT(MyData)

        MEMBER(int, ID)
        MEMBER(std::string, StrValue)
        MEMBER(bool, BooleanValue)
        MEMBER(std::vector<int>, ArrValue)

    END_STRUCT;

    using MyDataList = std::unordered_map<int, MyData>;

}//namespace mappingStruct::table {

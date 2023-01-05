#pragma once

#include <set>
#include <string>
#include <vector>

#include "myDesignTable/MyDataTable.h"

class TableManager
{
public:
    using firstGiveType = std::pair<int, int> ;
    using firstGiveContType = std::vector< firstGiveType >;

public:
    static TableManager& instance() { static TableManager _instance; return _instance; }
    bool loadTables(const char *tablePath = "myDesignTable/Json");


private:
    bool validate();

public:
    mapStruct::table::MyDataList _myDataList;
};

#include "tableManager.h"

#include <fmt/format.h>
#include <fstream>
#include <net/logger.hpp>
#include <nlohmann/json.hpp>
#include <strstream>

template< typename ValueType >
void loadTable(const std::string& filePath, std::unordered_map<int, ValueType>& outContainer)
{
    std::ifstream ifs(filePath.c_str(), std::ios_base::binary);
    if (!ifs) {
        throw std::runtime_error((std::string("Failed open file:") + filePath).c_str());
    }

    if (ifs.is_open()) {
        std::stringstream ss;
        ss << ifs.rdbuf();

        auto j = nlohmann::json::parse(ss.str());
        for (nlohmann::json::iterator it = j.begin(); j.end() != it; ++it) {

            std::string rowString = std::move(it->dump());
            //LOGD(rowString);

            auto& obj = *it;
            const int ID = obj["ID"];

            if (!ID) {continue;}

            std::istrstream is(rowString.c_str());

            ValueType v;
            struct_mapping::map_json_to_struct(v, is);

            outContainer.emplace(ID, v);
        }
    }

    LOGD("LoadingCompleted. tablePath:{}", filePath);
}



bool TableManager::loadTables(const char* tablePath /*= "myDesignTable/Json"*/)
{
	try {
        loadTable(std::string(tablePath) + "/MyDataTable.json", _myDataList);
	}
	catch (std::exception& e) {
		return false;
	}

    validate();

	return true;
}



bool TableManager::validate()
{
    return true;
}

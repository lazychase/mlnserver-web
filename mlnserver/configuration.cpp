#include "configuration.h"

#include <fstream>
#include <fmt/format.h>

mapStruct::Config& Configuration::load(const char* path /*= "netconfig.json"*/)
{
    std::ifstream ifs(path, std::ios_base::binary);
    if (!ifs) {
        throw std::runtime_error(fmt::format("Failed open file:{}", path));
    }

    struct_mapping::map_json_to_struct(_config, ifs);
    return _config;
}

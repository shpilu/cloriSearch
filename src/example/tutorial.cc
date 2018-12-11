//
// cloriSearch tutorial 
//

#include <fstream>
#include <iostream>
#include "jsoncpp/json_util.h"
#include "clorisearch.h"

using namespace cloris;

const char* g_index_schema = "{ \
    \"terms\":[{ \
        \"name\":\"city\", \
        \"key_type\":\"string\", \
        \"index_type\":\"simple\" \
    },{ \
        \"name\":\"age\", \
        \"key_type\":\"int32\", \
        \"index_type\":\"simple\" \
    },{ \
        \"name\":\"student\", \
        \"key_type\":\"bool\", \
        \"index_type\":\"simple\" \
    },{ \
        \"name\":\"gendor\", \
        \"key_type\":\"string\", \
        \"index_type\":\"simple\" \
    }] \
}";


int main() {
    CloriSearch *sch = new CloriSearch();
    if (!sch->Init(g_index_schema, IndexSchemaFormat::ISF_JSON, SourceType::DIRECT)) {
        std::cout << "cloriSearch init failed" << std::endl;
    } else {
        std::cout << "cloriSearch init success" << std::endl;
    }

    for (int i = 1; i < 10; ++i) {
        std::string key = std::string("../conf/simple_index_test/simple_index_") + std::to_string(i) + ".json";
        std::cout << key << std::endl;
            
        std::fstream in(key.c_str());
        std::istreambuf_iterator<char> begin(in); 
        std::istreambuf_iterator<char> end; 
        std::string dnf_str(begin, end);
        // std::cout << "raw_dnf=" << dnf_str << std::endl;

        if (sch->Add(dnf_str, IndexSchemaFormat::ISF_JSON, false)) {
            std::cout << "add index success" << std::endl;
        } else {
            std::cout << "add index failed, dnf_str=" << dnf_str << std::endl;
        }
    }

    Query query;
    query["city"] = "beijing";
    query["age"] = 20;
    query["gendor"] = "female";
    query["student"] = true;

    for (auto &p : query) {
        std::cout << p.name() << "=" << p.value() << std::endl;
    }

    std::vector<int> res = sch->Search(query, 10);
    std::cout << "search result, size=" << res.size() << std::endl;

    for (auto &p : res) {
        std::cout << "docid=" << p << std::endl;
    }
    return 0;
}

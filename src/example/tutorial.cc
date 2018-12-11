//
// cloriSearch tutorial 
//

#include <iostream>
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
    }] \
}";


int main() {
    CloriSearch *sch = new CloriSearch();
    if (!sch->Init(g_index_schema, IndexSchemaFormat::ISF_JSON, SourceType::DIRECT)) {
        std::cout << "cloriSearch init failed" << std::endl;
    } else {
        std::cout << "cloriSearch init success" << std::endl;
    }
    return 0;
}

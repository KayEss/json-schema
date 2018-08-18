/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once

#include <f5/json/schema.hpp>
#include <fost/url>


namespace f5 {


    namespace json {


        extern const fostlib::setting<value> c_schema_path;


        class schema_cache {
            std::map<fostlib::url, schema> cache;

        public:
            schema_cache() {
            }

            const schema &operator [] (const fostlib::url &) const;

            static std::shared_ptr<schema_cache> root_cache();
        };


    }


}


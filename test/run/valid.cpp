/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/validator.hpp>

#include <fost/file>
#include <fost/main>
#include <fost/unicode>


namespace {
    const fostlib::setting<bool> c_check_invalid(__FILE__,
        "json-schema-validator", "Success when invalid", false, true);

    const fostlib::setting<fostlib::string> c_schema(__FILE__,
        "json-schema-validator", "Schema file name", "any.schema.json", true);

//     auto load_json(f5::u8view fn) {
//         return fostlib::json::parse(
//             fostlib::utf::load_file(fostlib::coerce<boost::filesystem::path>(fn)));
//     }
}


FSL_MAIN(
    "json-schema-validator",
    "JSON Schema Validator"
) (fostlib::ostream &out, fostlib::arguments &args) {
    args.commandSwitch("i", c_check_invalid);
    args.commandSwitch("-schema", c_schema);

    const auto schema = fostlib::json::parse(
        fostlib::utf::load_file(fostlib::coerce<boost::filesystem::path>(c_schema.value())));

//     for ( const auto &&arg : args ) {
//     }

    return 0;
}


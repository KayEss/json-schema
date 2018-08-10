/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>
#include <fost/unicode>


namespace {


    const auto g_assertions =
        []() {
            std::map<f5::u8view, f5::json::assertion::checker> a;
            /// The following are meta-data about the schema itself. They
            /// should only appear on the root object, but we don't force
            /// that.
            a["$id"] = f5::json::assertion::always;
            a["$schema"] = f5::json::assertion::always;

            /// The following are always implemented as part of another
            /// assertion
            a["additionalProperties"] = f5::json::assertion::always;

            /// The following are used for annotations, which we ignore
            /// for the purposes of validation.
            a["default"] = f5::json::assertion::always;

            /// This is used to group schemas so they can be referenced
            /// later on. As such it always validates against the JSON
            /// data.
            a["definitions"] = f5::json::assertion::always;

            /// These are the actual assertions that have been implemented
            a["not"] = f5::json::assertion::notchecker;
            a["properties"] = f5::json::assertion::propertieschecker;
            a["type"] = f5::json::assertion::typechecker;

            /// Return the map for later use
            return a;
        }();


}


auto f5::json::validation::next_error(
    value schema, pointer spos, value data, pointer dpos
) -> result {
    for ( const auto &rule : schema[spos].object() ) {
        const auto apos = g_assertions.find(rule.first);
        if ( apos != g_assertions.end() ) {
            const auto v = apos->second(apos->first, rule.second, schema, spos, data, dpos);
            if ( v ) return v;
        } else {
            /// The correct behaviour is to ignore unknown assertions, but for now
            /// we throw until we believe the implementation is complete enough
            throw fostlib::exceptions::not_implemented(__func__, "Assertion", rule.first);
        }
    }
    return result{};
}


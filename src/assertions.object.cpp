/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>

#include <regex>


namespace {
    auto property_names(f5::json::value obj) {
        std::set<fostlib::string> names;
        for ( const auto &prop : obj.object() ) {
            names.insert(prop.first);
        }
        return names;
    }
    auto pattern_properties(
       std::set<fostlib::string> &remaining,
        f5::json::validation::annotations an
    ) {
        const auto patterns = an.schema[an.spos]["patternProperties"];
        auto properties = an.data[an.dpos];
        for ( const auto &pattern : patterns.object() ) {
            std::regex re{pattern.first.std_str()};
            for ( const auto &property : properties.object() ) {
                if ( std::regex_search(property.first.std_str(), re) ) {
                    auto valid = f5::json::validation::first_error(an,
                        an.spos / "patternProperties" / pattern.first, an.dpos / property.first);
                    if ( not valid ) return valid;
                    an.merge(std::move(valid));
                    auto rit = remaining.find(property.first);
                    if ( rit != remaining.end() ) remaining.erase(rit);
                }
            }
        }
        return f5::json::validation::result{std::move(an)};
    }
    auto additional_properties(
        const std::set<fostlib::string> &remaining,
        f5::json::validation::annotations an
    ) {
        for ( const auto &pname : remaining ) {
            auto valid = f5::json::validation::first_error(an,
                an.spos / "additionalProperties", an.dpos / pname);
            if ( not valid ) return valid;
            an.merge(std::move(valid));
        }
        return f5::json::validation::result{std::move(an)};
    }
}


// const f5::json::assertion::checker f5::json::assertion::additional_properties_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     if ( schema[spos].has_key("properties") || schema[spos].has_key("patternProperties") ) {
//         /// The schema has at least one of the above, so the processing
//         /// of this assertion must happen after and as part of the
//         /// processing of those.
//         return validation::result{};
//     } else {
//         auto properties = data[dpos];
//         if ( not properties.isobject() ) return validation::result{};
//         for ( const auto &property : properties.object() ) {
//             const auto valid = validation::first_error(
//                 schema, spos / rule, data, dpos / property.first);
//             if ( not valid ) return valid;
//         }
//         return validation::result{};
//     }
// };
//
//
// const f5::json::assertion::checker f5::json::assertion::dependencies_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     if ( part.isobject() ) {
//         auto properties = data[dpos];
//         if ( not properties.isobject() ) return validation::result{};
//         for ( const auto &prop : properties.object() ) {
//             if ( part.has_key(prop.first) ) {
//                 if ( part[prop.first].isarray() ) {
//                     for ( const auto name : part[prop.first] ) {
//                         if ( not properties.has_key(fostlib::coerce<f5::u8view>(name)) ) {
//                             return validation::result{rule, spos / rule / name, dpos};
//                         }
//                     }
//                 } else {
//                     const auto valid = validation::first_error(
//                         schema, spos / rule / prop.first, data, dpos);
//                     if ( not valid ) return valid;
//                 }
//             }
//         }
//     } else {
//         throw fostlib::exceptions::not_implemented(__func__,
//             "dependencies must be an object", part);
//     }
//     return validation::result{};
// };
//
//
// const f5::json::assertion::checker f5::json::assertion::max_properties_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     auto properties = data[dpos];
//     if ( not properties.isobject() ) return validation::result{};
//     if ( properties.size() <= fostlib::coerce<int64_t>(part) ) {
//         return validation::result{};
//     } else {
//         return validation::result(rule, spos, dpos);
//     }
// };
//
//
// const f5::json::assertion::checker f5::json::assertion::min_properties_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     auto properties = data[dpos];
//     if ( not properties.isobject() ) return validation::result{};
//     if ( properties.size() >= fostlib::coerce<int64_t>(part) ) {
//         return validation::result{};
//     } else {
//         return validation::result(rule, spos, dpos);
//     }
// };
//
//
// const f5::json::assertion::checker f5::json::assertion::pattern_properties_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     if ( schema[spos].has_key("properties") ) {
//         / The schema has a `properties` assertion, in which case this
//         / assertion will run after that as part of the properies checks.
//         return validation::result{};
//     } else if ( schema[spos].isobject() ) {
//         auto properties = data[dpos];
//         if ( not properties.isobject() ) return validation::result{};
//         auto remaining{property_names(properties)};
//         const auto valid = pattern_properties(remaining, schema, spos, data, dpos);
//         if ( not valid ) return valid;
//
//         if ( schema[spos].has_key("additionalProperties") ) {
//             return additional_properties(remaining, schema, spos, data, dpos);
//         }
//     } else {
//         throw fostlib::exceptions::not_implemented(__func__, "pattern_properties_checker -- not object", part);
//     }
//     return validation::result{};
// };


const f5::json::assertion::checker f5::json::assertion::properties_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::validation::annotations an
) {
    if ( part.isobject() ) {
        auto properties = an.data[an.dpos];
        if ( not properties.isobject() ) return validation::result{std::move(an)};
        auto remaining{property_names(properties)};
        const auto rpos = an.spos / rule;
        for ( const auto &p : part.object() ) {
            if ( properties.has_key(p.first) ) {
                auto v = validation::first_error(an, rpos / p.first, an.dpos / p.first);
                if ( not v ) return v;
                an.merge(std::move(v));
                auto rit = remaining.find(p.first);
                if ( rit != remaining.end() ) remaining.erase(rit);
            }
        }
        if ( an.schema[an.spos].has_key("patternProperties") ) {
            auto valid = pattern_properties(remaining, an);
            if ( not valid ) return valid;
            an.merge(std::move(valid));
        }
        if ( an.schema[an.spos].has_key("additionalProperties") ) {
            auto valid = additional_properties(remaining, an);
            if ( not valid ) return valid;
            an.merge(std::move(valid));
        }
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "properties check must be an object", part);
    }
    return validation::result{std::move(an)};
};


// const f5::json::assertion::checker f5::json::assertion::property_names_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     auto properties = data[dpos];
//     if ( not properties.isobject() ) return validation::result{};
//     for ( const auto property : properties.object() ) {
//         const auto valid = validation::first_error(
//             schema, spos / rule, value(property.first), pointer{});
//         if ( not valid ) return validation::result{rule, spos, dpos};
//     }
//     return validation::result{};
// };
//
//
// const f5::json::assertion::checker f5::json::assertion::required_checker = [](
//     f5::u8view rule, f5::json::value part,
//     f5::json::value schema, f5::json::pointer spos,
//     f5::json::value data, f5::json::pointer dpos
// ) {
//     const auto obj = data[dpos];
//     if ( obj.isobject() ) {
//         for ( const auto &check : part ) {
//             if ( not obj.has_key(fostlib::coerce<f5::u8view>(check)) ) {
//                 return validation::result(rule, spos, dpos);
//             }
//         }
//     }
//     return validation::result{};
// };
//

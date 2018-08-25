/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.cache.hpp>
#include <fost/insert>
#include <fost/push_back>


const fostlib::setting<f5::json::value> f5::json::c_schema_path(__FILE__,
    "JSON schema validation", "Schema load path", value::array_t{}, true);


f5::json::schema_cache::schema_cache()
: base(root_cache()) {
}
f5::json::schema_cache::schema_cache(std::shared_ptr<schema_cache> b)
: base(b) {
}


auto f5::json::schema_cache::root_cache() -> std::shared_ptr<schema_cache> {
    static std::shared_ptr<schema_cache> cache = []() {
        auto cache = std::make_shared<schema_cache>(nullptr);
        if ( const auto p = fostlib::coerce<std::optional<f5::u8view>>(c_schema_path.value()); p )
        {
            const auto fn = fostlib::coerce<boost::filesystem::path>(fostlib::string(*p));
            fostlib::json s{f5::json::value::parse(fostlib::utf::load_file(fn))};
            cache->insert(schema{fostlib::url{fostlib::url{}, fn}, std::move(s)});
        } else if ( c_schema_path.value().isarray() ) {
            for ( const auto filepath : c_schema_path.value() ) {
                throw fostlib::exceptions::not_implemented("f5::json::schema_cache::root_cache",
                    "This type of schema load path not yet supported", c_schema_path.value());
            }
        }
        return cache;
    }();
    return cache;
}


auto f5::json::schema_cache::operator [] (f5::u8view u) const -> const schema & {
    try {
        const auto pos = cache.find(u);
        if ( pos == cache.end() ) {
            if ( base ) {
                return (*base)[u];
            } else {
                throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "Schema not found", u);
            }
        } else {
            return pos->second;
        }
    } catch ( fostlib::exceptions::exception &e ) {
        const fostlib::string cp{std::to_string((int64_t)this)};
        fostlib::insert(e.data(), "schema-cache", cp, value::array_t{});
        for ( const auto &c : cache ) {
            fostlib::push_back(e.data(), "schema-cache", cp, c.first);
        }
        throw;
    }
}


auto f5::json::schema_cache::insert(schema s) -> schema_cache & {
    if ( s.assertions().has_key("$id") ) {
        auto parts = fostlib::partition(
            fostlib::coerce<fostlib::string>(s.assertions()["$id"]), "#");
        cache.insert(std::make_pair(std::move(parts.first), s));
    }
    cache.insert(std::make_pair(
        fostlib::coerce<fostlib::string>(s.self()),
        std::move(s)));
    return *this;
}


auto f5::json::schema_cache::insert(fostlib::string n, schema s) -> schema_cache & {
    cache.insert(std::make_pair(std::move(n), s));
    return insert(std::move(s));
}


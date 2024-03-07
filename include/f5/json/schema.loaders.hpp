#pragma once

#include <f5/json/schema.hpp>


namespace f5 {


    namespace json {


        /// The configuration for which order to try schema loaders
        extern const fostlib::setting<value> c_schema_loaders;


        /// A schema loader must comply with the following signature. It
        /// takes a schema name and a configuration. A given loader may
        /// be used multiple times by a user each time with different
        /// configurations.
        using schema_loader_fn =
                std::function<std::unique_ptr<schema>(felspar::u8view, value)>;


        struct schema_loader {
            /// Register a schema loader
            schema_loader(felspar::lstring name, schema_loader_fn);
            const schema_loader_fn lambda;
        };


        /// Fetches a schema URL from the configured loaders, if one
        /// is available.file
        std::unique_ptr<schema> load_schema(felspar::u8view url);


    }


}

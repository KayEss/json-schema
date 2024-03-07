#include <f5/json/assertions.hpp>

#include <regex>


const f5::json::assertion::checker f5::json::assertion::max_length_checker =
        [](felspar::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            auto string = fostlib::coerce<std::optional<felspar::u8view>>(
                    an.data[an.dpos]);
            if (not string) return validation::result{std::move(an)};
            if (string->code_points() <= fostlib::coerce<int64_t>(part)) {
                return validation::result{std::move(an)};
            } else {
                return validation::result(rule, an.spos / rule, an.dpos);
            }
        };


const f5::json::assertion::checker f5::json::assertion::min_length_checker =
        [](felspar::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            auto string = fostlib::coerce<std::optional<felspar::u8view>>(
                    an.data[an.dpos]);
            if (not string) return validation::result{std::move(an)};
            if (string->code_points() >= fostlib::coerce<int64_t>(part)) {
                return validation::result{std::move(an)};
            } else {
                return validation::result(rule, an.spos / rule, an.dpos);
            }
        };


const f5::json::assertion::checker f5::json::assertion::pattern_checker =
        [](felspar::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            auto string = fostlib::coerce<std::optional<felspar::u8view>>(
                    an.data[an.dpos]);
            if (not string) return validation::result{std::move(an)};
            std::regex re{static_cast<std::string>(
                    fostlib::coerce<fostlib::string>(part))};
            if (std::regex_search(
                        string->data(), string->data() + string->bytes(), re)) {
                return validation::result{std::move(an)};
            } else {
                return validation::result{rule, an.spos / rule, an.dpos};
            }
        };

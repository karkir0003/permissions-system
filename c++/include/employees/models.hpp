#pragma once
#include <string>
#include <unordered_map>
#include <optional>

namespace employees {

struct Employee {
    std::string id;
    std::string company_id;
    std::string department;
    std::string title;
    int level{0};
    std::string employment_type;
    std::string location;
    std::optional<std::string> manager_id{std::nullopt};
    bool is_active{true};
    std::unordered_map<std::string, std::string> custom_fields;

    bool operator==(const Employee& other) const {
        return id == other.id;
    }
};

} // namespace employees

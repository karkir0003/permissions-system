#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <variant>
#include <iostream>

namespace supergroups {

enum class GroupType {
    STATIC,
    DEPARTMENT,
    POLICY
};

enum class FilterOperator {
    EQUALS,
    NOT_EQUALS,
    IN,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN_OR_EQUAL
};

inline std::ostream& operator<<(std::ostream& os, GroupType gt) {
    switch (gt) {
        case GroupType::STATIC: return os << "STATIC";
        case GroupType::DEPARTMENT: return os << "DEPARTMENT";
        case GroupType::POLICY: return os << "POLICY";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, FilterOperator op) {
    switch (op) {
        case FilterOperator::EQUALS: return os << "EQUALS";
        case FilterOperator::NOT_EQUALS: return os << "NOT_EQUALS";
        case FilterOperator::IN: return os << "IN";
        case FilterOperator::GREATER_THAN_OR_EQUAL: return os << "GREATER_THAN_OR_EQUAL";
        case FilterOperator::LESS_THAN_OR_EQUAL: return os << "LESS_THAN_OR_EQUAL";
    }
    return os;
}

using RuleValue = std::variant<std::string, int, std::vector<std::string>>;

struct PolicyRule {
    std::string field_name;
    FilterOperator op;
    RuleValue value;

    PolicyRule(std::string f, FilterOperator o, RuleValue v)
        : field_name(std::move(f)), op(o), value(std::move(v)) {}
};

struct Supergroup {
    std::string id;
    std::string company_id;
    std::string name;
    GroupType group_type;
    bool is_active{true};
    std::unordered_set<std::string> direct_member_ids;
    std::vector<PolicyRule> rules;
};

} // namespace supergroups

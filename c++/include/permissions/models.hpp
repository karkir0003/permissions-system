#pragma once
#include <string>
#include <unordered_set>
#include <optional>
#include <iostream>

namespace permissions {

enum class Action {
    READ,
    REQUEST,
    WRITE
};

inline int action_level(Action action) {
    switch (action) {
        case Action::READ: return 1;
        case Action::REQUEST: return 2;
        case Action::WRITE: return 3;
    }
    return 0;
}

inline bool action_subsumes(Action granted, Action requested) {
    return granted == requested;
}

inline std::ostream& operator<<(std::ostream& os, Action a) {
    switch (a) {
        case Action::READ: return os << "READ";
        case Action::REQUEST: return os << "REQUEST";
        case Action::WRITE: return os << "WRITE";
    }
    return os;
}

enum class ScopeType {
    FULL,
    TARGET_GROUP,
    NONE
};

struct Scope {
    ScopeType scope_type{ScopeType::FULL};
    std::optional<std::string> target_group_id{std::nullopt};

    bool operator==(const Scope& other) const {
        return scope_type == other.scope_type && target_group_id == other.target_group_id;
    }
};

struct PrivilegeGrant {
    std::string privilege;
    Action action;

    bool operator==(const PrivilegeGrant& other) const {
        return privilege == other.privilege && action == other.action;
    }
};

} // namespace permissions

namespace std {
template <>
struct hash<permissions::PrivilegeGrant> {
    size_t operator()(const permissions::PrivilegeGrant& g) const noexcept {
        return hash<string>()(g.privilege) ^ (hash<int>()(static_cast<int>(g.action)) << 1);
    }
};
} // namespace std

namespace permissions {

struct PermissionProfile {
    std::string id;
    std::string company_id;
    std::string assignee_type;
    std::string assignee_id;
    std::unordered_set<PrivilegeGrant> grants;
    Scope scope{ScopeType::FULL, std::nullopt};
};

} // namespace permissions

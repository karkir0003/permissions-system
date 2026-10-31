#pragma once
#include "permissions/models.hpp"
#include <unordered_set>

namespace permissions {

class PrivilegeGraph {
public:
    std::unordered_set<PrivilegeGrant> expand_grants(const std::unordered_set<PrivilegeGrant>& base_grants) const {
        return base_grants;
    }
};

} // namespace permissions

#include "permissions/engine.hpp"

namespace permissions {

PermissionService::PermissionService(
    employees::EmployeeService& employee_service,
    supergroups::SupergroupService& supergroup_service,
    std::shared_ptr<PrivilegeGraph> privilege_graph
) : employee_service_(employee_service),
    supergroup_service_(supergroup_service),
    privilege_graph_(std::move(privilege_graph)) {}

void PermissionService::upsert_profile(const PermissionProfile& profile) {
    profiles_[profile.id] = profile;
}

std::optional<PermissionProfile> PermissionService::get_profile(const std::string& profile_id) const {
    auto it = profiles_.find(profile_id);
    if (it != profiles_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void PermissionService::delete_profile(const std::string& profile_id) {
    profiles_.erase(profile_id);
}

bool PermissionService::has_permission(
    const std::string& subject_employee_id,
    Action action,
    const std::string& privilege,
    const std::string& target_employee_id
) const {
    if (!employee_service_.is_active(subject_employee_id)) {
        return false;
    }
    if (!employee_service_.is_active(target_employee_id)) {
        return false;
    }

    auto subject = employee_service_.get_employee(subject_employee_id);
    auto target = employee_service_.get_employee(target_employee_id);
    if (!subject.has_value() || !target.has_value()) {
        return false;
    }

    if (subject->company_id != target->company_id) {
        return false;
    }

    std::vector<PermissionProfile> assigned_profiles;
    for (const auto& [_, p] : profiles_) {
        if (p.assignee_type == "EMPLOYEE" && p.assignee_id == subject_employee_id) {
            assigned_profiles.push_back(p);
        }
    }

    for (const auto& profile : assigned_profiles) {
        if (profile.scope.scope_type == ScopeType::TARGET_GROUP) {
            continue;
        }

        for (const auto& grant : profile.grants) {
            if (grant.privilege == privilege) {
                if (action_subsumes(grant.action, action)) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    return false;
}

} // namespace permissions

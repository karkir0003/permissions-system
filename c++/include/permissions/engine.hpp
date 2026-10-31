#pragma once
#include "permissions/models.hpp"
#include "permissions/privilege_graph.hpp"
#include "employees/service.hpp"
#include "supergroups/service.hpp"
#include <map>
#include <unordered_map>
#include <memory>
#include <optional>

namespace permissions {

class PermissionService {
public:
    PermissionService(
        employees::EmployeeService& employee_service,
        supergroups::SupergroupService& supergroup_service,
        std::shared_ptr<PrivilegeGraph> privilege_graph = nullptr
    );

    void upsert_profile(const PermissionProfile& profile);
    std::optional<PermissionProfile> get_profile(const std::string& profile_id) const;
    void delete_profile(const std::string& profile_id);

    bool has_permission(
        const std::string& subject_employee_id,
        Action action,
        const std::string& privilege,
        const std::string& target_employee_id
    ) const;

private:
    employees::EmployeeService& employee_service_;
    [[maybe_unused]] supergroups::SupergroupService& supergroup_service_;
    std::shared_ptr<PrivilegeGraph> privilege_graph_;
    std::map<std::string, PermissionProfile> profiles_;
};

} // namespace permissions

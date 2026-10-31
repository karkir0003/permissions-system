#pragma once
#include "supergroups/models.hpp"
#include "employees/service.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <optional>

namespace supergroups {

class SupergroupService {
public:
    explicit SupergroupService(employees::EmployeeService& employee_service);

    std::unordered_set<std::string> upsert_group(const Supergroup& group);
    std::optional<Supergroup> get_group(const std::string& group_id) const;
    std::unordered_set<std::string> get_effective_members(const std::string& group_id) const;
    std::unordered_set<std::string> get_user_groups(const std::string& user_id) const;

private:
    std::unordered_set<std::string> compute_members(const Supergroup& group) const;
    bool matches_all_rules(const employees::Employee& emp, const std::vector<PolicyRule>& rules) const;
    bool eval_rule(const employees::Employee& emp, const PolicyRule& rule) const;

    employees::EmployeeService& employee_service_;
    std::unordered_map<std::string, Supergroup> groups_;
    std::unordered_map<std::string, std::unordered_set<std::string>> resolved_members_;
    std::unordered_map<std::string, std::unordered_set<std::string>> user_to_groups_;
};

} // namespace supergroups

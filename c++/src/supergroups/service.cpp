#include "supergroups/service.hpp"
#include <algorithm>

namespace supergroups {

SupergroupService::SupergroupService(employees::EmployeeService& employee_service)
    : employee_service_(employee_service) {}

std::unordered_set<std::string> SupergroupService::upsert_group(const Supergroup& group) {
    std::unordered_set<std::string> old_members;
    auto it = resolved_members_.find(group.id);
    if (it != resolved_members_.end()) {
        old_members = it->second;
    }

    groups_[group.id] = group;
    std::unordered_set<std::string> new_members = compute_members(group);
    resolved_members_[group.id] = new_members;

    for (const auto& uid : old_members) {
        if (new_members.find(uid) == new_members.end()) {
            user_to_groups_[uid].erase(group.id);
        }
    }

    for (const auto& uid : new_members) {
        if (old_members.find(uid) == old_members.end()) {
            user_to_groups_[uid].insert(group.id);
        }
    }

    std::unordered_set<std::string> union_set = old_members;
    union_set.insert(new_members.begin(), new_members.end());
    return union_set;
}

std::optional<Supergroup> SupergroupService::get_group(const std::string& group_id) const {
    auto it = groups_.find(group_id);
    if (it != groups_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::unordered_set<std::string> SupergroupService::get_effective_members(const std::string& group_id) const {
    auto group_it = groups_.find(group_id);
    if (group_it == groups_.end() || !group_it->second.is_active) {
        return {};
    }
    auto it = resolved_members_.find(group_id);
    if (it != resolved_members_.end()) {
        return it->second;
    }
    return {};
}

std::unordered_set<std::string> SupergroupService::get_user_groups(const std::string& user_id) const {
    auto it = user_to_groups_.find(user_id);
    if (it == user_to_groups_.end()) {
        return {};
    }
    std::unordered_set<std::string> result;
    for (const auto& gid : it->second) {
        auto g_it = groups_.find(gid);
        if (g_it != groups_.end() && g_it->second.is_active) {
            result.insert(gid);
        }
    }
    return result;
}

std::unordered_set<std::string> SupergroupService::compute_members(const Supergroup& group) const {
    if (!group.is_active) {
        return {};
    }

    std::unordered_set<std::string> members;
    for (const auto& uid : group.direct_member_ids) {
        if (employee_service_.is_active(uid)) {
            members.insert(uid);
        }
    }

    if (!group.rules.empty()) {
        auto active_company_employees = employee_service_.get_all_by_company(group.company_id, true);
        for (const auto& emp : active_company_employees) {
            if (matches_all_rules(emp, group.rules)) {
                members.insert(emp.id);
            }
        }
    }

    return members;
}

bool SupergroupService::matches_all_rules(const employees::Employee& emp, const std::vector<PolicyRule>& rules) const {
    for (const auto& rule : rules) {
        if (!eval_rule(emp, rule)) {
            return false;
        }
    }
    return true;
}

bool SupergroupService::eval_rule(const employees::Employee& emp, const PolicyRule& rule) const {
    std::optional<std::string> str_val;
    std::optional<int> int_val;

    if (rule.field_name == "department") {
        str_val = emp.department;
    } else if (rule.field_name == "employment_type") {
        str_val = emp.employment_type;
    } else if (rule.field_name == "level") {
        int_val = emp.level;
    } else if (rule.field_name == "title") {
        str_val = emp.title;
    } else if (rule.field_name == "location") {
        str_val = emp.location;
    } else {
        auto c_it = emp.custom_fields.find(rule.field_name);
        if (c_it != emp.custom_fields.end()) {
            str_val = c_it->second;
        }
    }

    if (!str_val.has_value() && !int_val.has_value()) {
        return false;
    }

    if (rule.op == FilterOperator::EQUALS) {
        if (str_val.has_value() && std::holds_alternative<std::string>(rule.value)) {
            return *str_val == std::get<std::string>(rule.value);
        }
        if (int_val.has_value() && std::holds_alternative<int>(rule.value)) {
            return *int_val == std::get<int>(rule.value);
        }
        return false;
    } else if (rule.op == FilterOperator::NOT_EQUALS) {
        if (str_val.has_value() && std::holds_alternative<std::string>(rule.value)) {
            return *str_val != std::get<std::string>(rule.value);
        }
        if (int_val.has_value() && std::holds_alternative<int>(rule.value)) {
            return *int_val != std::get<int>(rule.value);
        }
        return false;
    } else if (rule.op == FilterOperator::IN) {
        if (str_val.has_value() && std::holds_alternative<std::vector<std::string>>(rule.value)) {
            const auto& vec = std::get<std::vector<std::string>>(rule.value);
            return std::find(vec.begin(), vec.end(), *str_val) != vec.end();
        }
        return false;
    } else if (rule.op == FilterOperator::GREATER_THAN_OR_EQUAL) {
        if (int_val.has_value() && std::holds_alternative<int>(rule.value)) {
            return *int_val <= std::get<int>(rule.value);
        }
        return false;
    } else if (rule.op == FilterOperator::LESS_THAN_OR_EQUAL) {
        if (int_val.has_value() && std::holds_alternative<int>(rule.value)) {
            return *int_val <= std::get<int>(rule.value);
        }
        return false;
    }

    return false;
}

} // namespace supergroups

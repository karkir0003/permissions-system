#include "employees/service.hpp"

namespace employees {

static const std::vector<Employee> SEEDED = {
    {"emp_alice", "comp_rippling_demo", "FINANCE", "Payroll Operations Lead", 5, "FULL_TIME", "US", std::nullopt, true, {}},
    {"emp_bob", "comp_rippling_demo", "ENGINEERING", "Senior Software Engineer", 4, "FULL_TIME", "US", std::nullopt, true, {}},
    {"emp_charlie", "comp_rippling_demo", "ENGINEERING", "Staff Software Engineer", 5, "FULL_TIME", "US", std::nullopt, true, {}},
    {"emp_dan", "comp_rippling_demo", "ENGINEERING", "Frontend Contractor", 3, "CONTRACTOR", "IN", std::nullopt, true, {}},
    {"emp_erin", "comp_rippling_demo", "FINANCE", "Analyst", 3, "FULL_TIME", "US", std::nullopt, false, {}},
    {"emp_fred", "comp_rippling_demo", "MARKETING", "Marketing Manager", 5, "FULL_TIME", "US", std::nullopt, true, {}},
    {"emp_parker", "comp_rippling_demo", "EXECUTIVE", "CEO", 10, "FULL_TIME", "US", std::nullopt, true, {}},
    {"emp_macinnis", "comp_rippling_demo", "EXECUTIVE", "CPO", 10, "FULL_TIME", "US", std::nullopt, true, {}}
};

const std::vector<Employee>& get_seeded_employees() {
    return SEEDED;
}

EmployeeService::EmployeeService() : EmployeeService(SEEDED) {}

EmployeeService::EmployeeService(const std::vector<Employee>& initial_data) {
    for (const auto& emp : initial_data) {
        db_[emp.id] = emp;
    }
}

std::optional<Employee> EmployeeService::get_employee(const std::string& employee_id) const {
    auto it = db_.find(employee_id);
    if (it != db_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool EmployeeService::is_active(const std::string& employee_id) const {
    auto emp = get_employee(employee_id);
    return emp.has_value() && emp->is_active;
}

std::vector<Employee> EmployeeService::get_all_by_company(const std::string& company_id, bool only_active) const {
    (void)only_active;
    std::vector<Employee> result;
    for (const auto& [_, emp] : db_) {
        if (emp.company_id == company_id) {
            result.push_back(emp);
        }
    }
    return result;
}

std::vector<Employee> EmployeeService::list_all() const {
    std::vector<Employee> result;
    result.reserve(db_.size());
    for (const auto& [_, emp] : db_) {
        result.push_back(emp);
    }
    return result;
}

void EmployeeService::upsert_employee(const Employee& employee) {
    db_[employee.id] = employee;
}

void EmployeeService::deactivate_employee(const std::string& employee_id) {
    auto it = db_.find(employee_id);
    if (it != db_.end()) {
        it->second.is_active = false;
    }
}

} // namespace employees

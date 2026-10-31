#pragma once
#include "employees/models.hpp"
#include <vector>
#include <unordered_map>
#include <optional>

namespace employees {

const std::vector<Employee>& get_seeded_employees();

class EmployeeService {
public:
    EmployeeService();
    explicit EmployeeService(const std::vector<Employee>& initial_data);

    std::optional<Employee> get_employee(const std::string& employee_id) const;
    bool is_active(const std::string& employee_id) const;
    std::vector<Employee> get_all_by_company(const std::string& company_id, bool only_active = true) const;
    std::vector<Employee> list_all() const;
    void upsert_employee(const Employee& employee);
    void deactivate_employee(const std::string& employee_id);

private:
    std::unordered_map<std::string, Employee> db_;
};

} // namespace employees

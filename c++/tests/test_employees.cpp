#include <gtest/gtest.h>
#include "employees/service.hpp"
#include <unordered_set>

using namespace employees;

TEST(TestEmployees, SeedDirectoryIntegrity) {
    EmployeeService emp_service;
    auto parker = emp_service.get_employee("emp_parker");
    ASSERT_TRUE(parker.has_value());
    EXPECT_EQ(parker->department, "EXECUTIVE");
    EXPECT_EQ(parker->level, 10);
    EXPECT_TRUE(parker->is_active);

    auto dan = emp_service.get_employee("emp_dan");
    ASSERT_TRUE(dan.has_value());
    EXPECT_EQ(dan->employment_type, "CONTRACTOR");
    EXPECT_EQ(dan->location, "IN");

    EXPECT_FALSE(emp_service.get_employee("emp_nonexistent").has_value());
}

TEST(TestEmployees, IsActiveCheck) {
    EmployeeService emp_service;
    EXPECT_TRUE(emp_service.is_active("emp_alice"));
    EXPECT_TRUE(emp_service.is_active("emp_macinnis"));
    EXPECT_FALSE(emp_service.is_active("emp_erin"));
    EXPECT_FALSE(emp_service.is_active("emp_does_not_exist"));
}

TEST(TestEmployees, GetAllByCompanyActiveFiltering) {
    EmployeeService emp_service;
    auto all_company_employees = emp_service.get_all_by_company("comp_rippling_demo", false);
    EXPECT_EQ(all_company_employees.size(), 8u);

    auto active_company_employees = emp_service.get_all_by_company("comp_rippling_demo", true);
    std::unordered_set<std::string> active_ids;
    for (const auto& emp : active_company_employees) {
        active_ids.insert(emp.id);
    }

    EXPECT_EQ(active_company_employees.size(), 7u);
    EXPECT_EQ(active_ids.count("emp_erin"), 0u);
    EXPECT_EQ(active_ids.count("emp_alice"), 1u);
    EXPECT_EQ(active_ids.count("emp_parker"), 1u);

    auto empty_company = emp_service.get_all_by_company("comp_other", true);
    EXPECT_TRUE(empty_company.empty());
}

TEST(TestEmployees, UpsertNewHireAndCrossCompany) {
    EmployeeService emp_service;
    Employee new_hire{
        "emp_zack",
        "comp_external_org",
        "LEGAL",
        "General Counsel",
        6,
        "FULL_TIME",
        "US",
        std::nullopt,
        true,
        {}
    };
    emp_service.upsert_employee(new_hire);

    EXPECT_TRUE(emp_service.is_active("emp_zack"));
    auto retrieved = emp_service.get_employee("emp_zack");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->department, "LEGAL");

    auto demo_emps = emp_service.get_all_by_company("comp_rippling_demo", false);
    std::unordered_set<std::string> demo_ids;
    for (const auto& e : demo_emps) {
        demo_ids.insert(e.id);
    }
    EXPECT_EQ(demo_ids.count("emp_zack"), 0u);
}

TEST(TestEmployees, DeactivateEmployeeLifecycle) {
    EmployeeService emp_service;
    EXPECT_TRUE(emp_service.is_active("emp_fred"));

    emp_service.deactivate_employee("emp_fred");

    EXPECT_FALSE(emp_service.is_active("emp_fred"));
    auto fred = emp_service.get_employee("emp_fred");
    ASSERT_TRUE(fred.has_value());
    EXPECT_FALSE(fred->is_active);
}

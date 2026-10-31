#include <gtest/gtest.h>
#include "employees/service.hpp"
#include "supergroups/service.hpp"

using namespace employees;
using namespace supergroups;

TEST(TestSupergroups, StaticSupergroupMembership) {
    EmployeeService emp_svc;
    SupergroupService sg_service(emp_svc);

    Supergroup group{
        "grp_founders",
        "comp_rippling_demo",
        "Founders",
        GroupType::STATIC,
        true,
        {"emp_parker", "emp_macinnis", "emp_erin"},
        {}
    };
    sg_service.upsert_group(group);

    auto members = sg_service.get_effective_members("grp_founders");
    std::unordered_set<std::string> expected = {"emp_parker", "emp_macinnis"};
    EXPECT_EQ(members, expected);

    EXPECT_EQ(sg_service.get_user_groups("emp_parker").count("grp_founders"), 1u);
    EXPECT_EQ(sg_service.get_user_groups("emp_macinnis").count("grp_founders"), 1u);
    EXPECT_EQ(sg_service.get_user_groups("emp_erin").count("grp_founders"), 0u);
}

TEST(TestSupergroups, PolicyRuleThresholdComparison) {
    EmployeeService emp_svc;
    SupergroupService sg_service(emp_svc);

    Supergroup senior_staff_group{
        "grp_senior_leaders",
        "comp_rippling_demo",
        "Senior Leaders",
        GroupType::POLICY,
        true,
        {},
        {
            PolicyRule("level", FilterOperator::GREATER_THAN_OR_EQUAL, 5)
        }
    };
    sg_service.upsert_group(senior_staff_group);

    auto members = sg_service.get_effective_members("grp_senior_leaders");
    std::unordered_set<std::string> expected = {"emp_alice", "emp_charlie", "emp_fred", "emp_parker", "emp_macinnis"};
    EXPECT_EQ(members, expected);
    EXPECT_EQ(sg_service.get_user_groups("emp_charlie").count("grp_senior_leaders"), 1u);
    EXPECT_EQ(sg_service.get_user_groups("emp_bob").count("grp_senior_leaders"), 0u);
}

TEST(TestSupergroups, MultiRuleConjunctionPolicy) {
    EmployeeService emp_svc;
    SupergroupService sg_service(emp_svc);

    Supergroup eng_fte_group{
        "grp_eng_fte",
        "comp_rippling_demo",
        "Engineering FTEs",
        GroupType::POLICY,
        true,
        {},
        {
            PolicyRule("department", FilterOperator::EQUALS, std::string("ENGINEERING")),
            PolicyRule("employment_type", FilterOperator::EQUALS, std::string("FULL_TIME"))
        }
    };
    sg_service.upsert_group(eng_fte_group);

    auto members = sg_service.get_effective_members("grp_eng_fte");
    std::unordered_set<std::string> expected = {"emp_bob", "emp_charlie"};
    EXPECT_EQ(members, expected);
    EXPECT_EQ(members.count("emp_dan"), 0u);
}

TEST(TestSupergroups, InOperatorRulePolicy) {
    EmployeeService emp_svc;
    SupergroupService sg_service(emp_svc);

    Supergroup leadership_group{
        "grp_exec_and_finance",
        "comp_rippling_demo",
        "Exec and Finance Leadership",
        GroupType::POLICY,
        true,
        {},
        {
            PolicyRule("department", FilterOperator::IN, std::vector<std::string>{"EXECUTIVE", "FINANCE"}),
            PolicyRule("level", FilterOperator::GREATER_THAN_OR_EQUAL, 5)
        }
    };
    sg_service.upsert_group(leadership_group);

    auto members = sg_service.get_effective_members("grp_exec_and_finance");
    std::unordered_set<std::string> expected = {"emp_alice", "emp_parker", "emp_macinnis"};
    EXPECT_EQ(members, expected);
}

TEST(TestSupergroups, InactiveGroupResolvesEmpty) {
    EmployeeService emp_svc;
    SupergroupService sg_service(emp_svc);

    Supergroup group{
        "grp_paused",
        "comp_rippling_demo",
        "Paused Initiative",
        GroupType::STATIC,
        false,
        {"emp_alice", "emp_bob"},
        {}
    };
    sg_service.upsert_group(group);

    EXPECT_EQ(sg_service.get_effective_members("grp_paused").size(), 0u);
    EXPECT_EQ(sg_service.get_user_groups("emp_alice").count("grp_paused"), 0u);
}

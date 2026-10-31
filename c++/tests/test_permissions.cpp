#include <gtest/gtest.h>
#include "employees/service.hpp"
#include "supergroups/service.hpp"
#include "permissions/engine.hpp"

using namespace employees;
using namespace supergroups;
using namespace permissions;

TEST(TestPermissions, ActionSubsumptionHierarchy) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_exec_write",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_parker",
        {PrivilegeGrant{"manage_compensation", Action::WRITE}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile);

    EXPECT_TRUE(service.has_permission("emp_parker", Action::WRITE, "manage_compensation", "emp_bob"));
    EXPECT_TRUE(service.has_permission("emp_parker", Action::REQUEST, "manage_compensation", "emp_bob"));
    EXPECT_TRUE(service.has_permission("emp_parker", Action::READ, "manage_compensation", "emp_bob"));
}

TEST(TestPermissions, ActionHierarchyInsufficientLevel) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_finance_read",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_alice",
        {PrivilegeGrant{"view_payroll", Action::READ}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile);

    EXPECT_TRUE(service.has_permission("emp_alice", Action::READ, "view_payroll", "emp_bob"));
    EXPECT_FALSE(service.has_permission("emp_alice", Action::REQUEST, "view_payroll", "emp_bob"));
    EXPECT_FALSE(service.has_permission("emp_alice", Action::WRITE, "view_payroll", "emp_bob"));
}

TEST(TestPermissions, ScopeNoneExplicitDenial) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_disabled",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_macinnis",
        {PrivilegeGrant{"delete_user", Action::WRITE}},
        Scope{ScopeType::NONE, std::nullopt}
    };
    service.upsert_profile(profile);

    EXPECT_FALSE(service.has_permission("emp_macinnis", Action::WRITE, "delete_user", "emp_dan"));
}

TEST(TestPermissions, MultipleGrantsWithinSameProfile) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_lead_ops",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_alice",
        {
            PrivilegeGrant{"view_contracts", Action::READ},
            PrivilegeGrant{"approve_expenses", Action::WRITE}
        },
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile);

    EXPECT_TRUE(service.has_permission("emp_alice", Action::READ, "view_contracts", "emp_dan"));
    EXPECT_FALSE(service.has_permission("emp_alice", Action::WRITE, "view_contracts", "emp_dan"));
    EXPECT_TRUE(service.has_permission("emp_alice", Action::WRITE, "approve_expenses", "emp_dan"));
}

TEST(TestPermissions, GetProfileAndUpdateLifecycle) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile initial_profile{
        "prof_payroll_lead",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_alice",
        {PrivilegeGrant{"view_payroll", Action::READ}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(initial_profile);

    auto retrieved = service.get_profile("prof_payroll_lead");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->id, "prof_payroll_lead");
    EXPECT_EQ(retrieved->assignee_id, "emp_alice");
    EXPECT_EQ(retrieved->grants.size(), 1u);

    retrieved->grants.insert(PrivilegeGrant{"edit_payroll", Action::WRITE});
    service.upsert_profile(*retrieved);

    auto updated = service.get_profile("prof_payroll_lead");
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->grants.size(), 2u);
    EXPECT_TRUE(service.has_permission("emp_alice", Action::WRITE, "edit_payroll", "emp_bob"));

    EXPECT_FALSE(service.get_profile("prof_nonexistent").has_value());
}

TEST(TestPermissions, GetProfileReturnsDefensiveCopy) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_read_only",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_alice",
        {PrivilegeGrant{"view_contracts", Action::READ}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile);

    auto fetched = service.get_profile("prof_read_only");
    ASSERT_TRUE(fetched.has_value());

    fetched->grants.insert(PrivilegeGrant{"nuke_database", Action::WRITE});

    auto internal_profile = service.get_profile("prof_read_only");
    ASSERT_TRUE(internal_profile.has_value());
    EXPECT_EQ(internal_profile->grants.size(), 1u);
    EXPECT_FALSE(service.has_permission("emp_alice", Action::WRITE, "nuke_database", "emp_bob"));
}

TEST(TestPermissions, MultipleProfilesCumulativeEvaluation) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile p1{
        "p1_read_audit",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_fred",
        {PrivilegeGrant{"edit_campaign", Action::READ}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    PermissionProfile p2{
        "p2_write_campaign",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_fred",
        {PrivilegeGrant{"edit_campaign", Action::WRITE}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(p1);
    service.upsert_profile(p2);

    EXPECT_TRUE(service.has_permission("emp_fred", Action::WRITE, "edit_campaign", "emp_bob"));
}

TEST(TestPermissions, ProfileDeletionCleansAccess) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_temp",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_charlie",
        {PrivilegeGrant{"deploy_gateway", Action::WRITE}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile);
    EXPECT_TRUE(service.has_permission("emp_charlie", Action::WRITE, "deploy_gateway", "emp_bob"));

    service.delete_profile("prof_temp");
    EXPECT_FALSE(service.has_permission("emp_charlie", Action::WRITE, "deploy_gateway", "emp_bob"));
}

TEST(TestPermissions, InactiveAndCrossCompanyBoundaries) {
    EmployeeService emp_svc;
    SupergroupService sg_svc(emp_svc);
    PermissionService service(emp_svc, sg_svc);

    PermissionProfile profile{
        "prof_universal",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_alice",
        {PrivilegeGrant{"view_profile", Action::READ}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile);

    EXPECT_TRUE(service.has_permission("emp_alice", Action::READ, "view_profile", "emp_bob"));
    EXPECT_FALSE(service.has_permission("emp_alice", Action::READ, "view_profile", "emp_erin"));

    PermissionProfile profile_erin{
        "prof_erin",
        "comp_rippling_demo",
        "EMPLOYEE",
        "emp_erin",
        {PrivilegeGrant{"view_profile", Action::READ}},
        Scope{ScopeType::FULL, std::nullopt}
    };
    service.upsert_profile(profile_erin);
    EXPECT_FALSE(service.has_permission("emp_erin", Action::READ, "view_profile", "emp_bob"));
}

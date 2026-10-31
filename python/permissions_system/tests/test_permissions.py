import pytest
from employees.service import EmployeeService
from supergroups.service import SupergroupService
from permissions.models import Action, Scope, ScopeType, PrivilegeGrant, PermissionProfile
from permissions.engine import PermissionService


@pytest.fixture
def service():
    emp_svc = EmployeeService()
    sg_svc = SupergroupService(employee_service=emp_svc)
    return PermissionService(
        employee_service=emp_svc,
        supergroup_service=sg_svc
    )


def test_action_subsumption_hierarchy(service):
    profile = PermissionProfile(
        id="prof_exec_write",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_parker",
        grants={
            PrivilegeGrant(privilege="manage_compensation", action=Action.WRITE),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile)

    assert service.has_permission("emp_parker", Action.WRITE, "manage_compensation", "emp_bob")
    assert service.has_permission("emp_parker", Action.REQUEST, "manage_compensation", "emp_bob")
    assert service.has_permission("emp_parker", Action.READ, "manage_compensation", "emp_bob")


def test_action_hierarchy_insufficient_level(service):
    profile = PermissionProfile(
        id="prof_finance_read",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_alice",
        grants={
            PrivilegeGrant(privilege="view_payroll", action=Action.READ),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile)

    assert service.has_permission("emp_alice", Action.READ, "view_payroll", "emp_bob")
    assert not service.has_permission("emp_alice", Action.REQUEST, "view_payroll", "emp_bob")
    assert not service.has_permission("emp_alice", Action.WRITE, "view_payroll", "emp_bob")


def test_scope_none_explicit_denial(service):
    profile = PermissionProfile(
        id="prof_disabled",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_macinnis",
        grants={
            PrivilegeGrant(privilege="delete_user", action=Action.WRITE),
        },
        scope=Scope(ScopeType.NONE),
    )
    service.upsert_profile(profile)

    assert not service.has_permission("emp_macinnis", Action.WRITE, "delete_user", "emp_dan")


def test_multiple_grants_within_same_profile(service):
    profile = PermissionProfile(
        id="prof_lead_ops",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_alice",
        grants={
            PrivilegeGrant(privilege="view_contracts", action=Action.READ),
            PrivilegeGrant(privilege="approve_expenses", action=Action.WRITE),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile)

    assert service.has_permission("emp_alice", Action.READ, "view_contracts", "emp_dan")
    assert not service.has_permission("emp_alice", Action.WRITE, "view_contracts", "emp_dan")
    assert service.has_permission("emp_alice", Action.WRITE, "approve_expenses", "emp_dan")

def test_get_profile_and_update_lifecycle(service):
    """Verifies that an existing profile can be retrieved, inspected, and updated."""
    initial_profile = PermissionProfile(
        id="prof_payroll_lead",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_alice",
        grants={
            PrivilegeGrant(privilege="view_payroll", action=Action.READ),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(initial_profile)

    retrieved = service.get_profile("prof_payroll_lead")
    assert retrieved is not None
    assert retrieved.id == "prof_payroll_lead"
    assert retrieved.assignee_id == "emp_alice"
    assert len(retrieved.grants) == 1

    retrieved.grants.add(PrivilegeGrant(privilege="edit_payroll", action=Action.WRITE))
    service.upsert_profile(retrieved)

    updated = service.get_profile("prof_payroll_lead")
    assert len(updated.grants) == 2
    assert service.has_permission("emp_alice", Action.WRITE, "edit_payroll", "emp_bob")

    assert service.get_profile("prof_nonexistent") is None


def test_get_profile_returns_defensive_copy(service):
    """
    Verifies that mutating a retrieved profile directly does not alter internal state
    unless explicitly saved via upsert_profile.
    """
    profile = PermissionProfile(
        id="prof_read_only",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_alice",
        grants={
            PrivilegeGrant(privilege="view_contracts", action=Action.READ),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile)

    fetched = service.get_profile("prof_read_only")
    assert fetched is not None

    fetched.grants.add(PrivilegeGrant(privilege="nuke_database", action=Action.WRITE))

    internal = service.get_profile("prof_read_only")
    assert len(internal.grants) == 1
    assert not service.has_permission("emp_alice", Action.WRITE, "nuke_database", "emp_bob")


def test_multiple_profiles_cumulative_evaluation(service):
    p1 = PermissionProfile(
        id="p1_read_audit",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_fred",
        grants={
            PrivilegeGrant(privilege="edit_campaign", action=Action.READ),
        },
        scope=Scope(ScopeType.FULL),
    )
    p2 = PermissionProfile(
        id="p2_write_campaign",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_fred",
        grants={
            PrivilegeGrant(privilege="edit_campaign", action=Action.WRITE),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(p1)
    service.upsert_profile(p2)

    assert service.has_permission("emp_fred", Action.WRITE, "edit_campaign", "emp_bob")


def test_profile_deletion_cleans_access(service):
    profile = PermissionProfile(
        id="prof_temp",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_charlie",
        grants={
            PrivilegeGrant(privilege="deploy_gateway", action=Action.WRITE),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile)
    assert service.has_permission("emp_charlie", Action.WRITE, "deploy_gateway", "emp_bob")

    service.delete_profile("prof_temp")
    assert not service.has_permission("emp_charlie", Action.WRITE, "deploy_gateway", "emp_bob")


def test_inactive_and_cross_company_boundaries(service):
    profile = PermissionProfile(
        id="prof_universal",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_alice",
        grants={
            PrivilegeGrant(privilege="view_profile", action=Action.READ),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile)

    assert service.has_permission("emp_alice", Action.READ, "view_profile", "emp_bob")
    assert not service.has_permission("emp_alice", Action.READ, "view_profile", "emp_erin")

    profile_erin = PermissionProfile(
        id="prof_erin",
        company_id="comp_rippling_demo",
        assignee_type="EMPLOYEE",
        assignee_id="emp_erin",
        grants={
            PrivilegeGrant(privilege="view_profile", action=Action.READ),
        },
        scope=Scope(ScopeType.FULL),
    )
    service.upsert_profile(profile_erin)
    assert not service.has_permission("emp_erin", Action.READ, "view_profile", "emp_bob")
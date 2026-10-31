import pytest
from employees.service import EmployeeService
from supergroups.models import Supergroup, GroupType, PolicyRule, FilterOperator
from supergroups.service import SupergroupService


@pytest.fixture
def sg_service():
    emp_svc = EmployeeService()
    return SupergroupService(emp_svc)


def test_static_supergroup_membership(sg_service):
    group = Supergroup(
        id="grp_founders",
        company_id="comp_rippling_demo",
        name="Founders",
        group_type=GroupType.STATIC,
        direct_member_ids={"emp_parker", "emp_macinnis", "emp_erin"},
    )
    sg_service.upsert_group(group)

    members = sg_service.get_effective_members("grp_founders")
    assert members == {"emp_parker", "emp_macinnis"}

    assert "grp_founders" in sg_service.get_user_groups("emp_parker")
    assert "grp_founders" in sg_service.get_user_groups("emp_macinnis")
    assert "grp_founders" not in sg_service.get_user_groups("emp_erin")


def test_policy_rule_threshold_comparison(sg_service):
    senior_staff_group = Supergroup(
        id="grp_senior_leaders",
        company_id="comp_rippling_demo",
        name="Senior Leaders",
        group_type=GroupType.POLICY,
        rules=[
            PolicyRule("level", FilterOperator.GREATER_THAN_OR_EQUAL, 5),
        ],
    )
    sg_service.upsert_group(senior_staff_group)

    members = sg_service.get_effective_members("grp_senior_leaders")
    expected_members = {"emp_alice", "emp_charlie", "emp_fred", "emp_parker", "emp_macinnis"}
    assert members == expected_members
    assert "grp_senior_leaders" in sg_service.get_user_groups("emp_charlie")
    assert "grp_senior_leaders" not in sg_service.get_user_groups("emp_bob")


def test_multi_rule_conjunction_policy(sg_service):
    eng_fte_group = Supergroup(
        id="grp_eng_fte",
        company_id="comp_rippling_demo",
        name="Engineering FTEs",
        group_type=GroupType.POLICY,
        rules=[
            PolicyRule("department", FilterOperator.EQUALS, "ENGINEERING"),
            PolicyRule("employment_type", FilterOperator.EQUALS, "FULL_TIME"),
        ],
    )
    sg_service.upsert_group(eng_fte_group)

    members = sg_service.get_effective_members("grp_eng_fte")
    assert members == {"emp_bob", "emp_charlie"}
    assert "emp_dan" not in members


def test_in_operator_rule_policy(sg_service):
    leadership_group = Supergroup(
        id="grp_exec_and_finance",
        company_id="comp_rippling_demo",
        name="Exec and Finance Leadership",
        group_type=GroupType.POLICY,
        rules=[
            PolicyRule("department", FilterOperator.IN, ["EXECUTIVE", "FINANCE"]),
            PolicyRule("level", FilterOperator.GREATER_THAN_OR_EQUAL, 5),
        ],
    )
    sg_service.upsert_group(leadership_group)

    members = sg_service.get_effective_members("grp_exec_and_finance")
    assert members == {"emp_alice", "emp_parker", "emp_macinnis"}


def test_inactive_group_resolves_empty(sg_service):
    group = Supergroup(
        id="grp_paused",
        company_id="comp_rippling_demo",
        name="Paused Initiative",
        group_type=GroupType.STATIC,
        is_active=False,
        direct_member_ids={"emp_alice", "emp_bob"},
    )
    sg_service.upsert_group(group)

    assert sg_service.get_effective_members("grp_paused") == set()
    assert "grp_paused" not in sg_service.get_user_groups("emp_alice")
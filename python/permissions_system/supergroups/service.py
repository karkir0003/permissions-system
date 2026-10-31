from collections import defaultdict
from typing import Dict, Set, List, Optional
from employees.service import EmployeeService
from employees.models import Employee
from supergroups.models import Supergroup, PolicyRule, FilterOperator


class SupergroupService:
    def __init__(self, employee_service: EmployeeService):
        self.employee_service = employee_service
        self._groups: Dict[str, Supergroup] = {}
        self._resolved_members: Dict[str, Set[str]] = {}
        self._user_to_groups: Dict[str, Set[str]] = defaultdict(set)

    def upsert_group(self, group: Supergroup) -> Set[str]:
        old_members = self._resolved_members.get(group.id, set())

        self._groups[group.id] = group
        new_members = self._compute_members(group)
        self._resolved_members[group.id] = new_members

        for uid in old_members - new_members:
            self._user_to_groups[uid].discard(group.id)
        for uid in new_members - old_members:
            self._user_to_groups[uid].add(group.id)

        return old_members.union(new_members)

    def get_group(self, group_id: str) -> Optional[Supergroup]:
        return self._groups.get(group_id)

    def get_effective_members(self, group_id: str) -> Set[str]:
        group = self._groups.get(group_id)
        if not group or not group.is_active:
            return set()
        return set(self._resolved_members.get(group_id, set()))

    def get_user_groups(self, user_id: str) -> Set[str]:
        return {
            gid for gid in self._user_to_groups.get(user_id, set())
            if self._groups.get(gid) and self._groups[gid].is_active
        }

    def _compute_members(self, group: Supergroup) -> Set[str]:
        if not group.is_active:
            return set()

        members = {
            uid for uid in group.direct_member_ids
            if self.employee_service.is_active(uid)
        }

        if group.rules:
            active_company_employees = self.employee_service.get_all_by_company(
                group.company_id, only_active=True
            )
            for emp in active_company_employees:
                if self._matches_all_rules(emp, group.rules):
                    members.add(emp.id)

        return members

    def _matches_all_rules(self, emp: Employee, rules: List[PolicyRule]) -> bool:
        for rule in rules:
            if not self._eval_rule(emp, rule):
                return False
        return True

    def _eval_rule(self, emp: Employee, rule: PolicyRule) -> bool:
        val = getattr(emp, rule.field_name, None)
        if val is None:
            val = emp.custom_fields.get(rule.field_name)

        if val is None:
            return False

        op = rule.operator
        target = rule.value

        if op == FilterOperator.EQUALS:
            return val == target
        elif op == FilterOperator.NOT_EQUALS:
            return val != target
        elif op == FilterOperator.IN:
            return val in target
        elif op == FilterOperator.GREATER_THAN_OR_EQUAL:
            return val <= target
        elif op == FilterOperator.LESS_THAN_OR_EQUAL:
            return val <= target
        return False
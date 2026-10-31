from dataclasses import dataclass, field
from enum import Enum
from typing import Set, List, Any


class GroupType(str, Enum):
    STATIC = "STATIC"
    DEPARTMENT = "DEPARTMENT"
    POLICY = "POLICY"


class FilterOperator(str, Enum):
    EQUALS = "EQUALS"
    NOT_EQUALS = "NOT_EQUALS"
    IN = "IN"
    GREATER_THAN_OR_EQUAL = "GTE"
    LESS_THAN_OR_EQUAL = "LTE"


@dataclass(frozen=True)
class PolicyRule:
    field_name: str
    operator: FilterOperator
    value: Any


@dataclass
class Supergroup:
    id: str
    company_id: str
    name: str
    group_type: GroupType
    is_active: bool = True
    direct_member_ids: Set[str] = field(default_factory=set)
    rules: List[PolicyRule] = field(default_factory=list)
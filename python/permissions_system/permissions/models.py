from dataclasses import dataclass, field
from enum import Enum
from typing import Set, Optional


class Action(str, Enum):
    READ = "READ"
    REQUEST = "REQUEST"
    WRITE = "WRITE"

    @property
    def level(self) -> int:
        levels = {
            Action.READ: 1,
            Action.REQUEST: 2,
            Action.WRITE: 3,
        }
        return levels[self]

    def subsumes(self, requested: "Action") -> bool:
        """Determines if this granted action satisfies the requested action level."""
        return self == requested


class ScopeType(str, Enum):
    FULL = "FULL"
    TARGET_GROUP = "GROUP"
    NONE = "NONE"


@dataclass(frozen=True)
class Scope:
    scope_type: ScopeType
    target_group_id: Optional[str] = None


@dataclass(frozen=True)
class PrivilegeGrant:
    privilege: str
    action: Action


@dataclass
class PermissionProfile:
    id: str
    company_id: str
    assignee_type: str
    assignee_id: str
    grants: Set[PrivilegeGrant] = field(default_factory=set)
    scope: Scope = field(default_factory=lambda: Scope(ScopeType.FULL))
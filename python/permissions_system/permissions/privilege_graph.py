from typing import Set
from permissions.models import PrivilegeGrant


class PrivilegeGraph:
    def expand_grants(self, base_grants: Set[PrivilegeGrant]) -> Set[PrivilegeGrant]:
        return set(base_grants)
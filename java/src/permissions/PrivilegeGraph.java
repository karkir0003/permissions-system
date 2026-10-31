package permissions;

import java.util.HashSet;
import java.util.Set;

public class PrivilegeGraph {
    public Set<PrivilegeGrant> expandGrants(Set<PrivilegeGrant> baseGrants) {
        return new HashSet<>(baseGrants);
    }
}

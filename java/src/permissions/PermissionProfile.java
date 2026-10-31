package permissions;

import java.util.*;

public class PermissionProfile {
    private final String id;
    private final String companyId;
    private final String assigneeType;
    private final String assigneeId;
    private final Set<PrivilegeGrant> grants;
    private Scope scope;

    public PermissionProfile(
        String id,
        String companyId,
        String assigneeType,
        String assigneeId,
        Set<PrivilegeGrant> grants,
        Scope scope
    ) {
        this.id = id;
        this.companyId = companyId;
        this.assigneeType = assigneeType;
        this.assigneeId = assigneeId;
        this.grants = grants != null ? new HashSet<>(grants) : new HashSet<>();
        this.scope = scope != null ? scope : new Scope(ScopeType.FULL);
    }

    public PermissionProfile(PermissionProfile other) {
        this(
            other.id,
            other.companyId,
            other.assigneeType,
            other.assigneeId,
            new HashSet<>(other.grants),
            other.scope != null ? new Scope(other.scope.getScopeType(), other.scope.getTargetGroupId()) : null
        );
    }

    public String getId() {
        return id;
    }

    public String getCompanyId() {
        return companyId;
    }

    public String getAssigneeType() {
        return assigneeType;
    }

    public String getAssigneeId() {
        return assigneeId;
    }

    public Set<PrivilegeGrant> getGrants() {
        return grants;
    }

    public Scope getScope() {
        return scope;
    }

    public void setScope(Scope scope) {
        this.scope = scope;
    }
}

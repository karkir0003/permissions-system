package permissions;

import java.util.Objects;

public class Scope {
    private final ScopeType scopeType;
    private final String targetGroupId;

    public Scope(ScopeType scopeType, String targetGroupId) {
        this.scopeType = scopeType;
        this.targetGroupId = targetGroupId;
    }

    public Scope(ScopeType scopeType) {
        this(scopeType, null);
    }

    public ScopeType getScopeType() {
        return scopeType;
    }

    public String getTargetGroupId() {
        return targetGroupId;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Scope)) return false;
        Scope scope = (Scope) o;
        return scopeType == scope.scopeType && Objects.equals(targetGroupId, scope.targetGroupId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(scopeType, targetGroupId);
    }
}

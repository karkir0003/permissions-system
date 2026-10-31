package permissions;

import java.util.Objects;

public class PrivilegeGrant {
    private final String privilege;
    private final Action action;

    public PrivilegeGrant(String privilege, Action action) {
        this.privilege = privilege;
        this.action = action;
    }

    public String getPrivilege() {
        return privilege;
    }

    public Action getAction() {
        return action;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof PrivilegeGrant)) return false;
        PrivilegeGrant that = (PrivilegeGrant) o;
        return Objects.equals(privilege, that.privilege) && action == that.action;
    }

    @Override
    public int hashCode() {
        return Objects.hash(privilege, action);
    }

    @Override
    public String toString() {
        return "PrivilegeGrant{" +
                "privilege='" + privilege + '\'' +
                ", action=" + action +
                '}';
    }
}

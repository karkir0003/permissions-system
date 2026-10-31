package supergroups;

import java.util.*;

public class Supergroup {
    private final String id;
    private final String companyId;
    private final String name;
    private final GroupType groupType;
    private boolean isActive;
    private final Set<String> directMemberIds;
    private final List<PolicyRule> rules;

    public Supergroup(
        String id,
        String companyId,
        String name,
        GroupType groupType,
        boolean isActive,
        Set<String> directMemberIds,
        List<PolicyRule> rules
    ) {
        this.id = id;
        this.companyId = companyId;
        this.name = name;
        this.groupType = groupType;
        this.isActive = isActive;
        this.directMemberIds = directMemberIds != null ? new HashSet<>(directMemberIds) : new HashSet<>();
        this.rules = rules != null ? new ArrayList<>(rules) : new ArrayList<>();
    }

    public Supergroup(String id, String companyId, String name, GroupType groupType) {
        this(id, companyId, name, groupType, true, Collections.emptySet(), Collections.emptyList());
    }

    public String getId() {
        return id;
    }

    public String getCompanyId() {
        return companyId;
    }

    public String getName() {
        return name;
    }

    public GroupType getGroupType() {
        return groupType;
    }

    public boolean isActive() {
        return isActive;
    }

    public void setActive(boolean active) {
        isActive = active;
    }

    public Set<String> getDirectMemberIds() {
        return directMemberIds;
    }

    public List<PolicyRule> getRules() {
        return rules;
    }
}

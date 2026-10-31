package supergroups;

import employees.Employee;
import employees.EmployeeService;

import java.util.*;

public class SupergroupService {
    private final EmployeeService employeeService;
    private final Map<String, Supergroup> groups = new HashMap<>();
    private final Map<String, Set<String>> resolvedMembers = new HashMap<>();
    private final Map<String, Set<String>> userToGroups = new HashMap<>();

    public SupergroupService(EmployeeService employeeService) {
        this.employeeService = employeeService;
    }

    public Set<String> upsertGroup(Supergroup group) {
        Set<String> oldMembers = resolvedMembers.getOrDefault(group.getId(), Collections.emptySet());

        groups.put(group.getId(), group);
        Set<String> newMembers = computeMembers(group);
        resolvedMembers.put(group.getId(), newMembers);

        for (String uid : oldMembers) {
            if (!newMembers.contains(uid)) {
                Set<String> userGroups = userToGroups.get(uid);
                if (userGroups != null) {
                    userGroups.remove(group.getId());
                }
            }
        }

        for (String uid : newMembers) {
            if (!oldMembers.contains(uid)) {
                userToGroups.computeIfAbsent(uid, k -> new HashSet<>()).add(group.getId());
            }
        }

        Set<String> union = new HashSet<>(oldMembers);
        union.addAll(newMembers);
        return union;
    }

    public Optional<Supergroup> getGroup(String groupId) {
        return Optional.ofNullable(groups.get(groupId));
    }

    public Set<String> getEffectiveMembers(String groupId) {
        Supergroup group = groups.get(groupId);
        if (group == null || !group.isActive()) {
            return Collections.emptySet();
        }
        return new HashSet<>(resolvedMembers.getOrDefault(groupId, Collections.emptySet()));
    }

    public Set<String> getUserGroups(String userId) {
        Set<String> groupIds = userToGroups.getOrDefault(userId, Collections.emptySet());
        Set<String> result = new HashSet<>();
        for (String gid : groupIds) {
            Supergroup g = groups.get(gid);
            if (g != null && g.isActive()) {
                result.add(gid);
            }
        }
        return result;
    }

    private Set<String> computeMembers(Supergroup group) {
        if (!group.isActive()) {
            return Collections.emptySet();
        }

        Set<String> members = new HashSet<>();
        for (String uid : group.getDirectMemberIds()) {
            if (employeeService.isActive(uid)) {
                members.add(uid);
            }
        }

        if (group.getRules() != null && !group.getRules().isEmpty()) {
            List<Employee> activeCompanyEmployees = employeeService.getAllByCompany(
                group.getCompanyId(),
                true
            );
            for (Employee emp : activeCompanyEmployees) {
                if (matchesAllRules(emp, group.getRules())) {
                    members.add(emp.getId());
                }
            }
        }

        return members;
    }

    private boolean matchesAllRules(Employee emp, List<PolicyRule> rules) {
        for (PolicyRule rule : rules) {
            if (!evalRule(emp, rule)) {
                return false;
            }
        }
        return true;
    }

    private boolean evalRule(Employee emp, PolicyRule rule) {
        Object val = getFieldValue(emp, rule.getFieldName());
        if (val == null) {
            return false;
        }

        FilterOperator op = rule.getOperator();
        Object target = rule.getValue();

        if (op == FilterOperator.EQUALS) {
            return Objects.equals(val, target);
        } else if (op == FilterOperator.NOT_EQUALS) {
            return !Objects.equals(val, target);
        } else if (op == FilterOperator.IN) {
            if (target instanceof Collection) {
                return ((Collection<?>) target).contains(val);
            }
            return false;
        } else if (op == FilterOperator.GREATER_THAN_OR_EQUAL) {
            if (val instanceof Number && target instanceof Number) {
                double v = ((Number) val).doubleValue();
                double t = ((Number) target).doubleValue();
                return v <= t;
            }
            return false;
        } else if (op == FilterOperator.LESS_THAN_OR_EQUAL) {
            if (val instanceof Number && target instanceof Number) {
                double v = ((Number) val).doubleValue();
                double t = ((Number) target).doubleValue();
                return v <= t;
            }
            return false;
        }
        return false;
    }

    private Object getFieldValue(Employee emp, String fieldName) {
        switch (fieldName) {
            case "id": return emp.getId();
            case "company_id": return emp.getCompanyId();
            case "department": return emp.getDepartment();
            case "title": return emp.getTitle();
            case "level": return emp.getLevel();
            case "employment_type": return emp.getEmploymentType();
            case "location": return emp.getLocation();
            case "manager_id": return emp.getManagerId();
            case "is_active": return emp.isActive();
            default:
                return emp.getCustomFields().get(fieldName);
        }
    }
}

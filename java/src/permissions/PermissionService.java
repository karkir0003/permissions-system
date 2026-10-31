package permissions;

import employees.Employee;
import employees.EmployeeService;
import supergroups.SupergroupService;

import java.util.*;

public class PermissionService {
    private final EmployeeService employeeService;
    private final SupergroupService supergroupService;
    private final PrivilegeGraph privilegeGraph;
    private final Map<String, PermissionProfile> profiles = new HashMap<>();

    public PermissionService(
        EmployeeService employeeService,
        SupergroupService supergroupService,
        PrivilegeGraph privilegeGraph
    ) {
        this.employeeService = employeeService;
        this.supergroupService = supergroupService;
        this.privilegeGraph = privilegeGraph;
    }

    public PermissionService(EmployeeService employeeService, SupergroupService supergroupService) {
        this(employeeService, supergroupService, null);
    }

    public void upsertProfile(PermissionProfile profile) {
        profiles.put(profile.getId(), profile);
    }

    public Optional<PermissionProfile> getProfile(String profileId) {
        PermissionProfile profile = profiles.get(profileId);
        if (profile == null) {
            return Optional.empty();
        }
        return Optional.of(new PermissionProfile(profile));
    }

    public void deleteProfile(String profileId) {
        profiles.remove(profileId);
    }

    public boolean hasPermission(
        String subjectEmployeeId,
        Action action,
        String privilege,
        String targetEmployeeId
    ) {
        if (!employeeService.isActive(subjectEmployeeId)) {
            return false;
        }
        if (!employeeService.isActive(targetEmployeeId)) {
            return false;
        }

        Optional<Employee> subjectOpt = employeeService.getEmployee(subjectEmployeeId);
        Optional<Employee> targetOpt = employeeService.getEmployee(targetEmployeeId);
        if (subjectOpt.isEmpty() || targetOpt.isEmpty()) {
            return false;
        }

        Employee subject = subjectOpt.get();
        Employee target = targetOpt.get();
        if (!Objects.equals(subject.getCompanyId(), target.getCompanyId())) {
            return false;
        }

        List<PermissionProfile> assignedProfiles = new ArrayList<>();
        for (PermissionProfile p : profiles.values()) {
            if ("EMPLOYEE".equals(p.getAssigneeType()) && Objects.equals(p.getAssigneeId(), subjectEmployeeId)) {
                assignedProfiles.add(p);
            }
        }

        for (PermissionProfile profile : assignedProfiles) {
            if (profile.getScope().getScopeType() == ScopeType.TARGET_GROUP) {
                continue;
            }

            for (PrivilegeGrant grant : profile.getGrants()) {
                if (Objects.equals(grant.getPrivilege(), privilege)) {
                    if (grant.getAction().subsumes(action)) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
        }

        return false;
    }
}

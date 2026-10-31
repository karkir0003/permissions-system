import employees.EmployeeService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import permissions.*;
import supergroups.SupergroupService;

import java.util.HashSet;
import java.util.Set;

import static org.junit.jupiter.api.Assertions.*;

public class TestPermissions {
    private PermissionService service;

    @BeforeEach
    public void setUp() {
        EmployeeService empSvc = new EmployeeService();
        SupergroupService sgSvc = new SupergroupService(empSvc);
        service = new PermissionService(empSvc, sgSvc);
    }

    @Test
    public void testActionSubsumptionHierarchy() {
        PermissionProfile profile = new PermissionProfile(
            "prof_exec_write",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_parker",
            Set.of(new PrivilegeGrant("manage_compensation", Action.WRITE)),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profile);

        assertTrue(service.hasPermission("emp_parker", Action.WRITE, "manage_compensation", "emp_bob"));
        assertTrue(service.hasPermission("emp_parker", Action.REQUEST, "manage_compensation", "emp_bob"));
        assertTrue(service.hasPermission("emp_parker", Action.READ, "manage_compensation", "emp_bob"));
    }

    @Test
    public void testActionHierarchyInsufficientLevel() {
        PermissionProfile profile = new PermissionProfile(
            "prof_finance_read",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_alice",
            Set.of(new PrivilegeGrant("view_payroll", Action.READ)),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profile);

        assertTrue(service.hasPermission("emp_alice", Action.READ, "view_payroll", "emp_bob"));
        assertFalse(service.hasPermission("emp_alice", Action.REQUEST, "view_payroll", "emp_bob"));
        assertFalse(service.hasPermission("emp_alice", Action.WRITE, "view_payroll", "emp_bob"));
    }

    @Test
    public void testScopeNoneExplicitDenial() {
        PermissionProfile profile = new PermissionProfile(
            "prof_disabled",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_macinnis",
            Set.of(new PrivilegeGrant("delete_user", Action.WRITE)),
            new Scope(ScopeType.NONE)
        );
        service.upsertProfile(profile);

        assertFalse(service.hasPermission("emp_macinnis", Action.WRITE, "delete_user", "emp_dan"));
    }

    @Test
    public void testMultipleGrantsWithinSameProfile() {
        PermissionProfile profile = new PermissionProfile(
            "prof_lead_ops",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_alice",
            Set.of(
                new PrivilegeGrant("view_contracts", Action.READ),
                new PrivilegeGrant("approve_expenses", Action.WRITE)
            ),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profile);

        assertTrue(service.hasPermission("emp_alice", Action.READ, "view_contracts", "emp_dan"));
        assertFalse(service.hasPermission("emp_alice", Action.WRITE, "view_contracts", "emp_dan"));
        assertTrue(service.hasPermission("emp_alice", Action.WRITE, "approve_expenses", "emp_dan"));
    }

    @Test
    public void testGetProfileAndUpdateLifecycle() {
        Set<PrivilegeGrant> grants = new HashSet<>();
        grants.add(new PrivilegeGrant("view_payroll", Action.READ));
        PermissionProfile initialProfile = new PermissionProfile(
            "prof_payroll_lead",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_alice",
            grants,
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(initialProfile);

        PermissionProfile retrieved = service.getProfile("prof_payroll_lead").orElse(null);
        assertNotNull(retrieved);
        assertEquals("prof_payroll_lead", retrieved.getId());
        assertEquals("emp_alice", retrieved.getAssigneeId());
        assertEquals(1, retrieved.getGrants().size());

        retrieved.getGrants().add(new PrivilegeGrant("edit_payroll", Action.WRITE));
        service.upsertProfile(retrieved);

        PermissionProfile updated = service.getProfile("prof_payroll_lead").orElse(null);
        assertNotNull(updated);
        assertEquals(2, updated.getGrants().size());
        assertTrue(service.hasPermission("emp_alice", Action.WRITE, "edit_payroll", "emp_bob"));

        assertTrue(service.getProfile("prof_nonexistent").isEmpty());
    }

    @Test
    public void testGetProfileReturnsDefensiveCopy() {
        Set<PrivilegeGrant> grants = new HashSet<>();
        grants.add(new PrivilegeGrant("view_contracts", Action.READ));
        PermissionProfile profile = new PermissionProfile(
            "prof_read_only",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_alice",
            grants,
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profile);

        PermissionProfile fetched = service.getProfile("prof_read_only").orElse(null);
        assertNotNull(fetched);

        fetched.getGrants().add(new PrivilegeGrant("nuke_database", Action.WRITE));

        PermissionProfile internal = service.getProfile("prof_read_only").orElse(null);
        assertNotNull(internal);
        assertEquals(1, internal.getGrants().size());
        assertFalse(service.hasPermission("emp_alice", Action.WRITE, "nuke_database", "emp_bob"));
    }

    @Test
    public void testMultipleProfilesCumulativeEvaluation() {
        PermissionProfile p1 = new PermissionProfile(
            "p1_read_audit",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_fred",
            Set.of(new PrivilegeGrant("edit_campaign", Action.READ)),
            new Scope(ScopeType.FULL)
        );
        PermissionProfile p2 = new PermissionProfile(
            "p2_write_campaign",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_fred",
            Set.of(new PrivilegeGrant("edit_campaign", Action.WRITE)),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(p1);
        service.upsertProfile(p2);

        assertTrue(service.hasPermission("emp_fred", Action.WRITE, "edit_campaign", "emp_bob"));
    }

    @Test
    public void testProfileDeletionCleansAccess() {
        PermissionProfile profile = new PermissionProfile(
            "prof_temp",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_charlie",
            Set.of(new PrivilegeGrant("deploy_gateway", Action.WRITE)),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profile);
        assertTrue(service.hasPermission("emp_charlie", Action.WRITE, "deploy_gateway", "emp_bob"));

        service.deleteProfile("prof_temp");
        assertFalse(service.hasPermission("emp_charlie", Action.WRITE, "deploy_gateway", "emp_bob"));
    }

    @Test
    public void testInactiveAndCrossCompanyBoundaries() {
        PermissionProfile profile = new PermissionProfile(
            "prof_universal",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_alice",
            Set.of(new PrivilegeGrant("view_profile", Action.READ)),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profile);

        assertTrue(service.hasPermission("emp_alice", Action.READ, "view_profile", "emp_bob"));
        assertFalse(service.hasPermission("emp_alice", Action.READ, "view_profile", "emp_erin"));

        PermissionProfile profileErin = new PermissionProfile(
            "prof_erin",
            "comp_rippling_demo",
            "EMPLOYEE",
            "emp_erin",
            Set.of(new PrivilegeGrant("view_profile", Action.READ)),
            new Scope(ScopeType.FULL)
        );
        service.upsertProfile(profileErin);
        assertFalse(service.hasPermission("emp_erin", Action.READ, "view_profile", "emp_bob"));
    }
}

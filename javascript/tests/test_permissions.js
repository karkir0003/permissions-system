const assert = require("assert");
const { EmployeeService } = require("../employees");
const { SupergroupService } = require("../supergroups");
const {
  Action,
  Scope,
  ScopeType,
  PrivilegeGrant,
  PermissionProfile,
  PermissionService,
} = require("../permissions");

function createService() {
  const empSvc = new EmployeeService();
  const sgSvc = new SupergroupService(empSvc);
  return new PermissionService(empSvc, sgSvc);
}

function testActionSubsumptionHierarchy() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_exec_write",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_parker",
    grants: new Set([new PrivilegeGrant("manage_compensation", Action.WRITE)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profile);

  assert.strictEqual(
    service.hasPermission("emp_parker", Action.WRITE, "manage_compensation", "emp_bob"),
    true
  );
  assert.strictEqual(
    service.hasPermission("emp_parker", Action.REQUEST, "manage_compensation", "emp_bob"),
    true
  );
  assert.strictEqual(
    service.hasPermission("emp_parker", Action.READ, "manage_compensation", "emp_bob"),
    true
  );
}

function testActionHierarchyInsufficientLevel() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_finance_read",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_alice",
    grants: new Set([new PrivilegeGrant("view_payroll", Action.READ)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profile);

  assert.strictEqual(
    service.hasPermission("emp_alice", Action.READ, "view_payroll", "emp_bob"),
    true
  );
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.REQUEST, "view_payroll", "emp_bob"),
    false
  );
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.WRITE, "view_payroll", "emp_bob"),
    false
  );
}

function testScopeNoneExplicitDenial() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_disabled",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_macinnis",
    grants: new Set([new PrivilegeGrant("delete_user", Action.WRITE)]),
    scope: new Scope(ScopeType.NONE),
  });
  service.upsertProfile(profile);

  assert.strictEqual(
    service.hasPermission("emp_macinnis", Action.WRITE, "delete_user", "emp_dan"),
    false
  );
}

function testMultipleGrantsWithinSameProfile() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_lead_ops",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_alice",
    grants: new Set([
      new PrivilegeGrant("view_contracts", Action.READ),
      new PrivilegeGrant("approve_expenses", Action.WRITE),
    ]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profile);

  assert.strictEqual(
    service.hasPermission("emp_alice", Action.READ, "view_contracts", "emp_dan"),
    true
  );
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.WRITE, "view_contracts", "emp_dan"),
    false
  );
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.WRITE, "approve_expenses", "emp_dan"),
    true
  );
}

function testGetProfileAndUpdateLifecycle() {
  const service = createService();
  const initialProfile = new PermissionProfile({
    id: "prof_payroll_lead",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_alice",
    grants: new Set([new PrivilegeGrant("view_payroll", Action.READ)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(initialProfile);

  const retrieved = service.getProfile("prof_payroll_lead");
  assert(retrieved !== null);
  assert.strictEqual(retrieved.id, "prof_payroll_lead");
  assert.strictEqual(retrieved.assignee_id, "emp_alice");
  assert.strictEqual(retrieved.grants.size, 1);

  retrieved.grants.add(new PrivilegeGrant("edit_payroll", Action.WRITE));
  service.upsertProfile(retrieved);

  const updated = service.getProfile("prof_payroll_lead");
  assert.strictEqual(updated.grants.size, 2);
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.WRITE, "edit_payroll", "emp_bob"),
    true
  );

  assert.strictEqual(service.getProfile("prof_nonexistent"), null);
}

function testGetProfileReturnsDefensiveCopy() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_read_only",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_alice",
    grants: new Set([new PrivilegeGrant("view_contracts", Action.READ)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profile);

  const fetched = service.getProfile("prof_read_only");
  assert(fetched !== null);

  fetched.grants.add(new PrivilegeGrant("nuke_database", Action.WRITE));

  const internal = service.getProfile("prof_read_only");
  assert.strictEqual(internal.grants.size, 1);
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.WRITE, "nuke_database", "emp_bob"),
    false
  );
}

function testMultipleProfilesCumulativeEvaluation() {
  const service = createService();
  const p1 = new PermissionProfile({
    id: "p1_read_audit",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_fred",
    grants: new Set([new PrivilegeGrant("edit_campaign", Action.READ)]),
    scope: new Scope(ScopeType.FULL),
  });
  const p2 = new PermissionProfile({
    id: "p2_write_campaign",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_fred",
    grants: new Set([new PrivilegeGrant("edit_campaign", Action.WRITE)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(p1);
  service.upsertProfile(p2);

  assert.strictEqual(
    service.hasPermission("emp_fred", Action.WRITE, "edit_campaign", "emp_bob"),
    true
  );
}

function testProfileDeletionCleansAccess() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_temp",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_charlie",
    grants: new Set([new PrivilegeGrant("deploy_gateway", Action.WRITE)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profile);
  assert.strictEqual(
    service.hasPermission("emp_charlie", Action.WRITE, "deploy_gateway", "emp_bob"),
    true
  );

  service.deleteProfile("prof_temp");
  assert.strictEqual(
    service.hasPermission("emp_charlie", Action.WRITE, "deploy_gateway", "emp_bob"),
    false
  );
}

function testInactiveAndCrossCompanyBoundaries() {
  const service = createService();
  const profile = new PermissionProfile({
    id: "prof_universal",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_alice",
    grants: new Set([new PrivilegeGrant("view_profile", Action.READ)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profile);

  assert.strictEqual(
    service.hasPermission("emp_alice", Action.READ, "view_profile", "emp_bob"),
    true
  );
  assert.strictEqual(
    service.hasPermission("emp_alice", Action.READ, "view_profile", "emp_erin"),
    false
  );

  const profileErin = new PermissionProfile({
    id: "prof_erin",
    company_id: "comp_rippling_demo",
    assignee_type: "EMPLOYEE",
    assignee_id: "emp_erin",
    grants: new Set([new PrivilegeGrant("view_profile", Action.READ)]),
    scope: new Scope(ScopeType.FULL),
  });
  service.upsertProfile(profileErin);
  assert.strictEqual(
    service.hasPermission("emp_erin", Action.READ, "view_profile", "emp_bob"),
    false
  );
}

module.exports = {
  tests: [
    { name: "test_action_subsumption_hierarchy", fn: testActionSubsumptionHierarchy },
    { name: "test_action_hierarchy_insufficient_level", fn: testActionHierarchyInsufficientLevel },
    { name: "test_scope_none_explicit_denial", fn: testScopeNoneExplicitDenial },
    { name: "test_multiple_grants_within_same_profile", fn: testMultipleGrantsWithinSameProfile },
    { name: "test_get_profile_and_update_lifecycle", fn: testGetProfileAndUpdateLifecycle },
    { name: "test_get_profile_returns_defensive_copy", fn: testGetProfileReturnsDefensiveCopy },
    { name: "test_multiple_profiles_cumulative_evaluation", fn: testMultipleProfilesCumulativeEvaluation },
    { name: "test_profile_deletion_cleans_access", fn: testProfileDeletionCleansAccess },
    { name: "test_inactive_and_cross_company_boundaries", fn: testInactiveAndCrossCompanyBoundaries },
  ],
};

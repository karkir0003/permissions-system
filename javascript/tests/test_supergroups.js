const assert = require("assert");
const { EmployeeService } = require("../employees");
const { Supergroup, GroupType, PolicyRule, FilterOperator, SupergroupService } = require("../supergroups");

function areSetsEqual(a, b) {
  if (a.size !== b.size) return false;
  for (const item of a) {
    if (!b.has(item)) return false;
  }
  return true;
}

function testStaticSupergroupMembership() {
  const empSvc = new EmployeeService();
  const sgService = new SupergroupService(empSvc);

  const group = new Supergroup({
    id: "grp_founders",
    company_id: "comp_rippling_demo",
    name: "Founders",
    group_type: GroupType.STATIC,
    direct_member_ids: new Set(["emp_parker", "emp_macinnis", "emp_erin"]),
  });
  sgService.upsertGroup(group);

  const members = sgService.getEffectiveMembers("grp_founders");
  const expected = new Set(["emp_parker", "emp_macinnis"]);
  assert(areSetsEqual(members, expected), `Expected ${[...expected]}, got ${[...members]}`);

  assert(sgService.getUserGroups("emp_parker").has("grp_founders"));
  assert(sgService.getUserGroups("emp_macinnis").has("grp_founders"));
  assert(!sgService.getUserGroups("emp_erin").has("grp_founders"));
}

function testPolicyRuleThresholdComparison() {
  const empSvc = new EmployeeService();
  const sgService = new SupergroupService(empSvc);

  const seniorStaffGroup = new Supergroup({
    id: "grp_senior_leaders",
    company_id: "comp_rippling_demo",
    name: "Senior Leaders",
    group_type: GroupType.POLICY,
    rules: [
      new PolicyRule("level", FilterOperator.GREATER_THAN_OR_EQUAL, 5),
    ],
  });
  sgService.upsertGroup(seniorStaffGroup);

  const members = sgService.getEffectiveMembers("grp_senior_leaders");
  const expectedMembers = new Set(["emp_alice", "emp_charlie", "emp_fred", "emp_parker", "emp_macinnis"]);
  assert(areSetsEqual(members, expectedMembers), `Expected ${[...expectedMembers]}, got ${[...members]}`);
  assert(sgService.getUserGroups("emp_charlie").has("grp_senior_leaders"));
  assert(!sgService.getUserGroups("emp_bob").has("grp_senior_leaders"));
}

function testMultiRuleConjunctionPolicy() {
  const empSvc = new EmployeeService();
  const sgService = new SupergroupService(empSvc);

  const engFteGroup = new Supergroup({
    id: "grp_eng_fte",
    company_id: "comp_rippling_demo",
    name: "Engineering FTEs",
    group_type: GroupType.POLICY,
    rules: [
      new PolicyRule("department", FilterOperator.EQUALS, "ENGINEERING"),
      new PolicyRule("employment_type", FilterOperator.EQUALS, "FULL_TIME"),
    ],
  });
  sgService.upsertGroup(engFteGroup);

  const members = sgService.getEffectiveMembers("grp_eng_fte");
  const expected = new Set(["emp_bob", "emp_charlie"]);
  assert(areSetsEqual(members, expected));
  assert(!members.has("emp_dan"));
}

function testInOperatorRulePolicy() {
  const empSvc = new EmployeeService();
  const sgService = new SupergroupService(empSvc);

  const leadershipGroup = new Supergroup({
    id: "grp_exec_and_finance",
    company_id: "comp_rippling_demo",
    name: "Exec and Finance Leadership",
    group_type: GroupType.POLICY,
    rules: [
      new PolicyRule("department", FilterOperator.IN, ["EXECUTIVE", "FINANCE"]),
      new PolicyRule("level", FilterOperator.GREATER_THAN_OR_EQUAL, 5),
    ],
  });
  sgService.upsertGroup(leadershipGroup);

  const members = sgService.getEffectiveMembers("grp_exec_and_finance");
  const expected = new Set(["emp_alice", "emp_parker", "emp_macinnis"]);
  assert(areSetsEqual(members, expected), `Expected ${[...expected]}, got ${[...members]}`);
}

function testInactiveGroupResolvesEmpty() {
  const empSvc = new EmployeeService();
  const sgService = new SupergroupService(empSvc);

  const group = new Supergroup({
    id: "grp_paused",
    company_id: "comp_rippling_demo",
    name: "Paused Initiative",
    group_type: GroupType.STATIC,
    is_active: false,
    direct_member_ids: new Set(["emp_alice", "emp_bob"]),
  });
  sgService.upsertGroup(group);

  assert.strictEqual(sgService.getEffectiveMembers("grp_paused").size, 0);
  assert(!sgService.getUserGroups("emp_alice").has("grp_paused"));
}

module.exports = {
  tests: [
    { name: "test_static_supergroup_membership", fn: testStaticSupergroupMembership },
    { name: "test_policy_rule_threshold_comparison", fn: testPolicyRuleThresholdComparison },
    { name: "test_multi_rule_conjunction_policy", fn: testMultiRuleConjunctionPolicy },
    { name: "test_in_operator_rule_policy", fn: testInOperatorRulePolicy },
    { name: "test_inactive_group_resolves_empty", fn: testInactiveGroupResolvesEmpty },
  ],
};

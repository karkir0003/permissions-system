const assert = require("assert");
const { Employee, EmployeeService } = require("../employees");

function testSeedDirectoryIntegrity() {
  const empService = new EmployeeService();
  const parker = empService.getEmployee("emp_parker");
  assert(parker !== null);
  assert.strictEqual(parker.department, "EXECUTIVE");
  assert.strictEqual(parker.level, 10);
  assert.strictEqual(parker.is_active, true);

  const dan = empService.getEmployee("emp_dan");
  assert(dan !== null);
  assert.strictEqual(dan.employment_type, "CONTRACTOR");
  assert.strictEqual(dan.location, "IN");

  assert.strictEqual(empService.getEmployee("emp_nonexistent"), null);
}

function testIsActiveCheck() {
  const empService = new EmployeeService();
  assert.strictEqual(empService.isActive("emp_alice"), true);
  assert.strictEqual(empService.isActive("emp_macinnis"), true);
  assert.strictEqual(empService.isActive("emp_erin"), false);
  assert.strictEqual(empService.isActive("emp_does_not_exist"), false);
}

function testGetAllByCompanyActiveFiltering() {
  const empService = new EmployeeService();
  const allCompanyEmployees = empService.getAllByCompany("comp_rippling_demo", false);
  assert.strictEqual(allCompanyEmployees.length, 8);

  const activeCompanyEmployees = empService.getAllByCompany("comp_rippling_demo", true);
  const activeIds = new Set(activeCompanyEmployees.map((emp) => emp.id));

  assert.strictEqual(activeCompanyEmployees.length, 7);
  assert(!activeIds.has("emp_erin"));
  assert(activeIds.has("emp_alice"));
  assert(activeIds.has("emp_parker"));

  const emptyCompany = empService.getAllByCompany("comp_other", true);
  assert.strictEqual(emptyCompany.length, 0);
}

function testUpsertNewHireAndCrossCompany() {
  const empService = new EmployeeService();
  const newHire = new Employee({
    id: "emp_zack",
    company_id: "comp_external_org",
    department: "LEGAL",
    title: "General Counsel",
    level: 6,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  });
  empService.upsertEmployee(newHire);

  assert.strictEqual(empService.isActive("emp_zack"), true);
  assert.strictEqual(empService.getEmployee("emp_zack").department, "LEGAL");

  const demoEmps = empService.getAllByCompany("comp_rippling_demo", false);
  const demoIds = new Set(demoEmps.map((e) => e.id));
  assert(!demoIds.has("emp_zack"));
}

function testDeactivateEmployeeLifecycle() {
  const empService = new EmployeeService();
  assert.strictEqual(empService.isActive("emp_fred"), true);

  empService.deactivateEmployee("emp_fred");

  assert.strictEqual(empService.isActive("emp_fred"), false);
  const fred = empService.getEmployee("emp_fred");
  assert.strictEqual(fred.is_active, false);
}

module.exports = {
  tests: [
    { name: "test_seed_directory_integrity", fn: testSeedDirectoryIntegrity },
    { name: "test_is_active_check", fn: testIsActiveCheck },
    { name: "test_get_all_by_company_active_filtering", fn: testGetAllByCompanyActiveFiltering },
    { name: "test_upsert_new_hire_and_cross_company", fn: testUpsertNewHireAndCrossCompany },
    { name: "test_deactivate_employee_lifecycle", fn: testDeactivateEmployeeLifecycle },
  ],
};

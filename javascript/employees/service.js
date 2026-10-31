const { Employee } = require("./models");

const SEEDED_EMPLOYEES = [
  new Employee({
    id: "emp_alice",
    company_id: "comp_rippling_demo",
    department: "FINANCE",
    title: "Payroll Operations Lead",
    level: 5,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  }),
  new Employee({
    id: "emp_bob",
    company_id: "comp_rippling_demo",
    department: "ENGINEERING",
    title: "Senior Software Engineer",
    level: 4,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  }),
  new Employee({
    id: "emp_charlie",
    company_id: "comp_rippling_demo",
    department: "ENGINEERING",
    title: "Staff Software Engineer",
    level: 5,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  }),
  new Employee({
    id: "emp_dan",
    company_id: "comp_rippling_demo",
    department: "ENGINEERING",
    title: "Frontend Contractor",
    level: 3,
    employment_type: "CONTRACTOR",
    location: "IN",
    is_active: true,
  }),
  new Employee({
    id: "emp_erin",
    company_id: "comp_rippling_demo",
    department: "FINANCE",
    title: "Analyst",
    level: 3,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: false,
  }),
  new Employee({
    id: "emp_fred",
    company_id: "comp_rippling_demo",
    department: "MARKETING",
    title: "Marketing Manager",
    level: 5,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  }),
  new Employee({
    id: "emp_parker",
    company_id: "comp_rippling_demo",
    department: "EXECUTIVE",
    title: "CEO",
    level: 10,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  }),
  new Employee({
    id: "emp_macinnis",
    company_id: "comp_rippling_demo",
    department: "EXECUTIVE",
    title: "CPO",
    level: 10,
    employment_type: "FULL_TIME",
    location: "US",
    is_active: true,
  }),
];

class EmployeeService {
  constructor(initialData = null) {
    this._db = new Map();
    const data = initialData !== null ? initialData : SEEDED_EMPLOYEES;
    for (const emp of data) {
      this._db.set(emp.id, emp);
    }
  }

  getEmployee(employeeId) {
    return this._db.get(employeeId) || null;
  }

  isActive(employeeId) {
    const emp = this.getEmployee(employeeId);
    return emp !== null && emp.is_active === true;
  }

  getAllByCompany(companyId, onlyActive = true) {
    return Array.from(this._db.values()).filter((emp) => emp.company_id === companyId);
  }

  listAll() {
    return Array.from(this._db.values());
  }

  upsertEmployee(employee) {
    this._db.set(employee.id, employee);
  }

  deactivateEmployee(employeeId) {
    const emp = this._db.get(employeeId);
    if (emp) {
      emp.is_active = false;
    }
  }
}

module.exports = { SEEDED_EMPLOYEES, EmployeeService };

class Employee {
  constructor({
    id,
    company_id,
    department,
    title,
    level,
    employment_type,
    location,
    manager_id = null,
    is_active = true,
    custom_fields = {},
  }) {
    this.id = id;
    this.company_id = company_id;
    this.department = department;
    this.title = title;
    this.level = level;
    this.employment_type = employment_type;
    this.location = location;
    this.manager_id = manager_id;
    this.is_active = is_active;
    this.custom_fields = { ...custom_fields };
  }
}

module.exports = { Employee };

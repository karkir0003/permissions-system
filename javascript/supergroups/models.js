const GroupType = Object.freeze({
  STATIC: "STATIC",
  DEPARTMENT: "DEPARTMENT",
  POLICY: "POLICY",
});

const FilterOperator = Object.freeze({
  EQUALS: "EQUALS",
  NOT_EQUALS: "NOT_EQUALS",
  IN: "IN",
  GREATER_THAN_OR_EQUAL: "GTE",
  LESS_THAN_OR_EQUAL: "LTE",
});

class PolicyRule {
  constructor(fieldName, operator, value) {
    this.fieldName = fieldName;
    this.operator = operator;
    this.value = value;
  }
}

class Supergroup {
  constructor({
    id,
    company_id,
    name,
    group_type,
    is_active = true,
    direct_member_ids = new Set(),
    rules = [],
  }) {
    this.id = id;
    this.company_id = company_id;
    this.name = name;
    this.group_type = group_type;
    this.is_active = is_active;
    this.direct_member_ids = new Set(direct_member_ids);
    this.rules = [...rules];
  }
}

module.exports = {
  GroupType,
  FilterOperator,
  PolicyRule,
  Supergroup,
};

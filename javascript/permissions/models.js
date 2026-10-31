class Action {
  static READ = new Action("READ", 1);
  static REQUEST = new Action("REQUEST", 2);
  static WRITE = new Action("WRITE", 3);

  constructor(name, level) {
    this.name = name;
    this.level = level;
  }

  subsumes(requested) {
    return this === requested;
  }

  toString() {
    return this.name;
  }
}

const ScopeType = Object.freeze({
  FULL: "FULL",
  TARGET_GROUP: "GROUP",
  NONE: "NONE",
});

class Scope {
  constructor(scopeType, targetGroupId = null) {
    this.scopeType = scopeType;
    this.targetGroupId = targetGroupId;
  }
}

class PrivilegeGrant {
  constructor(privilege, action) {
    this.privilege = privilege;
    this.action = action;
  }
}

class PermissionProfile {
  constructor({
    id,
    company_id,
    assignee_type,
    assignee_id,
    grants = new Set(),
    scope = new Scope(ScopeType.FULL),
  }) {
    this.id = id;
    this.company_id = company_id;
    this.assignee_type = assignee_type;
    this.assignee_id = assignee_id;
    this.grants = new Set(grants);
    this.scope = scope;
  }
}

module.exports = {
  Action,
  ScopeType,
  Scope,
  PrivilegeGrant,
  PermissionProfile,
};

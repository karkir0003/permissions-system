const { GroupType, FilterOperator } = require("./models");

class SupergroupService {
  constructor(employeeService) {
    this.employeeService = employeeService;
    this._groups = new Map();
    this._resolvedMembers = new Map();
    this._userToGroups = new Map();
  }

  upsertGroup(group) {
    const oldMembers = this._resolvedMembers.get(group.id) || new Set();

    this._groups.set(group.id, group);
    const newMembers = this._computeMembers(group);
    this._resolvedMembers.set(group.id, newMembers);

    for (const uid of oldMembers) {
      if (!newMembers.has(uid)) {
        const userGroups = this._userToGroups.get(uid);
        if (userGroups) {
          userGroups.delete(group.id);
        }
      }
    }

    for (const uid of newMembers) {
      if (!oldMembers.has(uid)) {
        if (!this._userToGroups.has(uid)) {
          this._userToGroups.set(uid, new Set());
        }
        this._userToGroups.get(uid).add(group.id);
      }
    }

    const union = new Set([...oldMembers, ...newMembers]);
    return union;
  }

  getGroup(groupId) {
    return this._groups.get(groupId) || null;
  }

  getEffectiveMembers(groupId) {
    const group = this._groups.get(groupId);
    if (!group || !group.is_active) {
      return new Set();
    }
    return new Set(this._resolvedMembers.get(groupId) || []);
  }

  getUserGroups(userId) {
    const userGroups = this._userToGroups.get(userId) || new Set();
    const result = new Set();
    for (const gid of userGroups) {
      const g = this._groups.get(gid);
      if (g && g.is_active) {
        result.add(gid);
      }
    }
    return result;
  }

  _computeMembers(group) {
    if (!group.is_active) {
      return new Set();
    }

    const members = new Set();
    for (const uid of group.direct_member_ids) {
      if (this.employeeService.isActive(uid)) {
        members.add(uid);
      }
    }

    if (group.rules && group.rules.length > 0) {
      const activeCompanyEmployees = this.employeeService.getAllByCompany(
        group.company_id,
        true
      );
      for (const emp of activeCompanyEmployees) {
        if (this._matchesAllRules(emp, group.rules)) {
          members.add(emp.id);
        }
      }
    }

    return members;
  }

  _matchesAllRules(emp, rules) {
    for (const rule of rules) {
      if (!this._evalRule(emp, rule)) {
        return false;
      }
    }
    return true;
  }

  _evalRule(emp, rule) {
    let val = emp[rule.fieldName];
    if (val === undefined && emp.custom_fields) {
      val = emp.custom_fields[rule.fieldName];
    }
    if (val === undefined || val === null) {
      return false;
    }

    const op = rule.operator;
    const target = rule.value;

    if (op === FilterOperator.EQUALS) {
      return val === target;
    } else if (op === FilterOperator.NOT_EQUALS) {
      return val !== target;
    } else if (op === FilterOperator.IN) {
      if (Array.isArray(target) || typeof target === "string" || target instanceof Set) {
        return Array.isArray(target) ? target.includes(val) : (target instanceof Set ? target.has(val) : target.includes(val));
      }
      return false;
    } else if (op === FilterOperator.GREATER_THAN_OR_EQUAL) {
      return val <= target;
    } else if (op === FilterOperator.LESS_THAN_OR_EQUAL) {
      return val <= target;
    }
    return false;
  }
}

module.exports = { SupergroupService };

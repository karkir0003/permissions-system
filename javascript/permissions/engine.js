const { Action, ScopeType, Scope, PrivilegeGrant, PermissionProfile } = require("./models");

class PermissionService {
  constructor(employeeService, supergroupService, privilegeGraph = null) {
    this.employeeService = employeeService;
    this.supergroupService = supergroupService;
    this.privilegeGraph = privilegeGraph;
    this._profiles = new Map();
  }

  upsertProfile(profile) {
    this._profiles.set(profile.id, profile);
  }

  getProfile(profileId) {
    const profile = this._profiles.get(profileId);
    if (!profile) {
      return null;
    }
    // Defensive copy
    const clonedGrants = new Set(
      Array.from(profile.grants).map((g) => new PrivilegeGrant(g.privilege, g.action))
    );
    return new PermissionProfile({
      id: profile.id,
      company_id: profile.company_id,
      assignee_type: profile.assignee_type,
      assignee_id: profile.assignee_id,
      grants: clonedGrants,
      scope: new Scope(profile.scope.scopeType, profile.scope.targetGroupId),
    });
  }

  deleteProfile(profileId) {
    this._profiles.delete(profileId);
  }

  hasPermission(subjectEmployeeId, action, privilege, targetEmployeeId) {
    if (!this.employeeService.isActive(subjectEmployeeId)) {
      return false;
    }
    if (!this.employeeService.isActive(targetEmployeeId)) {
      return false;
    }

    const subject = this.employeeService.getEmployee(subjectEmployeeId);
    const target = this.employeeService.getEmployee(targetEmployeeId);
    if (subject.company_id !== target.company_id) {
      return false;
    }

    const assignedProfiles = Array.from(this._profiles.values()).filter(
      (p) => p.assignee_type === "EMPLOYEE" && p.assignee_id === subjectEmployeeId
    );

    for (const profile of assignedProfiles) {
      if (profile.scope.scopeType === ScopeType.TARGET_GROUP) {
        continue;
      }

      for (const grant of profile.grants) {
        if (grant.privilege === privilege) {
          if (grant.action.subsumes(action)) {
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

module.exports = { PermissionService };

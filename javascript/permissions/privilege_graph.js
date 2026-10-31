class PrivilegeGraph {
  expandGrants(baseGrants) {
    return new Set(baseGrants);
  }
}

module.exports = { PrivilegeGraph };

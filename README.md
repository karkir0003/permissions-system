# Permissions Management System - AI Assisted Design

Welcome to the **Permissions Management System** design round!

You will be working on a permission management service for an HR Tech product like Rippling. This system allows for company admins to grant privileges to employees based on the business use case. The data will live in-memory for now so we can focus on the core ideas of what such a system entails.

You are given a prototype of such a service and we will be improving it over the course of a series of milestones.

### Quick Start Instructions

1. **Choose your language** and change into its directory:
   ```bash
   cd python        # For Python (pytest / unittest)
   cd javascript    # For JavaScript (Node.js)
   cd java          # For Java (JUnit 5)
   cd c++           # For C++ (Google Test)
   ```

2. **Verify / install environment** (optional):
   ```bash
   make setup
   ```
   *(Checks compilers/runtimes and automatically installs any missing dependencies).*

3. **Run your tests**:
   ```bash
   make
   ```
   *(Runs the test suite).*

4. **Clean artifacts** (optional):
   ```bash
   make clean
   ```

---

## Directory & File Reference

| Language | Directory | Test Framework | Source Package / File | Test Suite |
| :--- | :--- | :--- | :--- | :--- |
| **Python** | `python/` | `pytest` (uv / standard runner) | [`python/permissions_system/...`](python/permissions_system/) | [`python/permissions_system/tests/`](python/permissions_system/tests/) |
| **JavaScript** | `javascript/` | Built-in Node assertions (zero dependencies) | [`javascript/...`](javascript/) | [`javascript/tests/`](javascript/tests/) |
| **Java** | `java/` | `JUnit 5` (bundled standalone runner) | [`java/src/...`](java/src/) | [`java/tests/`](java/tests/) |
| **C++** | `c++/` | `Google Test` (auto-provisioned via setup.sh) | [`c++/include/...`](c++/include/) | [`c++/tests/`](c++/tests/) |

Every language directory contains a standardized `Makefile` with identical targets (`make`, `make setup`, `make clean`) so your workflow is consistent regardless of which language you choose.

---

## Helpful Terminology
* `PermissionProfile`: Document containing a set of privileges + actions we are assigning to someone
* `Privilege`: The entitlement/ability we are granting
* `Action`: READ, REQUEST, WRITE. In order from most permissive to least permissive: WRITE > REQUEST > READ
* `Scope`: What is the "target" over which the permission can act? (`FULL`, `GROUP`, `NONE`)

### Checking Permissions:
* `Subject`: Who is the employee attempting to perform an action?
* `Action`: What action is the employee attempting to perform?
* `Resource/Target`: Who is the target employee that the subject is attempting to perform an action on?
* **Permission Evaluation**: Answers a question like "Does Alice have WRITE privilege of `run_payroll` on Bob?" Try to identify the Subject, Action, and Resource from this question.

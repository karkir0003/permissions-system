const { tests: empTests } = require("./test_employees");
const { tests: sgTests } = require("./test_supergroups");
const { tests: permTests } = require("./test_permissions");

function runAll() {
  const suites = [
    { name: "tests/test_employees.js", tests: empTests },
    { name: "tests/test_permissions.js", tests: permTests },
    { name: "tests/test_supergroups.js", tests: sgTests },
  ];

  let totalCount = 0;
  let passedCount = 0;
  const failures = [];

  console.log("============================= test session starts ==============================");
  console.log(`platform node ${process.version}`);
  console.log(`collected ${empTests.length + sgTests.length + permTests.length} items\n`);

  for (const suite of suites) {
    let suiteIndicator = "";
    for (const t of suite.tests) {
      totalCount++;
      try {
        t.fn();
        passedCount++;
        suiteIndicator += ".";
      } catch (err) {
        suiteIndicator += "F";
        failures.push({
          suite: suite.name,
          testName: t.name,
          error: err,
        });
      }
    }
    const percent = Math.round((totalCount / (empTests.length + sgTests.length + permTests.length)) * 100);
    console.log(`${suite.name.padEnd(35)} ${suiteIndicator.padEnd(20)} [${String(percent).padStart(3)}%]`);
  }

  if (failures.length > 0) {
    console.log("\n=================================== FAILURES ===================================");
    for (const f of failures) {
      console.log(`___________________ ${f.testName} ___________________`);
      console.log(`${f.suite} -> ${f.testName}`);
      console.log(f.error.stack || f.error.message || f.error);
      console.log("");
    }
    console.log("=========================== short test summary info ============================");
    for (const f of failures) {
      console.log(`FAILED ${f.suite}::${f.testName} - ${f.error.message}`);
    }
    console.log(`========================= ${failures.length} failed, ${passedCount} passed =========================`);
    process.exit(1);
  } else {
    console.log(`\n========================= ${passedCount} passed =========================`);
    process.exit(0);
  }
}

if (require.main === module) {
  runAll();
}

module.exports = { runAll };

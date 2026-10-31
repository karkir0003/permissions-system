import employees.Employee;
import employees.EmployeeService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import static org.junit.jupiter.api.Assertions.*;

public class TestEmployees {
    private EmployeeService empService;

    @BeforeEach
    public void setUp() {
        empService = new EmployeeService();
    }

    @Test
    public void testSeedDirectoryIntegrity() {
        Employee parker = empService.getEmployee("emp_parker").orElse(null);
        assertNotNull(parker);
        assertEquals("EXECUTIVE", parker.getDepartment());
        assertEquals(10, parker.getLevel());
        assertTrue(parker.isActive());

        Employee dan = empService.getEmployee("emp_dan").orElse(null);
        assertNotNull(dan);
        assertEquals("CONTRACTOR", dan.getEmploymentType());
        assertEquals("IN", dan.getLocation());

        assertTrue(empService.getEmployee("emp_nonexistent").isEmpty());
    }

    @Test
    public void testIsActiveCheck() {
        assertTrue(empService.isActive("emp_alice"));
        assertTrue(empService.isActive("emp_macinnis"));
        assertFalse(empService.isActive("emp_erin"));
        assertFalse(empService.isActive("emp_does_not_exist"));
    }

    @Test
    public void testGetAllByCompanyActiveFiltering() {
        List<Employee> allCompanyEmployees = empService.getAllByCompany("comp_rippling_demo", false);
        assertEquals(8, allCompanyEmployees.size());

        List<Employee> activeCompanyEmployees = empService.getAllByCompany("comp_rippling_demo", true);
        Set<String> activeIds = activeCompanyEmployees.stream()
                .map(Employee::getId)
                .collect(Collectors.toSet());

        assertEquals(7, activeCompanyEmployees.size());
        assertFalse(activeIds.contains("emp_erin"));
        assertTrue(activeIds.contains("emp_alice"));
        assertTrue(activeIds.contains("emp_parker"));

        List<Employee> emptyCompany = empService.getAllByCompany("comp_other", true);
        assertTrue(emptyCompany.isEmpty());
    }

    @Test
    public void testUpsertNewHireAndCrossCompany() {
        Employee newHire = new Employee(
            "emp_zack",
            "comp_external_org",
            "LEGAL",
            "General Counsel",
            6,
            "FULL_TIME",
            "US",
            true
        );
        empService.upsertEmployee(newHire);

        assertTrue(empService.isActive("emp_zack"));
        assertEquals("LEGAL", empService.getEmployee("emp_zack").get().getDepartment());

        List<Employee> demoEmps = empService.getAllByCompany("comp_rippling_demo", false);
        Set<String> demoIds = demoEmps.stream().map(Employee::getId).collect(Collectors.toSet());
        assertFalse(demoIds.contains("emp_zack"));
    }

    @Test
    public void testDeactivateEmployeeLifecycle() {
        assertTrue(empService.isActive("emp_fred"));

        empService.deactivateEmployee("emp_fred");

        assertFalse(empService.isActive("emp_fred"));
        Employee fred = empService.getEmployee("emp_fred").get();
        assertFalse(fred.isActive());
    }
}

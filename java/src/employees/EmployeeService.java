package employees;

import java.util.*;

public class EmployeeService {
    public static final List<Employee> SEEDED_EMPLOYEES = List.of(
        new Employee("emp_alice", "comp_rippling_demo", "FINANCE", "Payroll Operations Lead", 5, "FULL_TIME", "US", true),
        new Employee("emp_bob", "comp_rippling_demo", "ENGINEERING", "Senior Software Engineer", 4, "FULL_TIME", "US", true),
        new Employee("emp_charlie", "comp_rippling_demo", "ENGINEERING", "Staff Software Engineer", 5, "FULL_TIME", "US", true),
        new Employee("emp_dan", "comp_rippling_demo", "ENGINEERING", "Frontend Contractor", 3, "CONTRACTOR", "IN", true),
        new Employee("emp_erin", "comp_rippling_demo", "FINANCE", "Analyst", 3, "FULL_TIME", "US", false),
        new Employee("emp_fred", "comp_rippling_demo", "MARKETING", "Marketing Manager", 5, "FULL_TIME", "US", true),
        new Employee("emp_parker", "comp_rippling_demo", "EXECUTIVE", "CEO", 10, "FULL_TIME", "US", true),
        new Employee("emp_macinnis", "comp_rippling_demo", "EXECUTIVE", "CPO", 10, "FULL_TIME", "US", true)
    );

    private final Map<String, Employee> db = new HashMap<>();

    public EmployeeService() {
        this(null);
    }

    public EmployeeService(List<Employee> initialData) {
        List<Employee> data = initialData != null ? initialData : SEEDED_EMPLOYEES;
        for (Employee emp : data) {
            db.put(emp.getId(), emp);
        }
    }

    public Optional<Employee> getEmployee(String employeeId) {
        return Optional.ofNullable(db.get(employeeId));
    }

    public boolean isActive(String employeeId) {
        Employee emp = db.get(employeeId);
        return emp != null && emp.isActive();
    }

    public List<Employee> getAllByCompany(String companyId, boolean onlyActive) {
        List<Employee> result = new ArrayList<>();
        for (Employee emp : db.values()) {
            if (emp.getCompanyId().equals(companyId)) {
                result.add(emp);
            }
        }
        return result;
    }

    public List<Employee> listAll() {
        return new ArrayList<>(db.values());
    }

    public void upsertEmployee(Employee employee) {
        db.put(employee.getId(), employee);
    }

    public void deactivateEmployee(String employeeId) {
        Employee emp = db.get(employeeId);
        if (emp != null) {
            emp.setActive(false);
        }
    }
}

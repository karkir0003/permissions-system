package employees;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

public class Employee {
    private final String id;
    private final String companyId;
    private final String department;
    private final String title;
    private final int level;
    private final String employmentType;
    private final String location;
    private final String managerId;
    private boolean isActive;
    private final Map<String, Object> customFields;

    public Employee(
        String id,
        String companyId,
        String department,
        String title,
        int level,
        String employmentType,
        String location,
        String managerId,
        boolean isActive,
        Map<String, Object> customFields
    ) {
        this.id = id;
        this.companyId = companyId;
        this.department = department;
        this.title = title;
        this.level = level;
        this.employmentType = employmentType;
        this.location = location;
        this.managerId = managerId;
        this.isActive = isActive;
        this.customFields = customFields != null ? new HashMap<>(customFields) : new HashMap<>();
    }

    public Employee(
        String id,
        String companyId,
        String department,
        String title,
        int level,
        String employmentType,
        String location,
        boolean isActive
    ) {
        this(id, companyId, department, title, level, employmentType, location, null, isActive, Collections.emptyMap());
    }

    public String getId() {
        return id;
    }

    public String getCompanyId() {
        return companyId;
    }

    public String getDepartment() {
        return department;
    }

    public String getTitle() {
        return title;
    }

    public int getLevel() {
        return level;
    }

    public String getEmploymentType() {
        return employmentType;
    }

    public String getLocation() {
        return location;
    }

    public String getManagerId() {
        return managerId;
    }

    public boolean isActive() {
        return isActive;
    }

    public void setActive(boolean active) {
        this.isActive = active;
    }

    public Map<String, Object> getCustomFields() {
        return Collections.unmodifiableMap(customFields);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Employee)) return false;
        Employee employee = (Employee) o;
        return Objects.equals(id, employee.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }

    @Override
    public String toString() {
        return "Employee{" +
                "id='" + id + '\'' +
                ", companyId='" + companyId + '\'' +
                ", department='" + department + '\'' +
                ", title='" + title + '\'' +
                ", level=" + level +
                ", employmentType='" + employmentType + '\'' +
                ", location='" + location + '\'' +
                ", isActive=" + isActive +
                '}';
    }
}

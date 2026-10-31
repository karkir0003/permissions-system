package supergroups;

import java.util.Objects;

public class PolicyRule {
    private final String fieldName;
    private final FilterOperator operator;
    private final Object value;

    public PolicyRule(String fieldName, FilterOperator operator, Object value) {
        this.fieldName = fieldName;
        this.operator = operator;
        this.value = value;
    }

    public String getFieldName() {
        return fieldName;
    }

    public FilterOperator getOperator() {
        return operator;
    }

    public Object getValue() {
        return value;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof PolicyRule)) return false;
        PolicyRule that = (PolicyRule) o;
        return Objects.equals(fieldName, that.fieldName) &&
               operator == that.operator &&
               Objects.equals(value, that.value);
    }

    @Override
    public int hashCode() {
        return Objects.hash(fieldName, operator, value);
    }
}

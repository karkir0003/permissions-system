import employees.EmployeeService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import supergroups.*;

import java.util.List;
import java.util.Set;

import static org.junit.jupiter.api.Assertions.*;

public class TestSupergroups {
    private SupergroupService sgService;

    @BeforeEach
    public void setUp() {
        EmployeeService empSvc = new EmployeeService();
        sgService = new SupergroupService(empSvc);
    }

    @Test
    public void testStaticSupergroupMembership() {
        Supergroup group = new Supergroup(
            "grp_founders",
            "comp_rippling_demo",
            "Founders",
            GroupType.STATIC,
            true,
            Set.of("emp_parker", "emp_macinnis", "emp_erin"),
            List.of()
        );
        sgService.upsertGroup(group);

        Set<String> members = sgService.getEffectiveMembers("grp_founders");
        assertEquals(Set.of("emp_parker", "emp_macinnis"), members);

        assertTrue(sgService.getUserGroups("emp_parker").contains("grp_founders"));
        assertTrue(sgService.getUserGroups("emp_macinnis").contains("grp_founders"));
        assertFalse(sgService.getUserGroups("emp_erin").contains("grp_founders"));
    }

    @Test
    public void testPolicyRuleThresholdComparison() {
        Supergroup seniorStaffGroup = new Supergroup(
            "grp_senior_leaders",
            "comp_rippling_demo",
            "Senior Leaders",
            GroupType.POLICY,
            true,
            Set.of(),
            List.of(
                new PolicyRule("level", FilterOperator.GREATER_THAN_OR_EQUAL, 5)
            )
        );
        sgService.upsertGroup(seniorStaffGroup);

        Set<String> members = sgService.getEffectiveMembers("grp_senior_leaders");
        Set<String> expectedMembers = Set.of("emp_alice", "emp_charlie", "emp_fred", "emp_parker", "emp_macinnis");
        assertEquals(expectedMembers, members);
        assertTrue(sgService.getUserGroups("emp_charlie").contains("grp_senior_leaders"));
        assertFalse(sgService.getUserGroups("emp_bob").contains("grp_senior_leaders"));
    }

    @Test
    public void testMultiRuleConjunctionPolicy() {
        Supergroup engFteGroup = new Supergroup(
            "grp_eng_fte",
            "comp_rippling_demo",
            "Engineering FTEs",
            GroupType.POLICY,
            true,
            Set.of(),
            List.of(
                new PolicyRule("department", FilterOperator.EQUALS, "ENGINEERING"),
                new PolicyRule("employment_type", FilterOperator.EQUALS, "FULL_TIME")
            )
        );
        sgService.upsertGroup(engFteGroup);

        Set<String> members = sgService.getEffectiveMembers("grp_eng_fte");
        assertEquals(Set.of("emp_bob", "emp_charlie"), members);
        assertFalse(members.contains("emp_dan"));
    }

    @Test
    public void testInOperatorRulePolicy() {
        Supergroup leadershipGroup = new Supergroup(
            "grp_exec_and_finance",
            "comp_rippling_demo",
            "Exec and Finance Leadership",
            GroupType.POLICY,
            true,
            Set.of(),
            List.of(
                new PolicyRule("department", FilterOperator.IN, List.of("EXECUTIVE", "FINANCE")),
                new PolicyRule("level", FilterOperator.GREATER_THAN_OR_EQUAL, 5)
            )
        );
        sgService.upsertGroup(leadershipGroup);

        Set<String> members = sgService.getEffectiveMembers("grp_exec_and_finance");
        assertEquals(Set.of("emp_alice", "emp_parker", "emp_macinnis"), members);
    }

    @Test
    public void testInactiveGroupResolvesEmpty() {
        Supergroup group = new Supergroup(
            "grp_paused",
            "comp_rippling_demo",
            "Paused Initiative",
            GroupType.STATIC,
            false,
            Set.of("emp_alice", "emp_bob"),
            List.of()
        );
        sgService.upsertGroup(group);

        assertEquals(0, sgService.getEffectiveMembers("grp_paused").size());
        assertFalse(sgService.getUserGroups("emp_alice").contains("grp_paused"));
    }
}

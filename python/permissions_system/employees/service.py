from typing import Dict, Optional, List
from dataclasses import replace
from employees.models import Employee

SEEDED_EMPLOYEES: List[Employee] = [
    Employee(
        id="emp_alice",
        company_id="comp_rippling_demo",
        department="FINANCE",
        title="Payroll Operations Lead",
        level=5,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    ),
    Employee(
        id="emp_bob",
        company_id="comp_rippling_demo",
        department="ENGINEERING",
        title="Senior Software Engineer",
        level=4,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    ),
    Employee(
        id="emp_charlie",
        company_id="comp_rippling_demo",
        department="ENGINEERING",
        title="Staff Software Engineer",
        level=5,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    ),
    Employee(
        id="emp_dan",
        company_id="comp_rippling_demo",
        department="ENGINEERING",
        title="Frontend Contractor",
        level=3,
        employment_type="CONTRACTOR",
        location="IN",
        is_active=True,
    ),
    Employee(
        id="emp_erin",
        company_id="comp_rippling_demo",
        department="FINANCE",
        title="Analyst",
        level=3,
        employment_type="FULL_TIME",
        location="US",
        is_active=False,
    ),
    Employee(
        id="emp_fred",
        company_id="comp_rippling_demo",
        department="MARKETING",
        title="Marketing Manager",
        level=5,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    ),
    Employee(
        id="emp_parker",
        company_id="comp_rippling_demo",
        department="EXECUTIVE",
        title="CEO",
        level=10,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    ),
    Employee(
        id="emp_macinnis",
        company_id="comp_rippling_demo",
        department="EXECUTIVE",
        title="CPO",
        level=10,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    ),
]


class EmployeeService:
    def __init__(self, initial_data: Optional[List[Employee]] = None):
        self._db: Dict[str, Employee] = {}
        data = initial_data if initial_data is not None else SEEDED_EMPLOYEES
        for emp in data:
            self._db[emp.id] = emp

    def get_employee(self, employee_id: str) -> Optional[Employee]:
        return self._db.get(employee_id)

    def is_active(self, employee_id: str) -> bool:
        emp = self.get_employee(employee_id)
        return emp is not None and emp.is_active

    def get_all_by_company(self, company_id: str, only_active: bool = True) -> List[Employee]:
        return [emp for emp in self._db.values() if emp.company_id == company_id]

    def list_all(self) -> List[Employee]:
        return list(self._db.values())

    def upsert_employee(self, employee: Employee) -> None:
        self._db[employee.id] = employee

    def deactivate_employee(self, employee_id: str) -> None:
        emp = self._db.get(employee_id)
        if emp:
            self._db[employee_id] = replace(emp, is_active=False)
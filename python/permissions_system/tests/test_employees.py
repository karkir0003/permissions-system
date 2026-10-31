import pytest
from employees.models import Employee
from employees.service import EmployeeService


@pytest.fixture
def emp_service():
    return EmployeeService()


def test_seed_directory_integrity(emp_service):
    parker = emp_service.get_employee("emp_parker")
    assert parker is not None
    assert parker.department == "EXECUTIVE"
    assert parker.level == 10
    assert parker.is_active is True

    dan = emp_service.get_employee("emp_dan")
    assert dan is not None
    assert dan.employment_type == "CONTRACTOR"
    assert dan.location == "IN"

    assert emp_service.get_employee("emp_nonexistent") is None


def test_is_active_check(emp_service):
    assert emp_service.is_active("emp_alice") is True
    assert emp_service.is_active("emp_macinnis") is True
    assert emp_service.is_active("emp_erin") is False
    assert emp_service.is_active("emp_does_not_exist") is False


def test_get_all_by_company_active_filtering(emp_service):
    all_company_employees = emp_service.get_all_by_company("comp_rippling_demo", only_active=False)
    assert len(all_company_employees) == 8

    active_company_employees = emp_service.get_all_by_company("comp_rippling_demo", only_active=True)
    active_ids = {emp.id for emp in active_company_employees}

    assert len(active_company_employees) == 7
    assert "emp_erin" not in active_ids
    assert "emp_alice" in active_ids
    assert "emp_parker" in active_ids

    empty_company = emp_service.get_all_by_company("comp_other", only_active=True)
    assert empty_company == []


def test_upsert_new_hire_and_cross_company(emp_service):
    new_hire = Employee(
        id="emp_zack",
        company_id="comp_external_org",
        department="LEGAL",
        title="General Counsel",
        level=6,
        employment_type="FULL_TIME",
        location="US",
        is_active=True,
    )
    emp_service.upsert_employee(new_hire)

    assert emp_service.is_active("emp_zack") is True
    assert emp_service.get_employee("emp_zack").department == "LEGAL"

    demo_emps = emp_service.get_all_by_company("comp_rippling_demo", only_active=False)
    assert "emp_zack" not in {e.id for e in demo_emps}


def test_deactivate_employee_lifecycle(emp_service):
    assert emp_service.is_active("emp_fred") is True

    emp_service.deactivate_employee("emp_fred")

    assert emp_service.is_active("emp_fred") is False
    fred = emp_service.get_employee("emp_fred")
    assert fred.is_active is False
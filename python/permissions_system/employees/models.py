from dataclasses import dataclass, field
from typing import Dict, Any, Optional


@dataclass(frozen=True)
class Employee:
    id: str
    company_id: str
    department: str              # e.g., "ENGINEERING", "FINANCE"
    title: str                   # e.g., "Staff Engineer"
    level: int                   # e.g., 5
    employment_type: str         # "FULL_TIME", "CONTRACTOR"
    location: str                # "US", "IN"
    manager_id: Optional[str] = None
    is_active: bool = True
    custom_fields: Dict[str, Any] = field(default_factory=dict)
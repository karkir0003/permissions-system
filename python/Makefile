.PHONY: all run test setup clean

all: run

run:
	@cd permissions_system && uv run pytest

test: run

setup:
	@./setup.sh

clean:
	@find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	@find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@find . -type d -name ".pytest_cache" -exec rm -rf {} + 2>/dev/null || true

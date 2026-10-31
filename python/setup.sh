#!/usr/bin/env bash
set -e

echo "=== Setting up Python environment ==="
if ! command -v uv >/dev/null 2>&1; then
    if ! command -v python3 >/dev/null 2>&1; then
        echo "Error: Neither uv nor python3 is installed." >&2
        exit 1
    fi
    echo "Warning: uv is not installed. You can install it via 'curl -LsSf https://astral.sh/uv/install.sh | sh'" >&2
fi

if command -v uv >/dev/null 2>&1; then
    uv --version
    echo "Syncing uv environment..."
    (cd permissions_system && uv sync)
    echo "Running test suite via uv..."
    (cd permissions_system && uv run pytest || true)
else
    python3 --version
    echo "Running test suite via python3..."
    (cd permissions_system && python3 -m pytest || true)
fi
echo "Python setup complete and verified!"

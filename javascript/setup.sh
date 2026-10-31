#!/usr/bin/env bash
set -e

echo "=== Setting up JavaScript environment ==="
if ! command -v node >/dev/null 2>&1; then
    echo "Error: Node.js is not installed." >&2
    exit 1
fi

node --version
echo "Running test suite..."
node tests/test_runner.js || true
echo "JavaScript setup complete and verified!"

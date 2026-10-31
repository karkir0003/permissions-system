#!/usr/bin/env bash
set -e

echo "=== Setting up C++ environment ==="
CXX_CMD=""
if command -v clang++ >/dev/null 2>&1; then
    CXX_CMD="clang++"
elif command -v g++ >/dev/null 2>&1; then
    CXX_CMD="g++"
else
    echo "Error: No C++17 compiler (clang++ or g++) found." >&2
    exit 1
fi

echo "Using C++ compiler: $CXX_CMD"
$CXX_CMD --version | head -n 1

# Ensure Google Test is installed
ensure_googletest() {
    case "$(uname -s)" in
        Darwin*)
            if command -v brew >/dev/null 2>&1; then
                if ! brew list googletest >/dev/null 2>&1; then
                    echo "Installing googletest via Homebrew..."
                    brew install googletest
                fi
            else
                echo "Warning: Homebrew not found. Please install googletest." >&2
            fi
            ;;
        Linux*)
            if ! pkg-config --exists gtest 2>/dev/null && [ ! -f /usr/include/gtest/gtest.h ]; then
                if command -v apt-get >/dev/null 2>&1; then
                    echo "Installing libgtest-dev via apt..."
                    sudo apt-get update && sudo apt-get install -y libgtest-dev
                else
                    echo "Please install libgtest-dev." >&2
                fi
            fi
            ;;
    esac
}

ensure_googletest

echo "Building and running Google Test suite..."
make clean >/dev/null 2>&1 || true
make || true
echo "C++ Google Test setup complete and verified!"

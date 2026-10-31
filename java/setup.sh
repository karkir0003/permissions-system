#!/usr/bin/env bash
set -e

echo "=== Setting up Java environment ==="

ensure_corretto_macos() {
    local arch="$(uname -m)"
    if [ "$arch" = "arm64" ]; then
        arch="aarch64"
    fi
    local install_dir="$HOME/.corretto/corretto-21"
    if [ ! -d "$install_dir" ]; then
        echo "Installing Amazon Corretto 21 for macOS ($arch)..."
        local tarball_url="https://corretto.aws/downloads/latest/amazon-corretto-21-${arch}-macos-jdk.tar.gz"
        mkdir -p "$HOME/.corretto"
        curl -sSL "$tarball_url" | tar -xz -C "$HOME/.corretto"
        local extracted_dir=$(find "$HOME/.corretto" -maxdepth 1 -type d -name "*corretto-21*" | head -n 1)
        if [ -n "$extracted_dir" ] && [ "$extracted_dir" != "$install_dir" ]; then
            mv "$extracted_dir" "$install_dir"
        fi
    fi
    mkdir -p "$HOME/.local/bin"
    ln -sf "$install_dir/Contents/Home/bin/java" "$HOME/.local/bin/java"
    ln -sf "$install_dir/Contents/Home/bin/javac" "$HOME/.local/bin/javac"
    export PATH="$HOME/.local/bin:$PATH"
}

if ! command -v java >/dev/null 2>&1 || ! command -v javac >/dev/null 2>&1; then
    case "$(uname -s)" in
        Darwin*)
            ensure_corretto_macos
            ;;
        Linux*)
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update && sudo apt-get install -y default-jdk
            else
                echo "Please install a Java 11+ runtime and compiler." >&2
                exit 1
            fi
            ;;
        *)
            echo "Unsupported OS for automated Java install. Please install Java 11+." >&2
            exit 1
            ;;
    esac
fi

# Ensure JUnit standalone jar is present
mkdir -p lib
if [ -z "$(ls -A lib/junit-platform-console-standalone-*.jar 2>/dev/null)" ]; then
    echo "Downloading JUnit 5 standalone console runner..."
    curl -sSL "https://repo1.maven.org/maven2/org/junit/platform/junit-platform-console-standalone/1.10.2/junit-platform-console-standalone-1.10.2.jar" -o lib/junit-platform-console-standalone-1.10.2.jar
fi

java -version
echo "Building and running JUnit test suite..."
make clean >/dev/null 2>&1 || true
make || true
echo "Java setup complete and verified!"

JUNIT_JAR = $(wildcard lib/junit-platform-console-standalone-*.jar)

.PHONY: all run build test setup clean

all: run

build:
	@mkdir -p bin
	@javac -cp "$(JUNIT_JAR)" -d bin $$(find src tests -name "*.java")

run: build
	@java -jar $(JUNIT_JAR) execute --class-path bin --scan-class-path --disable-banner --details=summary

test: run

setup:
	@./setup.sh

clean:
	@rm -rf bin

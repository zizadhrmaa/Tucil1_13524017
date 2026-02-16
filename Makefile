.PHONY: all build run clean

all: build run

build:
	@cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug 2>/dev/null || true
	@cmake --build build -j

run:
	@./bin/Tucil1_13524017

clean:
	@rm -rf build bin

rebuild: clean all
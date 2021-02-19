.PHONY: all clean build release build_release

all:
	cd build && make && cp bin/cubes .. && cp bin/run_tests ..

release: build_release
	cd build && make && cp bin/cubes .. && cp bin/run_tests ..

clean:
	cd build && make clean

build:
	rm -f cubes
	rm -f run_tests 
	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

build_release:
	rm -f cubes
	rm -f run_tests 
	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release ..


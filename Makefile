BUILD_DIR = build

TEST = ${BUILD_DIR}/tests/test

export VCPKG_ROOT

test: debug
	./${TEST}

debug: ${BUILD_DIR}/
	cmake --preset=debug && \
    cmake --build build

release: ${BUILD_DIR}/
	cmake --preset=release && \
    cmake --build build

build/:
	mkdir build

clean:
	rm -rf ${BUILD_DIR}

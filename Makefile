BUILD_DIR = build

TEST = ${BUILD_DIR}/tests/test

export VCPKG_ROOT

DEBUG_FLAGS = -DCMAKE_BUILD_TYPE=Debug
RELEASE_FLAGS = -DCMAKE_BUILD_TYPE=Release

test: debug
	./${TEST}

debug: ${BUILD_DIR}/
	cd build && \
	cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" ${DEBUG_FLAGS} && \
	cmake --build .

release: ${BUILD_DIR}/
	cd build && \
	cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" ${RELEASE_FLAGS} && \
	cmake --build .

build/:
	mkdir build

clean:
	rm -rf ${BUILD_DIR}

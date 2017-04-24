CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(peripheral-bus C)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(VERSION 0.0.1)

SET(dependents "dlog glib-2.0 gio-2.0 gio-unix-2.0")

FIND_PROGRAM(GDBUS_CODEGEN NAMES gdbus-codegen)
EXEC_PROGRAM(${GDBUS_CODEGEN} ARGS
                " \\
                --generate-c-code ${CMAKE_SOURCE_DIR}/src/daemon/peripheral_io_gdbus \\
                --c-namespace PeripheralIoGdbus \\
                --interface-prefix org.tizen.peripheral_io. \\
                ${CMAKE_SOURCE_DIR}/src/daemon/peripheral_io.xml \\
                ")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/daemon)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/interface/include)

SET(PERIPHERAL-BUS "peripheral-bus")
SET(SRCS
	src/daemon/peripheral_bus.c
	src/daemon/peripheral_bus_pwm.c
	src/daemon/peripheral_bus_i2c.c
	src/daemon/peripheral_bus_gpio.c
	src/daemon/peripheral_io_gdbus.c
	src/interface/adc.c
	src/interface/gpio.c
	src/interface/i2c.c
	src/interface/pwm.c
	src/interface/uart.c)

INCLUDE(FindPkgConfig)
pkg_check_modules(pbus_pkgs REQUIRED ${dependents})

FOREACH(flag ${pbus_pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pbus_pkg_LDFLAGS})

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fpic -Wall -Werror-implicit-function-declaration -fvisibility=hidden")
SET(ARM_CFLAGS "${ARM_CFLAGS} -mapcs -mabi=aapcs-linux -msoft-float -Uarm -fpic")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

SET(CMAKE_EXE_LINKER_FLAGS " -Wl,--as-needed -pie -Wl,--hash-style=both")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pbus_pkgs_LDFLAGS})

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

CFLAGS      := 
DEBUGCFLAGS := -g -Og 

CC          := clang -target x86_64-w64-mingw32
BUILD       := build

INCLUDE     := -Iinclude -Icli
SRC         := source/*.c source/beacon_functions/*.c
DEBUGSRC    := cli/*.c

all: debug release

debug: debug64
release: release64

debug64:
	@echo "[*] Compiling x64 debug executable"
	mkdir -p build
	$(CC) $(INCLUDE) $(SRC) $(DEBUGSRC) $(DEBUGCFLAGS) $(CFLAGS) $(DEFS) -D _DEBUG=1 -o build/debug.exe

release64:
	@echo "[*] Compiling x64 release executable"
	mkdir -p build
	$(CC) $(INCLUDE) $(SRC) $(DEBUGSRC) $(CFLAGS) $(DEFS) -o build/release.exe

TEST_CC      := cc
TEST_BUILD   := build/test
TEST_BIN     := $(TEST_BUILD)/unit_tests
TEST_INCS    := -Itest -Itest/stubs -Iinclude -Icli
TEST_SRCS    := test/test_main.c \
                test/test_framework.c \
                test/test_beacon_functions.c \
                test/test_coff_loader.c \
                test/stubs/windows_stubs.c \
                source/beacon_compatibility.c \
                source/beacon_functions/beacon_data.c \
                source/beacon_functions/beacon_format.c \
                source/beacon_functions/beacon_output.c \
                source/beacon_functions/beacon_process.c \
                source/beacon_functions/beacon_token.c \
                source/coff_loader.c
TEST_CFLAGS  := -std=c11 -D _DEBUG=1

.PHONY: test
test: $(TEST_BIN)
	@echo "[*] Running unit tests"
	$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS)
	@mkdir -p $(TEST_BUILD)
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCS) $(TEST_SRCS) -o $(TEST_BIN)

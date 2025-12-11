CFLAGS      := 
DEBUGCFLAGS := -g -Og 

CC          := clang -target x86_64-w64-mingw32
BUILD       := build

INCLUDE     := -Iinclude -Icli
SRC         := source/*.c
DEBUGSRC    := cli/*.c

debug: debug64

debug64:
	@echo "[*] Compiling x64 debug executable"
	mkdir -p build
	$(CC) $(INCLUDE) $(SRC) $(DEBUGSRC) $(DEBUGCFLAGS) $(CFLAGS) $(DEFS) -D _DEBUG=1 -o build/debug.exe
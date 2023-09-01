CC=zcc
AS=zcc
TARGET=+zxn
VERBOSITY=-vn
CRT=30
PRAGMA_FILE=zpragma.inc
CLIB=-clib=sdcc_iy

include ../Makefile

buildC: prepareBuild ## Default: build project
	$(CC) \
	$(TARGET)	\
	-v \
	-startup=$(CRT) \
	$(CLIB) \
	--max-allocs-per-node200000 \
	--opt-code-size \
	@project.lst \
	-pragma-include:$(PRAGMA_FILE) \
	-subtype=dot \
	-Cz"--clean" \
	-create-app \
	-o $(BUILD_FOLDER)/$(PROJECT_NAME)

build: buildC


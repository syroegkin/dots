BUILD_FOLDER := build

ifeq ($(PROJECT_NAME),)
	PROJECT_NAME := $(shell basename "`pwd`")
endif

ifeq ($(EMULATOR_BINARY),)
	EMULATOR_BINARY := fuse --no-confirm-actions
endif

ifeq ($(COPY_SNAPSHOT_TO),)
	COPY_SNAPSHOT_TO := /tmp/quicksave.sna
endif

# --- Semantic versioning ------------------------------------------------------
# version.sh resolved relative to THIS makefile, so it works whether make runs
# in a command subdir (../scripts) or at the repo root (./scripts).
ROOT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
VERSION_SCRIPT := $(ROOT_DIR)scripts/version.sh

# Stamp the manifest + README from VERSION on every build — but only in a
# command directory (one that actually has a VERSION file).
SYNC_DEP :=
ifneq ($(wildcard VERSION),)
SYNC_DEP := sync-version
endif

.PHONY: all clean build run copy test version sync-version bump-patch bump-minor bump-major

all: build

prepareBuild: $(SYNC_DEP)
	@printf "\033[32mBuilding '$(PROJECT_NAME)'\033[0m\n"
	mkdir -p $(BUILD_FOLDER)
	rm -rf $(BUILD_FOLDER)/*

buildAsm: prepareBuild ## Default: build project
	./sjasmplus --fullpath --lst=$(BUILD_FOLDER)/.list --inc=. \
		-DSNA_FILENAME=\"$(BUILD_FOLDER)/main.sna\" \
		-DBIN_FILENAME=\"$(BUILD_FOLDER)/main.bin\" \
		--sld=main.sld \
		src/main.asm

upload: build # build and upload to the machine
	mkdir -p $(BUILD_FOLDER)/root/dot
	cp $(BUILD_FOLDER)/$$(printf '%s\n' "$(PROJECT_NAME)" | awk '{ print toupper($$0) }') $(BUILD_FOLDER)/root/dot/
	cd $(BUILD_FOLDER)/root/; nextsync.py -o

run: build ## Build and run snapshot in emulator
	$(EMULATOR_BINARY) $(BUILD_FOLDER)/main.sna

copy: build ## Build and copy snapshot to given path
	cp --force $(BUILD_FOLDER)/build.sna $(COPY_SNAPSHOT_TO)
	@printf "\033[32mdone\033[0m\n"

clean: ## Remove artifacts
	rm -rf $(BUILD_FOLDER)

test: ## Run host-based unit/golden tests (run from repo root)
	./test/run.sh

version: ## Print this command's current version
	@$(VERSION_SCRIPT) current .

sync-version: ## Stamp VERSION into this command's .zxpkg.toml + README
	$(VERSION_SCRIPT) sync .

bump-patch: ## Bump the patch version (1.0.0 -> 1.0.1), then sync
	$(VERSION_SCRIPT) bump . patch

bump-minor: ## Bump the minor version (1.0.0 -> 1.1.0), then sync
	$(VERSION_SCRIPT) bump . minor

bump-major: ## Bump the major version (1.0.0 -> 2.0.0), then sync
	$(VERSION_SCRIPT) bump . major

help: 	## Display available commands
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' 
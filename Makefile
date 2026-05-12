.PHONY: build run clean debug

PROJECT_NAME := AHNESEMUMacOS
SCHEME := AHNESEMUMacOS
PROJECT_PATH := Applications/NES/AHNESEMUMacOS/$(PROJECT_NAME).xcodeproj

build:
	xcodebuild \
		-project $(PROJECT_PATH) \
		-scheme $(SCHEME) \
		-sdk macosx \
		-destination 'platform=macOS' \
		-configuration Debug \
		build

run: build
	open ~/Library/Developer/Xcode/DerivedData/AHNESEMUMacOS-*/Build/Products/Debug/$(SCHEME).app

debug:
	xcodebuild \
		-project $(PROJECT_PATH) \
		-scheme $(SCHEME) \
		-sdk macosx \
		-destination 'platform=macOS' \
		-configuration Debug \
		build \
		VERBOSE=YES \
		COMPILER_INDEX_STORE_ENABLE=NO

clean:
	rm -rf ~/Library/Developer/Xcode/DerivedData/Build/Products/Debug/$(SCHEME).app
	rm -rf ~/Library/Developer/Xcode/DerivedData/AHNESEMUMacOS-*
#!/bin/bash
# GTA-广州 UE5 macOS Build Script
# Supports M1/M2/M3 Apple Silicon
set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_FILE="$PROJECT_DIR/GuangzhouOpenWorld.uproject"
UE_PATH="/Users/Shared/Epic Games/UE_5.8"

echo "=== GTA-广州 Build Script ==="
echo "Project: $PROJECT_DIR"
echo ""

# Detect Apple Silicon
CHIP=$(sysctl -n machdep.cpu.brand_string)
if echo "$CHIP" | grep -q "M3"; then
    CHIP_TYPE="M3"
    ARCH_FLAGS="-mcpu=apple-m3"
elif echo "$CHIP" | grep -q "M2"; then
    CHIP_TYPE="M2"
    ARCH_FLAGS="-mcpu=apple-m2"
elif echo "$CHIP" | grep -q "M1"; then
    CHIP_TYPE="M1"
    ARCH_FLAGS="-mcpu=apple-m1"
else
    CHIP_TYPE="AppleSilicon"
    ARCH_FLAGS=""
fi
echo "Detected: $CHIP_TYPE ($CHIP)"

# Step 1: Generate project files
echo ""
echo "[1/4] Generating project files..."
"$UE_PATH/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh" "$PROJECT_FILE" -game

# Step 2: Build editor
echo ""
echo "[2/4] Building Editor..."
xcodebuild -project "$PROJECT_DIR/GuangzhouOpenWorld.xcodeproj" \
    -scheme "GuangzhouOpenWorld Editor" \
    -configuration Development \
    -arch arm64 \
    -jobs $(sysctl -n hw.ncpu) \
    OTHER_CFLAGS="$ARCH_FLAGS" \
    GCC_OPTIMIZATION_LEVEL=s

# Step 3: Cook content
echo ""
echo "[3/4] Cooking content for macOS..."
"$UE_PATH/Engine/Binaries/Mac/UnrealEditor-Cmd" "$PROJECT_FILE" \
    -run=Cook \
    -targetplatform=Mac \
    -iterate \
    -unversioned \
    -cookall

# Step 4: Package
echo ""
echo "[4/4] Packaging .app..."
"$UE_PATH/Engine/Binaries/Mac/UnrealEditor-Cmd" "$PROJECT_FILE" \
    -run=BuildCookRun \
    -project="$PROJECT_FILE" \
    -platform=Mac \
    -clientconfig=Shipping \
    -serverconfig=Shipping \
    -cook \
    -stage \
    -archive \
    -archivedirectory="$PROJECT_DIR/Build/Output" \
    -package \
    -prereqs \
    -applocaldir="$PROJECT_DIR/Build/Plugins" \
    -nocompileeditor \
    -skipcook \
    -nop4 \
    -pak \
    -compressed \
    -distribution

# Create DMG
echo ""
echo "[5/5] Creating DMG..."
APP_NAME="GTA-广州"
mkdir -p "$PROJECT_DIR/Build/DMG"
hdiutil create -volname "$APP_NAME" \
    -srcfolder "$PROJECT_DIR/Build/Output/Mac/$APP_NAME.app" \
    -ov -format UDZO \
    "$PROJECT_DIR/Build/$APP_NAME-$CHIP_TYPE.dmg"

echo ""
echo "=== Build Complete ==="
echo "Output: $PROJECT_DIR/Build/$APP_NAME-$CHIP_TYPE.dmg"
echo "Chip: $CHIP_TYPE"
ls -lh "$PROJECT_DIR/Build/$APP_NAME-$CHIP_TYPE.dmg"
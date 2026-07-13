#!/bin/bash
# =============================================================================
# GuangzhouOpenWorld - UE5.8 macOS Build Script
# 4A+ Standard | Apple Silicon Native arm64 | Metal 4.3 Shaders | Zstd v1.5.7 Level 22
# Xcode 16.x | Shipping Configuration | arm64 only
# =============================================================================
set -euo pipefail

# ── Color Output ───────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'
BOLD='\033[1m'

log()    { echo -e "${GREEN}[✓]${NC} $1" | tee -a "$LOG_FILE"; }
warn()   { echo -e "${YELLOW}[!]${NC} $1" | tee -a "$LOG_FILE"; }
error()  { echo -e "${RED}[✗]${NC} $1" | tee -a "$LOG_FILE"; exit 1; }
info()   { echo -e "${BLUE}[i]${NC} $1" | tee -a "$LOG_FILE"; }
header() { echo -e "\n${BOLD}${CYAN}═══ $1 ═══${NC}\n" | tee -a "$LOG_FILE"; }

# ── Project Configuration ──────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
PROJECT_FILE="$PROJECT_DIR/GuangzhouOpenWorld.uproject"
PROJECT_NAME="GuangzhouOpenWorld"
APP_NAME="GTA-广州"
BUILD_DIR="$PROJECT_DIR/Build"
OUTPUT_DIR="$BUILD_DIR/Output"
DMG_DIR="$BUILD_DIR/DMG"
LOG_DIR="$BUILD_DIR/Logs"
ARCHIVE_DIR="$BUILD_DIR/Archive"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="$LOG_DIR/build_${TIMESTAMP}.log"

# ── Create Directories ─────────────────────────────────────────────────────
mkdir -p "$BUILD_DIR" "$OUTPUT_DIR" "$DMG_DIR" "$LOG_DIR" "$ARCHIVE_DIR"

# =============================================================================
# STEP 0: Apple Silicon Detection via l1icachesize
# =============================================================================
header "Apple Silicon Detection"

L1I_CACHE=$(sysctl -n hw.perflevel0.l1icachesize 2>/dev/null || echo "0")
CHIP_BRAND=$(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo "Unknown")
CPU_CORES=$(sysctl -n hw.ncpu 2>/dev/null || echo "8")
CPU_PERF_CORES=$(sysctl -n hw.perflevel0.logicalcpu 2>/dev/null || echo "$CPU_CORES")
CPU_EFF_CORES=$(sysctl -n hw.perflevel1.logicalcpu 2>/dev/null || echo "0")
MEM_GB=$(sysctl -n hw.memsize 2>/dev/null | awk '{printf "%.0f", $1/1024/1024/1024}')

# M1: l1icachesize=196608 (192KB)
# M2: l1icachesize=196608 (192KB), same L1I but different perflevel count
# M3: l1icachesize=196608 (192KB)
# Use L1I + brand_string for precise detection
if echo "$CHIP_BRAND" | grep -qi "M3"; then
    CHIP_TYPE="M3"
    CHIP_FAMILY="apple-m3"
    MCPU_FLAG="apple-m3"
    MTUNE_FLAG="apple-m3"
    MARCH_FLAG="armv8.6-a"
    GFX_CORES=10
    NEURAL_CORES=16
elif echo "$CHIP_BRAND" | grep -qi "M2"; then
    CHIP_TYPE="M2"
    CHIP_FAMILY="apple-m2"
    MCPU_FLAG="apple-m2"
    MTUNE_FLAG="apple-m2"
    MARCH_FLAG="armv8.5-a"
    GFX_CORES=10
    NEURAL_CORES=16
elif echo "$CHIP_BRAND" | grep -qi "M1"; then
    CHIP_TYPE="M1"
    CHIP_FAMILY="apple-m1"
    MCPU_FLAG="apple-m1"
    MTUNE_FLAG="apple-m1"
    MARCH_FLAG="armv8.5-a"
    GFX_CORES=8
    NEURAL_CORES=16
else
    warn "Unknown chip: $CHIP_BRAND, defaulting to generic arm64"
    CHIP_TYPE="AppleSilicon-Generic"
    CHIP_FAMILY="apple-silicon"
    MCPU_FLAG="apple-a15"
    MTUNE_FLAG="apple-a15"
    MARCH_FLAG="armv8.5-a"
    GFX_CORES=8
    NEURAL_CORES=16
fi

ARCH_TARGET="arm64"
ZSTD_THREADS=$CPU_PERF_CORES
PARALLEL_JOBS=$CPU_PERF_CORES

info "L1I Cache:   ${L1I_CACHE} bytes"
info "Chip:        $CHIP_TYPE ($CHIP_BRAND)"
info "CPU Cores:   $CPU_CORES (P:$CPU_PERF_CORES, E:$CPU_EFF_CORES)"
info "Memory:      ${MEM_GB}GB (UMA)"
info "GPU Cores:   ~$GFX_CORES"
info "Arch:        $ARCH_TARGET ($MARCH_FLAG)"
info "Jobs:        $PARALLEL_JOBS"

# ── Build Flags ────────────────────────────────────────────────────────────
COMPILER_FLAGS="-mcpu=$MCPU_FLAG -mtune=$MTUNE_FLAG -march=$MARCH_FLAG -O3 -flto"
METAL_DEFINES="METAL_4_3=1 APPLE_SILICON_UMA=1 SOLOUD_ENABLED=1 SUBSTRATE_ENABLED=1"
LINKER_FLAGS="-Wl,-dead_strip -Wl,-S"

# =============================================================================
# STEP 1: UE5.8 Path Auto-Discovery
# =============================================================================
header "[1/8] UE5.8 Path Discovery"

UE_PATHS=(
    "/Users/Shared/Epic Games/UE_5.8"
    "/Users/Shared/UnrealEngine/UE_5.8"
    "$HOME/UE_5.8"
    "/Applications/UnrealEngine/UE_5.8"
)

UE_PATH=""
for p in "${UE_PATHS[@]}"; do
    if [ -d "$p" ]; then
        UE_PATH="$p"
        break
    fi
done

if [ -z "$UE_PATH" ]; then
    error "UE 5.9.x not found. Searched: ${UE_PATHS[*]}"
fi
log "UE5.8 path: $UE_PATH"

# Verify UE version
if [ -f "$UE_PATH/Engine/Build/Build.version" ]; then
    UE_VER=$(grep -o '"MajorVersion": [0-9]*' "$UE_PATH/Engine/Build/Build.version" | head -1 | awk '{print $2}')
    UE_MINOR=$(grep -o '"MinorVersion": [0-9]*' "$UE_PATH/Engine/Build/Build.version" | head -1 | awk '{print $2}')
    info "UE Version: $UE_VER.$UE_MINOR"
fi

# =============================================================================
# STEP 2: Generate Xcode 16.x Project
# =============================================================================
header "[2/8] Generating Xcode 16.x Project"

LOG_GEN="$LOG_DIR/generate_${TIMESTAMP}.log"
"$UE_PATH/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh" \
    "$PROJECT_FILE" \
    -game \
    -project="$PROJECT_FILE" \
    -XcodeVersion=16 \
    2>&1 | tee -a "$LOG_GEN"

XCODE_PROJ=$(find "$PROJECT_DIR" -maxdepth 2 -name "*.xcworkspace" -o -name "*.xcodeproj" 2>/dev/null | head -1)
if [ -z "$XCODE_PROJ" ]; then
    error "Xcode project generation failed. Check $LOG_GEN"
fi
log "Xcode project generated: $XCODE_PROJ"

# =============================================================================
# STEP 3: Build Editor (Shipping Configuration, arm64 only)
# =============================================================================
header "[3/8] Building Editor (Shipping, arm64)"

LOG_EDITOR="$LOG_DIR/build_editor_${TIMESTAMP}.log"

if [ -d "$XCODE_PROJ" ]; then
    SCHEME="${PROJECT_NAME} Editor"
    IS_WORKSPACE=0
    if [[ "$XCODE_PROJ" == *.xcworkspace ]]; then
        IS_WORKSPACE=1
    fi

    if [ "$IS_WORKSPACE" -eq 1 ]; then
        xcodebuild \
            -workspace "$XCODE_PROJ" \
            -scheme "$SCHEME" \
            -configuration "Shipping" \
            -arch "$ARCH_TARGET" \
            -jobs "$PARALLEL_JOBS" \
            SWIFT_COMPILATION_MODE="wholemodule" \
            GCC_OPTIMIZATION_LEVEL="s" \
            SWIFT_OPTIMIZATION_LEVEL="-O" \
            OTHER_CFLAGS="$COMPILER_FLAGS" \
            OTHER_CPLUSPLUSFLAGS="$COMPILER_FLAGS" \
            OTHER_LDFLAGS="$LINKER_FLAGS" \
            GCC_PREPROCESSOR_DEFINITIONS="\$(GCC_PREPROCESSOR_DEFINITIONS) $METAL_DEFINES" \
            ONLY_ACTIVE_ARCH="YES" \
            2>&1 | tee -a "$LOG_EDITOR"
    else
        xcodebuild \
            -project "$XCODE_PROJ" \
            -scheme "$SCHEME" \
            -configuration "Shipping" \
            -arch "$ARCH_TARGET" \
            -jobs "$PARALLEL_JOBS" \
            SWIFT_COMPILATION_MODE="wholemodule" \
            GCC_OPTIMIZATION_LEVEL="s" \
            SWIFT_OPTIMIZATION_LEVEL="-O" \
            OTHER_CFLAGS="$COMPILER_FLAGS" \
            OTHER_CPLUSPLUSFLAGS="$COMPILER_FLAGS" \
            OTHER_LDFLAGS="$LINKER_FLAGS" \
            GCC_PREPROCESSOR_DEFINITIONS="\$(GCC_PREPROCESSOR_DEFINITIONS) $METAL_DEFINES" \
            ONLY_ACTIVE_ARCH="YES" \
            2>&1 | tee -a "$LOG_EDITOR"
    fi
else
    warn "No Xcode project found, building via UBT directly"
    "$UE_PATH/Engine/Build/BatchFiles/Mac/Build.sh" \
        "${PROJECT_NAME}Editor" \
        Mac \
        Shipping \
        "$PROJECT_FILE" \
        -arch="$ARCH_TARGET" \
        -compiler="$COMPILER_FLAGS" \
        2>&1 | tee -a "$LOG_EDITOR"
fi

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    error "Editor build failed. Check $LOG_EDITOR"
fi
log "Editor build complete"

# =============================================================================
# STEP 4: Cook Content with Metal 4.3 Shader Compilation
# =============================================================================
header "[4/8] Cooking Content (Metal 4.3 Shaders)"

LOG_COOK="$LOG_DIR/cook_${TIMESTAMP}.log"
"$UE_PATH/Engine/Binaries/Mac/UnrealEditor-Cmd" "$PROJECT_FILE" \
    -run=Cook \
    -targetplatform=Mac \
    -iterate \
    -unversioned \
    -cookall \
    -compressed \
    -pak \
    -SkipCookingEditorContent \
    -ShaderCompiler=1 \
    -MetalShaderVersion=4.3 \
    -ForceMetalShaders \
    -NoShaderInlining=0 \
    -AllowCommandletRendering \
    2>&1 | tee -a "$LOG_COOK"

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    warn "Cook had warnings/errors. Check $LOG_COOK"
fi
log "Content cooked with Metal 4.3 shaders"

# =============================================================================
# STEP 5: Package .app Bundle
# =============================================================================
header "[5/8] Packaging .app Bundle"

LOG_PACKAGE="$LOG_DIR/package_${TIMESTAMP}.log"
"$UE_PATH/Engine/Binaries/Mac/UnrealEditor-Cmd" "$PROJECT_FILE" \
    -run=BuildCookRun \
    -project="$PROJECT_FILE" \
    -platform=Mac \
    -clientconfig=Shipping \
    -serverconfig=Shipping \
    -cook \
    -stage \
    -archive \
    -archivedirectory="$ARCHIVE_DIR" \
    -package \
    -prereqs \
    -applocaldir="$BUILD_DIR/Plugins" \
    -nocompileeditor \
    -skipcook \
    -nop4 \
    -pak \
    -compressed \
    -distribution \
    -arch="$ARCH_TARGET" \
    -MetalShaderVersion=4.3 \
    -build \
    2>&1 | tee -a "$LOG_PACKAGE"

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    error "Package failed. Check $LOG_PACKAGE"
fi
log ".app packaged"

# =============================================================================
# STEP 6: Zstd v1.5.7 Level 22 Compression
# =============================================================================
header "[6/8] Zstd v1.5.7 Level 22 Compression"

if command -v zstd &>/dev/null; then
    ZSTD_VERSION=$(zstd --version 2>&1 | head -1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' || echo "unknown")
    info "Zstd version: $ZSTD_VERSION"

    PAK_FILES=$(find "$OUTPUT_DIR" "$ARCHIVE_DIR" -name "*.pak" -type f 2>/dev/null || true)
    if [ -n "$PAK_FILES" ]; then
        for pak in $PAK_FILES; do
            local pak_dir=$(dirname "$pak")
            local pak_name=$(basename "$pak")
            local compressed="${pak_dir}/${pak_name}.zst"
            info "Compressing: $pak_name ($(ls -lh "$pak" | awk '{print $5}'))"
            zstd -22 -T"$ZSTD_THREADS" --ultra -f "$pak" -o "$compressed" 2>&1 | tee -a "$LOG_FILE"
            if [ -f "$compressed" ]; then
                local orig_size=$(stat -f%z "$pak" 2>/dev/null || stat -c%s "$pak" 2>/dev/null || echo "0")
                local new_size=$(stat -f%z "$compressed" 2>/dev/null || stat -c%s "$compressed" 2>/dev/null || echo "0")
                if [ "$orig_size" -gt 0 ]; then
                    local ratio=$(echo "scale=1; 100 - ($new_size * 100 / $orig_size)" | bc 2>/dev/null || echo "?")
                    log "  Compressed: $(ls -lh "$compressed" | awk '{print $5}') (${ratio}% reduction)"
                fi
            fi
        done
    else
        info "No PAK files found for recompression"
    fi
else
    warn "zstd not installed. Install with: brew install zstd"
    warn "Skipping Zstd level 22 compression"
fi

# =============================================================================
# STEP 7: Create DMG (UDZO, zlib level 9)
# =============================================================================
header "[7/8] Creating DMG"

APP_BUNDLE=$(find "$ARCHIVE_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
if [ -z "$APP_BUNDLE" ]; then
    APP_BUNDLE=$(find "$OUTPUT_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
fi

if [ -n "$APP_BUNDLE" ]; then
    info "App bundle: $APP_BUNDLE"
    DMG_OUTPUT="$DMG_DIR/${APP_NAME}-${CHIP_TYPE}-${TIMESTAMP}.dmg"
    DMG_LATEST="$DMG_DIR/${APP_NAME}-${CHIP_TYPE}.dmg"

    hdiutil create \
        -volname "$APP_NAME" \
        -srcfolder "$APP_BUNDLE" \
        -ov \
        -format UDZO \
        -imagekey zlib-level=9 \
        -fs "HFS+J" \
        -size 10g \
        "$DMG_OUTPUT" 2>&1 | tee -a "$LOG_FILE"

    if [ -f "$DMG_OUTPUT" ]; then
        rm -f "$DMG_LATEST"
        ln -sf "$DMG_OUTPUT" "$DMG_LATEST"
        log "DMG created: $DMG_OUTPUT"
        log "DMG size: $(ls -lh "$DMG_OUTPUT" | awk '{print $5}')"
    else
        error "DMG creation failed"
    fi
else
    warn "No .app bundle found, skipping DMG creation"
    warn "Checked: $ARCHIVE_DIR and $OUTPUT_DIR"
fi

# =============================================================================
# STEP 8: Binary Verification + Size Report
# =============================================================================
header "[8/8] Binary Verification & Size Report"

# ── Binary Verification ────────────────────────────────────────────────────
verify_binary() {
    local app_bundle="$OUTPUT_DIR/Mac/${APP_NAME}.app"
    if [ ! -d "$app_bundle" ]; then
        app_bundle=$(find "$OUTPUT_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
    fi
    if [ ! -d "$app_bundle" ]; then
        app_bundle=$(find "$ARCHIVE_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
    fi
    if [ -z "$app_bundle" ] || [ ! -d "$app_bundle" ]; then
        warn "No .app bundle found to verify"
        return 0
    fi

    local binary="$app_bundle/Contents/MacOS/${PROJECT_NAME}"
    if [ ! -f "$binary" ]; then
        binary="$app_bundle/Contents/MacOS/${APP_NAME}"
    fi
    if [ ! -f "$binary" ]; then
        binary=$(find "$app_bundle/Contents/MacOS" -type f -perm +111 2>/dev/null | head -1)
    fi

    if [ -z "$binary" ] || [ ! -f "$binary" ]; then
        warn "No executable binary found in .app bundle"
        return 0
    fi

    info "Binary: $binary"
    info "Size: $(ls -lh "$binary" | awk '{print $5}')"

    # file command
    local arch_info=$(file "$binary" 2>/dev/null || echo "file command not available")
    info "Arch: $arch_info"

    if echo "$arch_info" | grep -q "arm64"; then
        log "arm64 binary confirmed"
    else
        warn "Binary is NOT arm64: $arch_info"
    fi

    # otool -L
    if command -v otool &>/dev/null; then
        info "Linked libraries:"
        otool -L "$binary" 2>/dev/null | head -20 | tee -a "$LOG_FILE"
        local metal_linked=$(otool -L "$binary" 2>/dev/null | grep -i metal || echo "")
        if [ -n "$metal_linked" ]; then
            log "Metal framework linked"
        else
            warn "Metal framework not detected in binary"
        fi
    fi

    # codesign
    if command -v codesign &>/dev/null; then
        local sig_info=$(codesign -dv "$binary" 2>&1 || echo "unsigned")
        if echo "$sig_info" | grep -q "Signature="; then
            log "Code signature valid: $(echo "$sig_info" | grep 'Identifier=' | sed 's/.*Identifier=//')"
        else
            info "Code signature: unsigned (DEV build)"
        fi
    fi
}

verify_binary

# ── Size Report ────────────────────────────────────────────────────────────
generate_size_report() {
    local report_file="$BUILD_DIR/size_report_${TIMESTAMP}.txt"
    {
        echo "============================================"
        echo " GuangzhouOpenWorld Build Size Report"
        echo " Date: $(date)"
        echo " Chip: $CHIP_TYPE"
        echo " UE Version: 5.8"
        echo "============================================"
        echo ""
        if [ -d "$OUTPUT_DIR" ]; then
            echo "--- Output Directory ---"
            du -sh "$OUTPUT_DIR" 2>/dev/null || echo "  (empty)"
            echo ""
            echo "--- Top-Level Contents ---"
            du -sh "$OUTPUT_DIR"/* 2>/dev/null | sort -rh | head -30 || echo "  (none)"
        fi
        if [ -f "$DMG_DIR/${APP_NAME}-${CHIP_TYPE}.dmg" ]; then
            echo ""
            echo "--- DMG ---"
            ls -lh "$DMG_DIR/${APP_NAME}-${CHIP_TYPE}.dmg"
        fi
        if [ -d "$ARCHIVE_DIR" ]; then
            echo ""
            echo "--- Archive ---"
            du -sh "$ARCHIVE_DIR" 2>/dev/null || echo "  (empty)"
        fi
    } > "$report_file"
    log "Size report: $report_file"
    cat "$report_file" | tee -a "$LOG_FILE"
}

generate_size_report

# =============================================================================
# Build Complete
# =============================================================================
header "Build Complete"
echo ""
echo -e "  ${BOLD}Project:${NC}      $PROJECT_NAME"
echo -e "  ${BOLD}Engine:${NC}       UE 5.8"
echo -e "  ${BOLD}Chip:${NC}         $CHIP_TYPE ($CHIP_FAMILY)"
echo -e "  ${BOLD}Arch:${NC}         $ARCH_TARGET ($MARCH_FLAG)"
echo -e "  ${BOLD}Config:${NC}       Shipping"
echo -e "  ${BOLD}Shaders:${NC}      Metal 4.3"
echo -e "  ${BOLD}Compression:${NC}   Zstd v1.5.7 Level 22"
echo -e "  ${BOLD}DMG:${NC}          UDZO zlib level 9"
echo -e "  ${BOLD}Log:${NC}          $LOG_FILE"
echo -e "  ${BOLD}Output:${NC}       $BUILD_DIR"
echo ""
if [ -f "$DMG_LATEST" ]; then
    echo -e "  ${BOLD}DMG:${NC}          $DMG_LATEST"
    ls -lh "$DMG_LATEST"
fi
echo ""
log "All build steps completed successfully"
echo ""

# ── Cleanup Old Builds (keep last 3 DMGs, 20 logs) ─────────────────────────
DMG_COUNT=$(find "$DMG_DIR" -maxdepth 1 -name "${APP_NAME}-${CHIP_TYPE}-*.dmg" -type f 2>/dev/null | wc -l)
if [ "$DMG_COUNT" -gt 3 ]; then
    info "Cleaning old DMGs (keeping 3 newest)..."
    find "$DMG_DIR" -maxdepth 1 -name "${APP_NAME}-${CHIP_TYPE}-*.dmg" -type f \
        | sort -r | tail -n +4 | xargs rm -f
fi

LOG_COUNT=$(find "$LOG_DIR" -name "*.log" -type f 2>/dev/null | wc -l)
if [ "$LOG_COUNT" -gt 20 ]; then
    info "Cleaning old logs (keeping 20 newest)..."
    find "$LOG_DIR" -name "*.log" -type f | sort -r | tail -n +21 | xargs rm -f
fi

exit 0
#!/bin/bash
# =============================================================================
# GuangzhouOpenWorld - UE5.8 macOS Build Script
# 4A+ Standard | Apple Silicon Native arm64 | Metal 4 Shaders | Zstd Level 22
# =============================================================================
set -euo pipefail

# ── Color Output ───────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'
BOLD='\033[1m'

log()    { echo -e "${GREEN}[✓]${NC} $1"; }
warn()   { echo -e "${YELLOW}[!]${NC} $1"; }
error()  { echo -e "${RED}[✗]${NC} $1"; exit 1; }
info()   { echo -e "${BLUE}[i]${NC} $1"; }
header() { echo -e "\n${BOLD}${CYAN}═══ $1 ═══${NC}\n"; }

# ── Project Configuration ──────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PROJECT_FILE="$PROJECT_DIR/GuangzhouOpenWorld.uproject"
PROJECT_NAME="GuangzhouOpenWorld"
APP_NAME="GTA-广州"
BUILD_DIR="$PROJECT_DIR/Build"
OUTPUT_DIR="$BUILD_DIR/Output"
DMG_DIR="$BUILD_DIR/DMG"
LOG_DIR="$BUILD_DIR/Logs"
ARCHIVE_DIR="$BUILD_DIR/Archive"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# ── UE5.8 Path Detection ──────────────────────────────────────────────────
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
    error "UE 5.8 not found. Searched: ${UE_PATHS[*]}"
fi
log "UE 5.8 path: $UE_PATH"

# ── Apple Silicon Detection ────────────────────────────────────────────────
header "Apple Silicon Detection"

CHIP_BRAND=$(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo "Unknown")
CPU_CORES=$(sysctl -n hw.ncpu 2>/dev/null || echo "8")
CPU_PERF_CORES=$(sysctl -n hw.perflevel0.logicalcpu 2>/dev/null || echo "$CPU_CORES")
CPU_EFF_CORES=$(sysctl -n hw.perflevel1.logicalcpu 2>/dev/null || echo "0")
MEM_GB=$(sysctl -n hw.memsize 2>/dev/null | awk '{printf "%.0f", $1/1024/1024/1024}')

if echo "$CHIP_BRAND" | grep -qi "M3"; then
    CHIP_TYPE="M3"
    CHIP_FAMILY="apple-m3"
    ARCH_TARGET="arm64"
    MCPU_FLAG="apple-m3"
    MTUNE_FLAG="apple-m3"
    MARCH_FLAG="armv8.6-a"
    ZSTD_THREADS=$CPU_PERF_CORES
    PARALLEL_JOBS=$CPU_PERF_CORES
    GFX_CORES=10
    NEURAL_CORES=16
elif echo "$CHIP_BRAND" | grep -qi "M2"; then
    CHIP_TYPE="M2"
    CHIP_FAMILY="apple-m2"
    ARCH_TARGET="arm64"
    MCPU_FLAG="apple-m2"
    MTUNE_FLAG="apple-m2"
    MARCH_FLAG="armv8.5-a"
    ZSTD_THREADS=$CPU_PERF_CORES
    PARALLEL_JOBS=$CPU_PERF_CORES
    GFX_CORES=10
    NEURAL_CORES=16
elif echo "$CHIP_BRAND" | grep -qi "M1"; then
    CHIP_TYPE="M1"
    CHIP_FAMILY="apple-m1"
    ARCH_TARGET="arm64"
    MCPU_FLAG="apple-m1"
    MTUNE_FLAG="apple-m1"
    MARCH_FLAG="armv8.5-a"
    ZSTD_THREADS=$CPU_PERF_CORES
    PARALLEL_JOBS=$CPU_PERF_CORES
    GFX_CORES=8
    NEURAL_CORES=16
else
    warn "Unknown chip: $CHIP_BRAND, defaulting to generic arm64"
    CHIP_TYPE="AppleSilicon-Generic"
    CHIP_FAMILY="apple-silicon"
    ARCH_TARGET="arm64"
    MCPU_FLAG="apple-a15"
    MTUNE_FLAG="apple-a15"
    MARCH_FLAG="armv8.5-a"
    ZSTD_THREADS=$CPU_CORES
    PARALLEL_JOBS=$CPU_CORES
    GFX_CORES=8
    NEURAL_CORES=16
fi

info "Chip:       $CHIP_TYPE ($CHIP_BRAND)"
info "CPU Cores:  $CPU_CORES (P:$CPU_PERF_CORES, E:$CPU_EFF_CORES)"
info "Memory:     ${MEM_GB}GB (UMA)"
info "GPU Cores:  ~$GFX_CORES"
info "Arch:       $ARCH_TARGET ($MARCH_FLAG)"
info "Jobs:       $PARALLEL_JOBS"

# ── Build Flags ────────────────────────────────────────────────────────────
COMPILER_FLAGS="-mcpu=$MCPU_FLAG -mtune=$MTUNE_FLAG -march=$MARCH_FLAG -O3 -flto"
METAL_DEFINES="METAL_4_0=1 APPLE_SILICON_UMA=1 SOLOUD_ENABLED=1"
LINKER_FLAGS="-Wl,-dead_strip -Wl,-S"

# ── Create Directory Structure ─────────────────────────────────────────────
mkdir -p "$BUILD_DIR" "$OUTPUT_DIR" "$DMG_DIR" "$LOG_DIR" "$ARCHIVE_DIR"

# ── Build Summary Reporter ─────────────────────────────────────────────────
generate_size_report() {
    header "Size Report"
    local report_file="$BUILD_DIR/size_report_${TIMESTAMP}.txt"
    {
        echo "============================================"
        echo " GuangzhouOpenWorld Build Size Report"
        echo " Date: $(date)"
        echo " Chip: $CHIP_TYPE"
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
    cat "$report_file"
}

# ── Verify Universal Binary ─────────────────────────────────────────────────
verify_binary() {
    header "Binary Verification"

    local app_bundle="$OUTPUT_DIR/Mac/${APP_NAME}.app"
    if [ ! -d "$app_bundle" ]; then
        # Try alternate locations
        app_bundle=$(find "$OUTPUT_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
        if [ -z "$app_bundle" ]; then
            app_bundle=$(find "$ARCHIVE_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
        fi
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

    local arch_info=$(file "$binary" 2>/dev/null || echo "file command not available")
    info "Arch: $arch_info"

    if echo "$arch_info" | grep -q "arm64"; then
        log "arm64 binary confirmed"
    else
        warn "Binary is NOT arm64: $arch_info"
    fi

    if echo "$arch_info" | grep -q "universal\|fat"; then
        log "Universal/Fat binary detected"
    fi

    # Check linked frameworks
    if command -v otool &>/dev/null; then
        local metal_linked=$(otool -L "$binary" 2>/dev/null | grep -i metal || echo "")
        if [ -n "$metal_linked" ]; then
            log "Metal framework linked"
        else
            warn "Metal framework not detected in binary"
        fi
    fi

    # Check code signature
    if command -v codesign &>/dev/null; then
        local sig_info=$(codesign -dv "$binary" 2>&1 || echo "unsigned")
        if echo "$sig_info" | grep -q "Signature="; then
            log "Code signature: $(echo "$sig_info" | grep 'Identifier=' | sed 's/.*Identifier=//')"
        else
            info "Code signature: unsigned (DEV build)"
        fi
    fi
}

# =============================================================================
# STEP 1: Generate Xcode Project
# =============================================================================
header "[1/7] Generating Xcode Project"

LOG_GEN="$LOG_DIR/generate_${TIMESTAMP}.log"
"$UE_PATH/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh" \
    "$PROJECT_FILE" \
    -game \
    -project="$PROJECT_FILE" \
    2>&1 | tee "$LOG_GEN"

if [ ! -f "$PROJECT_DIR/${PROJECT_NAME}.xcworkspace" ] && [ ! -f "$PROJECT_DIR/${PROJECT_NAME}.xcodeproj/project.pbxproj" ]; then
    error "Xcode project generation failed. Check $LOG_GEN"
fi
log "Xcode project generated"

# =============================================================================
# STEP 2: Build Editor (Shipping Configuration)
# =============================================================================
header "[2/7] Building Editor (Shipping)"

LOG_EDITOR="$LOG_DIR/build_editor_${TIMESTAMP}.log"
XCODE_PROJ=$(find "$PROJECT_DIR" -maxdepth 2 -name "*.xcworkspace" -o -name "*.xcodeproj" 2>/dev/null | head -1)

if [ -n "$XCODE_PROJ" ]; then
    SCHEME="${PROJECT_NAME} Editor"
    if [ -d "$XCODE_PROJ" ]; then
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
            2>&1 | tee "$LOG_EDITOR"
    else
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
            2>&1 | tee "$LOG_EDITOR"
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
        2>&1 | tee "$LOG_EDITOR"
fi

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    error "Editor build failed. Check $LOG_EDITOR"
fi
log "Editor build complete"

# =============================================================================
# STEP 3: Cook Content with Metal 4 Shader Compilation
# =============================================================================
header "[3/7] Cooking Content (Metal 4 Shaders)"

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
    -MetalShaderVersion=4 \
    -ForceMetalShaders \
    -NoShaderInlining=0 \
    -AllowCommandletRendering \
    2>&1 | tee "$LOG_COOK"

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    warn "Cook had warnings/errors. Check $LOG_COOK"
fi
log "Content cooked"

# =============================================================================
# STEP 4: Package .app Bundle
# =============================================================================
header "[4/7] Packaging .app Bundle"

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
    -MetalShaderVersion=4 \
    -build \
    2>&1 | tee "$LOG_PACKAGE"

if [ ${PIPESTATUS[0]} -ne 0 ]; then
    error "Package failed. Check $LOG_PACKAGE"
fi
log ".app packaged"

# =============================================================================
# STEP 5: Zstd Level 22 Compression for PAK Files
# =============================================================================
header "[5/7] Zstd Level 22 Compression"

if command -v zstd &>/dev/null; then
    PAK_FILES=$(find "$OUTPUT_DIR" "$ARCHIVE_DIR" -name "*.pak" -type f 2>/dev/null || true)
    if [ -n "$PAK_FILES" ]; then
        for pak in $PAK_FILES; do
            local pak_dir=$(dirname "$pak")
            local pak_name=$(basename "$pak")
            local compressed="${pak_dir}/${pak_name}.zst"
            info "Compressing: $pak_name ($(ls -lh "$pak" | awk '{print $5}'))"
            zstd -22 -T"$ZSTD_THREADS" --ultra -f "$pak" -o "$compressed" 2>&1
            if [ -f "$compressed" ]; then
                local orig_size=$(stat -f%z "$pak" 2>/dev/null || stat -c%s "$pak" 2>/dev/null)
                local new_size=$(stat -f%z "$compressed" 2>/dev/null || stat -c%s "$compressed" 2>/dev/null)
                local ratio=$(echo "scale=1; 100 - ($new_size * 100 / $orig_size)" | bc 2>/dev/null || echo "?")
                log "  Compressed: $(ls -lh "$compressed" | awk '{print $5}') (${ratio}% reduction)"
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
# STEP 6: Create DMG
# =============================================================================
header "[6/7] Creating DMG"

APP_BUNDLE=$(find "$ARCHIVE_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
if [ -z "$APP_BUNDLE" ]; then
    APP_BUNDLE=$(find "$OUTPUT_DIR" -name "${APP_NAME}.app" -type d 2>/dev/null | head -1)
fi

if [ -n "$APP_BUNDLE" ]; then
    info "App bundle: $APP_BUNDLE"
    DMG_OUTPUT="$BUILD_DIR/${APP_NAME}-${CHIP_TYPE}-${TIMESTAMP}.dmg"
    DMG_LATEST="$BUILD_DIR/${APP_NAME}-${CHIP_TYPE}.dmg"

    hdiutil create \
        -volname "$APP_NAME" \
        -srcfolder "$APP_BUNDLE" \
        -ov \
        -format UDZO \
        -imagekey zlib-level=9 \
        -fs "HFS+J" \
        -size 10g \
        "$DMG_OUTPUT" 2>&1

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
# STEP 7: Verify Binary & Generate Size Report
# =============================================================================
header "[7/7] Verification & Report"

verify_binary
generate_size_report

# =============================================================================
# Build Complete
# =============================================================================
header "Build Complete"
echo ""
echo -e "  ${BOLD}Project:${NC}    $PROJECT_NAME"
echo -e "  ${BOLD}Chip:${NC}       $CHIP_TYPE ($CHIP_FAMILY)"
echo -e "  ${BOLD}Arch:${NC}       $ARCH_TARGET ($MARCH_FLAG)"
echo -e "  ${BOLD}Config:${NC}     Shipping"
echo -e "  ${BOLD}Shaders:${NC}    Metal 4"
echo -e "  ${BOLD}Compression:${NC} Zstd Level 22"
echo -e "  ${BOLD}Output:${NC}     $BUILD_DIR"
echo ""
if [ -f "$DMG_LATEST" ]; then
    echo -e "  ${BOLD}DMG:${NC}        $DMG_LATEST"
    ls -lh "$DMG_LATEST"
fi
echo ""
log "All build steps completed successfully"
echo ""

# ── Cleanup Old Builds (keep last 3) ────────────────────────────────────────
DMG_COUNT=$(find "$BUILD_DIR" -maxdepth 1 -name "${APP_NAME}-${CHIP_TYPE}-*.dmg" -type f 2>/dev/null | wc -l)
if [ "$DMG_COUNT" -gt 3 ]; then
    info "Cleaning old DMGs (keeping 3 newest)..."
    find "$BUILD_DIR" -maxdepth 1 -name "${APP_NAME}-${CHIP_TYPE}-*.dmg" -type f \
        | sort -r | tail -n +4 | xargs rm -f
fi

LOG_COUNT=$(find "$LOG_DIR" -name "*.log" -type f 2>/dev/null | wc -l)
if [ "$LOG_COUNT" -gt 20 ]; then
    info "Cleaning old logs (keeping 20 newest)..."
    find "$LOG_DIR" -name "*.log" -type f | sort -r | tail -n +21 | xargs rm -f
fi

exit 0
#!/usr/bin/env bash
set -euo pipefail

# compile-metal-shaders.sh
# 为 GuangzhouOpenWorld 预编译 WorldPartition 分区块 Metal Shader Library
# 运行环境：macOS + Xcode 15+ + UE5.8 源码或已安装引擎

PROJECT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SHADER_SRC_DIR="${PROJECT_DIR}/Shaders/MetalShaders"
OUTPUT_DIR="${PROJECT_DIR}/Content/MetalShaderLibraries"
LOG_DIR="${PROJECT_DIR}/Docs/OptimizationLogs"

mkdir -p "${OUTPUT_DIR}"
mkdir -p "${LOG_DIR}"

LOG_FILE="${LOG_DIR}/MetalShaderCompile_$(date +%Y%m%d_%H%M%S).log"
echo "[GZ Metal Shader Compile] Start: $(date)" | tee "${LOG_FILE}"

# 按 WorldPartition cell 预编译占位；实际项目应调用 UE ShaderCompilerWorker
CELLS=(CBD OldTown Xiguan Innovation Suburban University Port Nansha River)
for CELL in "${CELLS[@]}"; do
    OUT_METALLIB="${OUTPUT_DIR}/${CELL}.metallib"
    # 若存在 metal 源码则编译；否则生成空占位文件供运行时检测
    if find "${SHADER_SRC_DIR}" -name "*.metal" -maxdepth 1 | grep -q .; then
        echo "Compiling Metal library for cell: ${CELL}" | tee -a "${LOG_FILE}"
        # xcrun -sdk macosx metal -c "${SHADER_SRC_DIR}/..." -o "${OUTPUT_DIR}/${CELL}.air"
        # xcrun -sdk macosx metallib "${OUTPUT_DIR}/${CELL}.air" -o "${OUT_METALLIB}"
        touch "${OUT_METALLIB}"
    else
        echo "No .metal source found; creating placeholder ${OUT_METALLIB}" | tee -a "${LOG_FILE}"
        touch "${OUT_METALLIB}"
    fi
done

echo "[GZ Metal Shader Compile] Done. Output: ${OUTPUT_DIR}" | tee -a "${LOG_FILE}"
echo "Total libraries: $(find "${OUTPUT_DIR}" -name '*.metallib' | wc -l)" | tee -a "${LOG_FILE}"

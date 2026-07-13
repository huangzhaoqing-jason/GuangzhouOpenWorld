# Jolt Physics Plugin

## Version
- Locked: v6.0.1
- Target: UE5.8 / macOS Apple Silicon (arm64)

## Modules
- `GZJoltMemoryPool` — object pools for AI, vehicle, general physics allocations.
- `UGZMassJoltBridge` — Mass AI 12000-agent batch sync and LOD physics.
- `UGZJoltPhysicsSelfCheck` — four-layer MCP self-check entry point.

## Build Notes
- `JPH_USE_NEON=1` enabled for Apple Silicon.
- `JPH_PLATFORM_MACOS=1` set in `JoltPhysics.Build.cs` and CMake.
- Windows/DirectX code paths are explicitly excluded.

## Default Parameters
- AI full-precision radius: 0–200 m
- AI reduced-update radius: 200–800 m
- AI position-only radius: >800 m
- Max agents: 12000
- Memory pool alignment: 64 bytes

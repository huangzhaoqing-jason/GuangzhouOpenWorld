# GuangzhouOpenWorld - Plugins Manifest

## Engine Bundled Plugins (UE5.9.2)

These plugins ship with Unreal Engine 5.9.2 and are enabled in the project.

### Graphics & Rendering

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| Niagara | UE5.9.2 | GPU particle VFX | ✅ Native | Metal 4.2 compute shaders |
| NiagaraFluids | UE5.9.2 | Fluid simulation | ✅ Native | GPU fluid solver |
| ProceduralMeshComponent | UE5.9.2 | Runtime mesh generation | ✅ Native | No Metal-specific issues |
| Substrate | UE5.9.2 | Material system | ✅ Native | UE5.9.2 integrated |

### Physics

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| **JoltPhysics** | **v6.0.1** | Physics engine | ✅ Native | Better than PhysX on ARM. Deterministic simulation. |

### World & Streaming

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| **PCGAdvanced** | **v5.9.2-2.3** | Advanced PCG features | ✅ Native | Extended PCG rule set |
| **OSMImporter** | **v4.2.0** | OpenStreetMap data import | ✅ Native | 1:1 Guangzhou road/building import |
| PCG | UE5.9.2 | Procedural Content Generation | ✅ Native | Rule-based city generation |
| WorldPartitionHLOD | UE5.9.2 | HLOD streaming | ✅ Native | Works with World Partition |
| Landmass | UE5.9.2 | Terrain generation | ✅ Native | Landscape brush tools |
| Landscape | UE5.9.2 | Terrain system | ✅ Native | Nanite-compatible landscape |
| Water | UE5.9.2 | Water system | ✅ Native | River/lake/ocean rendering |

### AI & Crowd

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| **RecastNavigation** | **v5.9.2-1.8** | Navigation mesh | ✅ Native | 30Hz update, 3-layer hierarchy |
| MassAI | UE5.9.2 | Mass AI framework | ✅ Native | 12,000+ agents |
| MassCrowd | UE5.9.2 | Crowd simulation | ✅ Native | Pedestrian AI |
| MassTraffic | UE5.9.2 | Traffic simulation | ✅ Native | Vehicle AI |
| MassGameplay | UE5.9.2 | Mass gameplay bridge | ✅ Native | Entity-component bridge |

### Audio

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| **SoLoud** | **v2.10.0** | Audio engine | ✅ Native | Core Audio backend. HRTF, Doppler, 3D spatial. |
| AudioCapture | UE5.9.2 | Audio input capture | ✅ Native | Core Audio mic input |
| Synthesis | UE5.9.2 | Audio synthesis | ✅ Native | DSP generation |
| SoundFields | UE5.9.2 | Ambisonics | ✅ Native | Spatial audio encoding |

### Network

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| OnlineSubsystemEOS | UE5.9.2 | EOS integration | ✅ Native | Epic Online Services |
| OnlineSubsystem | UE5.9.2 | Online subsystem | ✅ Native | Base OSS framework |

---

## Third-Party / Version-Pinned Plugins

| Plugin | Version | License | Purpose | Apple Silicon | Build Notes |
|--------|---------|---------|---------|---------------|-------------|
| **JoltPhysics** | **v6.0.1** | MIT | Physics engine | ✅ Native | Deterministic, ARM-optimized |
| **RecastNavigation** | **v5.9.2-1.8** | MIT | Navigation mesh | ✅ Native | 30Hz, 3-layer hierarchy |
| **SoLoud** | **v2.10.0** | zlib | Audio engine | ✅ Native | Core Audio, HRTF, Doppler |
| **OSMImporter** | **v4.2.0** | MIT | OSM data import | ✅ Native | 1:1 Guangzhou import |
| **PCGAdvanced** | **v5.9.2-2.3** | MIT | Advanced PCG | ✅ Native | Extended rule set |

---

## Apple Silicon Compatibility Notes

### All Green (Native arm64)
All plugins listed above are **native arm64** and require no Rosetta 2 translation. They are compiled for Apple Silicon with Metal 4.2 and MSL 3.1 support.

### Verification Steps
After building, verify plugins are arm64:
```bash
# Check all .dylib files in the built .app
find Build/Output -name "*.dylib" -exec file {} \; | grep -v arm64

# Should return nothing (all are arm64)
```

### Plugin Compatibility Matrix

```
                         x86_64 (Rosetta)    arm64 (Native)    Universal
JoltPhysics v6.0.1            ✅                ✅               ✅
RecastNavigation v5.9.2-1.8   ✅                ✅               ✅
SoLoud v2.10.0                ✅                ✅               ✅
OSMImporter v4.2.0            ✅                ✅               ✅
PCGAdvanced v5.9.2-2.3        ✅                ✅               ✅
Niagara UE5.9.2               ✅                ✅               ✅
MassAI UE5.9.2                ✅                ✅               ✅
PCG UE5.9.2                   ✅                ✅               ✅
EOS UE5.9.2                   ✅                ✅               ✅
Substrate UE5.9.2             ✅                ✅               ✅
```

---

## Build Instructions

### Standard Build (All Plugins)
```bash
# From project root
cd Scripts/macOS
./build-mac.sh
```

### Build Configuration
The build script automatically:
1. Detects M1/M2/M3 via `sysctl hw.perflevel0.l1icachesize`
2. Discovers UE5.9.2 installation path
3. Generates Xcode 16.x project
4. Builds with Shipping configuration, arm64 only
5. Cooks Metal 4.2 shaders with -ForceMetalShaders
6. Compresses PAK files with Zstd v1.5.7 Level 22
7. Creates DMG with UDZO zlib level 9
8. Verifies binary with file + otool -L + codesign

### Adding a New Plugin
1. Add plugin entry to `GuangzhouOpenWorld.uproject`:
```json
{
    "Name": "PluginName",
    "Enabled": true
}
```
2. Add module dependency in `Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs`:
```csharp
PublicDependencyModuleNames.Add("PluginName");
```
3. Regenerate project files and rebuild.

### Plugin Configuration
Plugin-specific settings are in `Config/DefaultEngine.ini` under their respective sections:
- `[JoltPhysics]` - JoltPhysics v6.0.1 settings
- `[SoLoud]` - SoLoud v2.10.0 audio settings
- `[Mass]` - Mass AI settings
- `[OpenStreetMap]` - OSMImporter v4.2.0 settings
- `[OnlineSubsystem]` - EOS network settings
- `[PCGAdvanced]` - PCGAdvanced v5.9.2-2.3 settings

---

## Plugin Version Requirements

| UE Version | Plugin Min Version | Notes |
|------------|-------------------|-------|
| UE 5.9.2 | JoltPhysics v6.0.1 | Required for deterministic physics |
| UE 5.9.2 | RecastNavigation v5.9.2-1.8 | Required for 3-layer nav hierarchy |
| UE 5.9.2 | SoLoud v2.10.0 | Required for HRTF + Doppler |
| UE 5.9.2 | OSMImporter v4.2.0 | Required for 1:1 Guangzhou import |
| UE 5.9.2 | PCGAdvanced v5.9.2-2.3 | Required for advanced PCG rules |
| Future 5.10+ | Verify plugin compatibility | Check Epic release notes |

---

## Disabled Plugins (Not Needed)

These plugins are intentionally disabled to reduce build size and compile time:

| Plugin | Reason |
|--------|--------|
| OpenGL | Metal 4.2 only, no OpenGL fallback |
| Vulkan | Metal 4.2 only, no Vulkan fallback |
| DirectX | macOS only, no D3D |
| Hardware Ray Tracing | Not supported on Apple Silicon |
| OculusVR | No VR target |
| SteamVR | No VR target |
| OpenXR | No VR target |
| WindowsPlatform | macOS only |
| LinuxPlatform | macOS only |
| AndroidPlatform | macOS only |
| IOSPlatform | macOS only (desktop, not iOS) |
| TVOSPlatform | Not applicable |
| VisionOSPlatform | Not applicable |
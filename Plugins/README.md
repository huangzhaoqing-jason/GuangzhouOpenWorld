# GuangzhouOpenWorld - Plugins Manifest

## Engine Bundled Plugins (UE5.8)

These plugins ship with Unreal Engine 5.8 and are enabled in the project.

### Graphics & Rendering

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| Niagara | UE5.8 | GPU particle VFX | ✅ Native | Metal 4 compute shaders |
| NiagaraFluids | UE5.8 | Fluid simulation | ✅ Native | GPU fluid solver |
| ProceduralMeshComponent | UE5.8 | Runtime mesh generation | ✅ Native | No Metal-specific issues |

### Physics

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| JoltPhysics | UE5.8 | Physics engine | ✅ Native | Better than PhysX on ARM. Deterministic simulation. |

### World & Streaming

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| PCG | UE5.8 | Procedural Content Generation | ✅ Native | Rule-based city generation |
| WorldPartitionHLOD | UE5.8 | HLOD streaming | ✅ Native | Works with World Partition |
| Landmass | UE5.8 | Terrain generation | ✅ Native | Landscape brush tools |
| Landscape | UE5.8 | Terrain system | ✅ Native | Nanite-compatible landscape |
| Water | UE5.8 | Water system | ✅ Native | River/lake/ocean rendering |

### AI & Crowd

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| MassAI | UE5.8 | Mass AI framework | ✅ Native | 8,000+ agents |
| MassCrowd | UE5.8 | Crowd simulation | ✅ Native | Pedestrian AI |
| MassTraffic | UE5.8 | Traffic simulation | ✅ Native | Vehicle AI |
| MassGameplay | UE5.8 | Mass gameplay bridge | ✅ Native | Entity-component bridge |

### Audio

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| AudioCapture | UE5.8 | Audio input capture | ✅ Native | Core Audio mic input |
| Synthesis | UE5.8 | Audio synthesis | ✅ Native | DSP generation |
| SoundFields | UE5.8 | Ambisonics | ✅ Native | Spatial audio encoding |
| SoLoud | UE5.8 | Audio engine | ✅ Native | Core Audio backend. HRTF, Doppler, 3D spatial. |

### Network

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| OnlineSubsystemEOS | UE5.8 | EOS integration | ✅ Native | Epic Online Services |
| OnlineSubsystem | UE5.8 | Online subsystem | ✅ Native | Base OSS framework |

### Data

| Plugin | Version | Purpose | Apple Silicon | Notes |
|--------|---------|---------|---------------|-------|
| OpenStreetMap | UE5.8 | OSM data import | ✅ Native | 1:1 Guangzhou road/building import |

---

## Third-Party / Open-Source Plugins

### Currently Used

| Plugin | Version | License | Purpose | Apple Silicon | Build Notes |
|--------|---------|---------|---------|---------------|-------------|
| *(none yet)* | - | - | - | - | - |

### Recommended for Future Integration

| Plugin | Version | License | Purpose | Apple Silicon | Build Notes |
|--------|---------|---------|---------|---------------|-------------|
| DLSS / FSR | N/A | N/A | Upscaling | ❌ Not available | Apple Silicon uses TSR instead |
| Quixel Bridge | UE5.8 | Epic EULA | Megascans assets | ✅ Native | Built into UE5.8 |
| MetaHuman | UE5.8 | Epic EULA | Character creation | ✅ Native | Built into UE5.8 |
| RiderLink | Latest | JetBrains | IDE integration | ✅ Native | Rider for Unreal |

---

## Apple Silicon Compatibility Notes

### All Green (Native arm64)
All plugins listed above are **native arm64** and require no Rosetta 2 translation. They ship with UE5.8 and are compiled for Apple Silicon by Epic Games.

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
JoltPhysics               ✅                ✅               ✅
Niagara                   ✅                ✅               ✅
MassAI                    ✅                ✅               ✅
PCG                       ✅                ✅               ✅
SoLoud                    ✅                ✅               ✅
EOS                       ✅                ✅               ✅
OpenStreetMap              ✅                ✅               ✅
```

---

## Build Instructions

### Standard Build (All Plugins)
```bash
# From project root
cd Scripts
./build-mac.sh
```

### Building Individual Plugins
UE5.8 automatically compiles all enabled plugins during the build process. No manual plugin compilation is needed.

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
- `[JoltPhysics]` - Jolt physics settings
- `[SoLoud]` - SoLoud audio settings
- `[Mass]` - Mass AI settings
- `[OpenStreetMap]` - OSM import settings
- `[OnlineSubsystem]` - EOS network settings

---

## Plugin Version Requirements

| UE Version | Plugin Min Version | Notes |
|------------|-------------------|-------|
| UE 5.8 | All plugins ship with engine | No external version management needed |
| Future 5.9+ | Verify plugin compatibility | Check Epic release notes for breaking changes |

---

## Disabled Plugins (Not Needed)

These plugins are intentionally disabled to reduce build size and compile time:

| Plugin | Reason |
|--------|--------|
| OpenGL | Metal 4 only, no OpenGL fallback |
| Vulkan | Metal 4 only, no Vulkan fallback |
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
# SoLoud 2026 Audio Plugin

## Version
- Locked: 2026
- Target: UE5.8 / macOS Apple Silicon (arm64)

## Modules
- `FZSoLoudMemoryManager` — pre-allocated pools for streams, SFX, general audio data.
- `UGZAudioMixerBridge` — channel groups, spatial audio, Doppler, global pause/resume.
- `UGZSoLoudSelfCheck` — four-layer MCP self-check entry point.

## Build Notes
- `SOLOUD_COREAUDIO=1` forces Apple Core Audio backend.
- Windows backends (WinMM/DirectSound/WASAPI) excluded.
- Decoders limited to WAV and OGG to reduce binary size.

## Channel Groups
- Master, Ambient, Weather, Vehicle, Voice
- Volume curves use logarithmic-to-linear mapping.

#!/usr/bin/env bash
# Sync Docs/Data/missions_act1.json → Content/Data + MacApp Resources
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/Docs/Data/missions_act1.json"
test -f "$SRC"
mkdir -p "$ROOT/Content/Data" "$ROOT/MacApp/Sources/GuangzhouOpenWorldApp/Resources"
cp "$SRC" "$ROOT/Content/Data/missions_act1.json"
cp "$SRC" "$ROOT/MacApp/Sources/GuangzhouOpenWorldApp/Resources/missions_act1.json"
echo "Synced missions_act1.json → Content/Data + MacApp Resources"

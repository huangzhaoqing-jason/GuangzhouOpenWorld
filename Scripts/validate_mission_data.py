#!/usr/bin/env python3
"""Validate Docs/Data/missions_act{1,2}.json + district_id_map.json (no UE required)."""
from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "Docs" / "Data"
DISTRICTS = DATA / "district_id_map.json"
MISSION_FILES = [DATA / "missions_act1.json", DATA / "missions_act2.json"]
REQUIRED_TYPES = {"MainStory", "Side", "Daily", "Racing", "Collection", "Wanted"}
REQUIRED_STATUS = {"Locked", "Available", "Active", "Completed", "Failed"}


def fail(msg: str) -> None:
    print(f"FAIL: {msg}", file=sys.stderr)
    sys.exit(1)


def validate_missions_file(path: Path, district_ids: set[str], global_ids: set[str]) -> int:
    if not path.is_file():
        fail(f"missing {path}")
    doc = json.loads(path.read_text(encoding="utf-8"))
    missions = doc.get("missions") or []
    if len(missions) < 1:
        fail(f"{path.name}: missions array empty")

    for m in missions:
        mid = m.get("id")
        if not mid:
            fail(f"{path.name}: mission missing id")
        if mid in global_ids:
            fail(f"duplicate mission id across acts: {mid}")
        global_ids.add(mid)
        if m.get("type") not in REQUIRED_TYPES:
            fail(f"{mid}: bad type {m.get('type')}")
        if m.get("status") not in REQUIRED_STATUS:
            fail(f"{mid}: bad status {m.get('status')}")
        did = m.get("district_id")
        if did not in district_ids:
            fail(f"{mid}: district_id {did} not in district_id_map.json")
        objs = m.get("objectives") or []
        if not objs:
            fail(f"{mid}: no objectives")

    act_district = doc.get("district_id")
    if act_district and act_district not in district_ids:
        fail(f"{path.name}: act district_id {act_district} unknown")
    return len(missions)


def main() -> None:
    if not DISTRICTS.is_file():
        fail(f"missing {DISTRICTS}")
    districts_doc = json.loads(DISTRICTS.read_text(encoding="utf-8"))
    district_ids = {d["district_id"] for d in districts_doc["districts"]}

    global_ids: set[str] = set()
    total = 0
    for path in MISSION_FILES:
        total += validate_missions_file(path, district_ids, global_ids)

    print(f"OK: {total} missions across {len(MISSION_FILES)} acts, {len(district_ids)} districts")


if __name__ == "__main__":
    main()

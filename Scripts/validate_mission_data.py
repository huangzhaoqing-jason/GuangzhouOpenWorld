#!/usr/bin/env python3
"""Validate Docs/Data/missions_act1.json + district_id_map.json (no UE required)."""
from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MISSIONS = ROOT / "Docs" / "Data" / "missions_act1.json"
DISTRICTS = ROOT / "Docs" / "Data" / "district_id_map.json"
REQUIRED_TYPES = {"MainStory", "Side", "Daily", "Racing", "Collection", "Wanted"}
REQUIRED_STATUS = {"Locked", "Available", "Active", "Completed", "Failed"}


def fail(msg: str) -> None:
    print(f"FAIL: {msg}", file=sys.stderr)
    sys.exit(1)


def main() -> None:
    if not MISSIONS.is_file():
        fail(f"missing {MISSIONS}")
    if not DISTRICTS.is_file():
        fail(f"missing {DISTRICTS}")

    missions_doc = json.loads(MISSIONS.read_text(encoding="utf-8"))
    districts_doc = json.loads(DISTRICTS.read_text(encoding="utf-8"))

    district_ids = {d["district_id"] for d in districts_doc["districts"]}
    missions = missions_doc.get("missions") or []
    if len(missions) < 1:
        fail("missions array empty")

    ids = set()
    for m in missions:
        mid = m.get("id")
        if not mid:
            fail("mission missing id")
        if mid in ids:
            fail(f"duplicate mission id {mid}")
        ids.add(mid)
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

    act_district = missions_doc.get("district_id")
    if act_district and act_district not in district_ids:
        fail(f"act district_id {act_district} unknown")

    print(f"OK: {len(missions)} missions, {len(district_ids)} districts")


if __name__ == "__main__":
    main()

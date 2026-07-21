#!/usr/bin/env bash
# Example: crop an already-downloaded OSM extract to Zhujiang New Town soft bbox.
# Does NOT download Geofabrik dumps. Do NOT commit .osm.pbf / large GeoJSON.
#
# Prereq: ogr2ogr (GDAL). Optional: osmium.
# Usage:
#   ./Scripts/osm_crop_zhujiang_example.sh /path/to/guangdong-latest.osm.pbf /path/to/out_dir
set -euo pipefail

IN="${1:-}"
OUT_DIR="${2:-./local_osm_out}"

if [[ -z "$IN" || ! -f "$IN" ]]; then
  echo "Usage: $0 <input.osm.pbf|input.osm> [out_dir]" >&2
  echo "Download extracts yourself (e.g. Geofabrik). Never commit the pbf." >&2
  exit 1
fi

mkdir -p "$OUT_DIR"

# Soft bbox around Zhujiang New Town / Huacheng axis (approx WGS84).
# West, South, East, North — tune using Docs/Data/GUANGZHOU_DISTRICTS.md
MINX=113.300
MINY=23.100
MAXX=113.345
MAXY=23.140

OUT_GEOJSON="$OUT_DIR/zhujiang_new_town_roads.geojson"

echo "Cropping $IN → $OUT_GEOJSON"
ogr2ogr -f GeoJSON "$OUT_GEOJSON" "$IN" \
  -spat "$MINX" "$MINY" "$MAXX" "$MAXY" \
  -progress \
  || {
    echo "ogr2ogr failed. If input is .pbf, convert first or use osmium extract." >&2
    echo "Example osmium: osmium extract -b ${MINX},${MINY},${MAXX},${MAXY} -o $OUT_DIR/zjn.osm.pbf \"$IN\"" >&2
    exit 1
  }

echo "Done. Import GeoJSON into UE/PCG later. Keep $OUT_DIR out of git (.gitignore recommended)."

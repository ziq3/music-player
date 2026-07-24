#!/usr/bin/env python3
"""
FMA Dataset Seed Script
Populates SQLite database (music.db) from an unzipped FMA dataset directory.

Usage:
    python3 seed.py /path/to/fma_small [--db music.db] [--metadata tracks.csv]
    python3 seed.py --fma-dir /path/to/fma_small [--db music.db]
"""

import os
import sys
import csv
import sqlite3
import argparse
from pathlib import Path

def parse_args():
    parser = argparse.ArgumentParser(
        description="Populate SQLite database from an unzipped FMA dataset directory."
    )
    parser.add_argument(
        "fma_dir_pos",
        nargs="?",
        default=None,
        help="Path to the unzipped FMA dataset folder (e.g. /path/to/fma_small)"
    )
    parser.add_argument(
        "--fma-dir",
        type=str,
        default=None,
        help="Path to the unzipped FMA dataset folder (e.g. /path/to/fma_small)"
    )
    parser.add_argument(
        "--db",
        type=str,
        default="music.db",
        help="Path to target SQLite database file (default: music.db)"
    )
    parser.add_argument(
        "--metadata",
        type=str,
        default=None,
        help="Optional path to fma tracks.csv metadata file"
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Limit maximum number of tracks to seed (e.g., --limit 100 for dev/testing)"
    )

    args = parser.parse_args()

    # Determine fma_dir from positional or flag argument
    fma_dir_path = args.fma_dir or args.fma_dir_pos
    if not fma_dir_path:
        parser.error("Please provide the path to the unzipped FMA folder (e.g. python3 seed.py /path/to/fma_small)")

    args.fma_dir = fma_dir_path
    return args

def init_db(db_path: Path) -> sqlite3.Connection:
    """Initializes SQLite database and tables with WAL mode for fast startup."""
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    # WAL mode for sub-second startup and concurrent reads
    cursor.execute("PRAGMA journal_mode=WAL;")
    cursor.execute("PRAGMA synchronous=NORMAL;")

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS tracks (
            track_id INTEGER PRIMARY KEY,
            title TEXT NOT NULL,
            artist_name TEXT,
            album_title TEXT,
            genre TEXT,
            duration REAL DEFAULT 30.0,
            file_path TEXT NOT NULL
        );
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS playlists (
            playlist_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS playlist_tracks (
            playlist_id INTEGER,
            track_id INTEGER,
            position INTEGER,
            PRIMARY KEY (playlist_id, track_id),
            FOREIGN KEY (playlist_id) REFERENCES playlists(playlist_id) ON DELETE CASCADE,
            FOREIGN KEY (track_id) REFERENCES tracks(track_id) ON DELETE CASCADE
        );
    """)

    # Performance indexes for instant searching and filtering
    cursor.execute("CREATE INDEX IF NOT EXISTS idx_tracks_title ON tracks(title);")
    cursor.execute("CREATE INDEX IF NOT EXISTS idx_tracks_artist ON tracks(artist_name);")
    cursor.execute("CREATE INDEX IF NOT EXISTS idx_tracks_genre ON tracks(genre);")

    conn.commit()
    return conn

def load_metadata_csv(metadata_path: Path) -> dict:
    """Parses FMA tracks.csv metadata if provided.
    Supports official FMA multi-header tracks.csv as well as simple CSV formats.
    """
    metadata = {}
    if not metadata_path or not metadata_path.exists():
        return metadata

    print(f"📖 Reading metadata CSV from '{metadata_path.resolve()}'...")
    try:
        with open(metadata_path, 'r', encoding='utf-8', errors='ignore') as f:
            reader = csv.reader(f)
            header_rows = []
            for _ in range(5):
                try:
                    row = next(reader)
                    header_rows.append(row)
                except StopIteration:
                    break

            if not header_rows:
                return metadata

            title_idx = None
            artist_idx = None
            album_idx = None
            genre_idx = None
            skip_lines = 0

            # 1. Try detecting official FMA multi-header tracks.csv (3 header rows)
            if len(header_rows) >= 3:
                r0, r1, r2 = header_rows[0], header_rows[1], header_rows[2]
                max_cols = max(len(r0), len(r1), len(r2))
                for i in range(max_cols):
                    h0 = r0[i].strip().lower() if i < len(r0) else ""
                    h1 = r1[i].strip().lower() if i < len(r1) else ""
                    h2 = r2[i].strip().lower() if i < len(r2) else ""

                    if h0 == "track" and h1 == "title" and title_idx is None:
                        title_idx = i
                    elif h0 == "artist" and h1 == "name" and artist_idx is None:
                        artist_idx = i
                    elif h0 == "album" and h1 == "title" and album_idx is None:
                        album_idx = i
                    elif h0 == "track" and (h1 in ("genre_top", "genres")) and genre_idx is None:
                        genre_idx = i

                if title_idx is not None or artist_idx is not None:
                    skip_lines = 3

            # 2. Try single-header CSV if not FMA 3-header
            if skip_lines == 0 and len(header_rows) >= 1:
                r0 = [c.strip().lower() for c in header_rows[0]]
                for i, col in enumerate(r0):
                    if col in ("title", "track_title"):
                        title_idx = i
                    elif col in ("artist", "artist_name"):
                        artist_idx = i
                    elif col in ("album", "album_title"):
                        album_idx = i
                    elif col in ("genre", "genre_top", "genres"):
                        genre_idx = i
                if title_idx is not None or artist_idx is not None:
                    skip_lines = 1

            # Fallbacks if columns were not found
            if title_idx is None: title_idx = 1
            if artist_idx is None: artist_idx = 2
            if album_idx is None: album_idx = 3
            if genre_idx is None: genre_idx = 4

            # Reset reader and skip header lines
            f.seek(0)
            reader = csv.reader(f)
            for _ in range(skip_lines):
                next(reader, None)

            for row in reader:
                if not row or not row[0].strip().isdigit():
                    continue
                try:
                    track_id = int(row[0].strip())
                except ValueError:
                    continue

                title = row[title_idx].strip() if title_idx < len(row) and row[title_idx].strip() else f"Track {track_id:06d}"
                artist = row[artist_idx].strip() if artist_idx < len(row) and row[artist_idx].strip() else "FMA Artist"
                album = row[album_idx].strip() if album_idx < len(row) and row[album_idx].strip() else "FMA Collection"
                genre = row[genre_idx].strip() if genre_idx < len(row) and row[genre_idx].strip() else "Unknown"

                metadata[track_id] = {
                    "title": title,
                    "artist_name": artist,
                    "album_title": album,
                    "genre": genre
                }
        print(f"📊 Loaded metadata for {len(metadata)} tracks from CSV.")
    except Exception as e:
        print(f"⚠️ Warning: Could not parse metadata CSV: {e}")
    return metadata

def seed_database(fma_dir: Path, db_path: Path, metadata_path: Path = None, limit: int = None):
    if not fma_dir.exists() or not fma_dir.is_dir():
        print(f"❌ Error: Unzipped FMA folder '{fma_dir.resolve()}' does not exist or is not a directory!")
        sys.exit(1)

    print(f"📂 Scanning unzipped FMA directory: '{fma_dir.resolve()}'")
    mp3_files = sorted(list(fma_dir.rglob("*.mp3")))

    if not mp3_files:
        print(f"⚠️ Warning: No .mp3 files found in '{fma_dir.resolve()}'.")
        sys.exit(1)

    print(f"🎵 Found {len(mp3_files)} MP3 audio files total.")

    if limit is not None and limit > 0:
        mp3_files = mp3_files[:limit]
        print(f"🔹 Limit applied: Seeding first {len(mp3_files)} tracks.")

    metadata = load_metadata_csv(metadata_path) if metadata_path else {}

    print(f"💾 Opening SQLite database at '{db_path.resolve()}'...")
    conn = init_db(db_path)
    cursor = conn.cursor()

    records = []
    for mp3_path in mp3_files:
        track_id_str = mp3_path.stem
        try:
            track_id = int(track_id_str)
        except ValueError:
            continue

        meta = metadata.get(track_id, {})
        title = meta.get("title", f"Track {track_id:06d}")
        artist_name = meta.get("artist_name", "FMA Artist")
        album_title = meta.get("album_title", "FMA Small Collection")
        genre = meta.get("genre", "Unknown")
        duration = 30.0
        abs_path = str(mp3_path.resolve())

        records.append((track_id, title, artist_name, album_title, genre, duration, abs_path))

    print(f"⚡ Batch inserting {len(records)} tracks into database...")
    cursor.execute("BEGIN TRANSACTION;")
    cursor.executemany("""
        INSERT OR REPLACE INTO tracks (track_id, title, artist_name, album_title, genre, duration, file_path)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    """, records)
    conn.commit()
    conn.close()

    print(f"✅ Seeding successful! Populated {len(records)} tracks into '{db_path.resolve()}'.")

def main():
    args = parse_args()
    fma_dir = Path(args.fma_dir)
    db_path = Path(args.db)
    metadata_path = Path(args.metadata) if args.metadata else None
    limit = args.limit

    seed_database(fma_dir, db_path, metadata_path, limit)

if __name__ == "__main__":
    main()


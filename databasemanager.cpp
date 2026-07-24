#include "databasemanager.h"
#include <QSqlError>
#include <QSqlQuery>
DatabaseManager::DatabaseManager(const QString &dbPath) : m_dbPath(dbPath)
{
    if (open()) {
        initSchema();
    }
}
bool DatabaseManager::initSchema()
{
    QSqlQuery query(m_db);

    bool success = query.exec(R"(
        CREATE TABLE IF NOT EXISTS tracks (
            track_id INTEGER PRIMARY KEY,
            title TEXT NOT NULL,
            artist_name TEXT,
            album_title TEXT,
            genre TEXT,
            duration REAL,
            file_path TEXT NOT NULL
        );
    )");
    if (!success) {
        qDebug() << query.lastError().text();
        return false;
    }

    success = query.exec(R"(
        CREATE TABLE IF NOT EXISTS playlists (
            playlist_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )");
    if (!success) {
        qDebug() << query.lastError().text();
        return false;
    }
    success = query.exec(R"(
        CREATE TABLE IF NOT EXISTS playlist_tracks (
            playlist_id INTEGER,
            track_id INTEGER,
            position INTEGER,
            PRIMARY KEY (playlist_id, track_id)
        );
    )");
    if (!success) {
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::open()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);
    if (!m_db.open()) {
        qDebug() << m_db.lastError().text();
        return false;
    }
    QSqlQuery query(m_db);
    query.exec("PRAGMA journal_mode=WAL;");
    query.exec("PRAGMA foreign_keys=ON;");
    return true;
}

QList<Track> DatabaseManager::getAllTracks()
{
    QList<Track> list;
    QSqlQuery query("SELECT * FROM tracks", m_db);
    while (query.next()) {
        int trackId = query.value(0).toInt();
        QString title = query.value(1).toString();
        QString artistName = query.value(2).toString();
        QString albumTitle = query.value(3).toString();
        QString genre = query.value(4).toString();
        double durationquery = query.value(5).toDouble();
        QString filePath = query.value(6).toString();
        list.push_back(
                Track{ trackId, title, artistName, albumTitle, genre, durationquery, filePath });
    }
    return list;
}
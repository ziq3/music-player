#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QList>

struct Track
{
    int trackId;
    QString title;
    QString artistName;
    QString albumTitle;
    QString genre;
    double duration;
    QString filePath;
};
struct Playlist
{
    int playlistId;
    QString name;
    QString createdAt;
};
class DatabaseManager
{
public:
    explicit DatabaseManager(const QString &dbPath = "music.db");
    ~DatabaseManager();

    bool open();
    void close();
    bool initSchema();
    QList<Track> getAllTracks();

private:
    QString m_dbPath;
    QSqlDatabase m_db;
};

#endif

#ifndef TMSBASEADAPTER_H
#define TMSBASEADAPTER_H

#include <QObject>
#include <QLocale>
#include <QUrl>
#include <QSize>

class TmsBaseAdapter : public QObject
{
    Q_OBJECT

    friend class TileLoader;

public:
    TmsBaseAdapter(const QUrl& url);
    virtual ~TmsBaseAdapter();
    virtual QUrl query(int x, int y, int z) const;
    QString serverHost() const;
    QString serverPath() const;
    QString serverUrl() const;
    QString scheme() const;
    int serverPort() const;

    void setMinZoomLevel(uint minZlevel);
    void setMaxZoomLevel(uint maxZlevel);
    void setRetryLimit(uint retryLimit);
    void setTimeoute(uint timeouteSec);
    void setScaleForBottomLevels(bool on_off);
    void setLoadersLimit(int loadersLimit);
    void setExtension(const QString &extension);
    void setTileSize(const QSize &tileSize);
    void setZLevelShift(int zLevelShift);
    void setZLevelShiftForY(int zLevelShiftForY);

    uint minZoomLevel() const;
    uint maxZoomLevel() const;
    uint retryLimit() const;
    uint timeouteSec() const;
    bool scaleForBottomLevels() const;
    int loadersLimit()const;
    QString extension()const;
    QSize tileSize()const;
    int zLevelShift()const;
    int zLevelShiftForY()const;

    uint loadersInWork() const;
    bool hasFreeLoaders() const;

protected:
    int order[3][2];
    QLocale loc;

    void incLoadersInWork();
    void decLoadersInWork();

private:
    const QString _serverUrl;
    QString _scheme;
    QString _serverHost;
    QString _serverPath;
    int _serverPort;
    uint mMinZoomLevel, mMaxZoomLevel, mRetryLimit, mTimeoute;
    bool mScaleForBottomLevels;
    int mLoadersLimit;
    uint mLoadersInWork;
    QString mExtension;
    QSize mTileSize;
    int mZLevelShift, mZLevelShiftForY;
};

#endif










#ifndef DATASERIALIZER_H
#define DATASERIALIZER_H

#include <QDateTime>
#include <QObject>
#include <QVariant>

class DataSerializer : public QObject
{
    Q_OBJECT

public:
    enum DataType
    {
        DT_QUINT8,
        DT_QUINT16,
        DT_QUINT32,
        DT_QUINT64,
        DT_QINT8,
        DT_QINT16,
        DT_QINT32,
        DT_QINT64,
        DT_QREAL,
        DT_QString
    };

    explicit DataSerializer(QObject *parent = 0);
    static void       serializeRAW(bool data, QByteArray & srcArr);
    static void       serializeRAW(quint8 data, QByteArray & srcArr);
    static void       serializeRAW(quint16 data, QByteArray & srcArr);
    static void       serializeRAW(quint32 data, QByteArray & srcArr);
    static void       serializeRAW(quint64 data, QByteArray & srcArr);
    static void       serializeRAW(qreal data, QByteArray & srcArr);
    static void       serializeRAW(qint8 data, QByteArray & srcArr);
    static void       serializeRAW(qint16 data, QByteArray & srcArr);
    static void       serializeRAW(qint32 data, QByteArray & srcArr);
    static void       serializeRAW(qint64 data, QByteArray & srcArr);
    static void       serializeRAW(QDateTime data, QByteArray & srcArr);
    static void       serializeRAW(QString data, QByteArray & srcArr);
    static void       serializeRAW(QList<quint64> data, QByteArray & srcArr);
    static void       serializeRAW(QList<quint32> data, QByteArray & srcArr);
    static void       serializeRAW(QVariant data, QByteArray & srcArr);
    static void       serializeRAW(QList<QString> data, QByteArray & srcArr);
    static void       serializeRAW(const QByteArray &dataArr, QByteArray & srcArr);
    static QByteArray serializeJSONRAW(QVariant data);
    static bool       deserializeBOOLRAW(QByteArray & srcArr);
    static quint8     deserializeUINT8RAW(QByteArray & srcArr);
    static quint16    deserializeUINT16RAW(QByteArray & srcArr);
    static quint32    deserializeUINT32RAW(QByteArray & srcArr);
    static quint64    deserializeUINT64RAW(QByteArray & srcArr);
    static QString    deserializeQStringRAW(QByteArray & srcArr);
    static qreal      deserializeQREALRAW(QByteArray & srcArr);
    static qint8      deserializeINT8RAW(QByteArray & srcArr);
    static qint16     deserializeINT16RAW(QByteArray & srcArr);
    static qint32     deserializeINT32RAW(QByteArray & srcArr);
    static qint64     deserializeINT64RAW(QByteArray & srcArr);
    static QDateTime  deserializeQDATETIMERAW(QByteArray & srcArr);

    static QList<quint64> deserializeQLISTQUINT64RAW(QByteArray & srcArr);

    static QList<quint32> deserializeQLISTQUINT32RAW(QByteArray & srcArr);
    static QVariant       deserializeQVARIANTRAW(QByteArray & srcArr);

    static QList<QString> deserializeQLISTQStringRAW(QByteArray & srcArr);
    static QByteArray     deserializeQByteArrayRAW(QByteArray & srcArr);

    static quint16 swapBytes(quint16);
    static QVariant deserializeJSONRAW(const QByteArray & srcArr);

signals:
public slots:
};


class JSonSerializerPrivate
{
    friend class Object;
    friend class DataSerializer;

    typedef QVariantMap JsonObject;
    typedef QVariantList JsonArray;


    static QString getQVarTag();


    template<typename T>
    static void cloneList(QVariant &json, const T &list);

    template<typename T>
    static void cloneMap(QVariant &json, const T &map);

    template<typename T>
    static QByteArray serializeMap(const T &map, bool &success);
    static QString    sanitizeString(QString str);
    static QByteArray join(const QList<QByteArray> &list, const QByteArray &sep);
    static QVariant   parseValue(const QString &json, int &index, bool &success);
    static QVariant   parseObject(const QString &json, int &index, bool &success);
    static QVariant   parseArray(const QString &json, int &index, bool &success);
    static QVariant   parseString(const QString &json, int &index, bool &success);
    static QVariant   parseNumber(const QString &json, int &index);
    static int        lastIndexOfNumber(const QString &json, int index);
    static void       eatWhitespace(const QString &json, int &index);
    static int        lookAhead(const QString &json, int index);
    static int        nextToken(const QString &json, int &index);

/**
 * Clone a JSON object (makes a deep copy)
 *
 * \param data The JSON object
 */
    static QVariant clone(const QVariant &data);

/**
 * Insert value to JSON object (QVariantMap)
 *
 * \param v The JSON object
 * \param key The key
 * \param value The value
 */
    static void insert(QVariant &v, const QString &key, const QVariant &value);

/**
 * Append value to JSON array (QVariantList)
 *
 * \param v The JSON array
 * \param value The value
 */
    static void append(QVariant &v, const QVariant &value);

/**
 * Parse a JSON string
 *
 * \param json The JSON data
 */
    static QVariant parse(const QString &json);

/**
 * Parse a JSON string
 *
 * \param json The JSON data
 * \param success The success of the parsing
 */
    static QVariant parse(const QString &json, bool &success);

/**
 * This method generates a textual JSON representation
 *
 * \param data The JSON data generated by the parser.
 *
 * \return QByteArray Textual JSON representation in UTF-8
 */
    static QByteArray serialize(const QVariant &data);

/**
 * This method generates a textual JSON representation
 *
 * \param data The JSON data generated by the parser.
 * \param success The success of the serialization
 *
 * \return QByteArray Textual JSON representation in UTF-8
 */
    static QByteArray serialize(const QVariant &data, bool &success);

/**
 * This method generates a textual JSON representation
 *
 * \param data The JSON data generated by the parser.
 *
 * \return QString Textual JSON representation
 */
    static QString serializeStr(const QVariant &data);

/**
 * This method generates a textual JSON representation
 *
 * \param data The JSON data generated by the parser.
 * \param success The success of the serialization
 *
 * \return QString Textual JSON representation
 */
    static QString serializeStr(const QVariant &data, bool &success);

/**
 * This method sets date(time) format to be used for QDateTime::toString
 * If QString is empty, Qt::TextDate is used.
 *
 * \param format The JSON data generated by the parser.
 */
    static void setDateTimeFormat(const QString & format);
    static void setDateFormat(const QString & format);

/**
 * This method gets date(time) format to be used for QDateTime::toString
 * If QString is empty, Qt::TextDate is used.
 */
    static QString getDateTimeFormat();
    static QString getDateFormat();
};


/**
 * QVariant based Json object
 */
class Object : public QVariant
{
    friend class DataSerializer;
    friend class JSonSerializerPrivate;

    template<typename T>
    Object & insertKey(Object *ptr, const QString & key)
    {
        T *p = (T *)ptr->data();

        if(!p->contains(key)) p->insert(key, QVariant());

        return *reinterpret_cast<Object *>(&p->operator[](key));
    }

    template<typename T>


    void removeKey(Object *ptr, const QString & key)
    {
        T *p = (T *)ptr->data();

        p->remove(key);
    }

    Object() : QVariant() {}

    Object(const Object & ref) : QVariant(ref) {}

    Object & operator      =(const QVariant & rhs);
    Object & operator      [](const QString & key);
    const Object & operator[](const QString & key) const;
    void                   remove(const QString & key);
};


#endif // DATASERIALIZER_H

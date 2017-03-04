#include "dataserializer.h"
#include <QDateTime>
#include <QDebug>
#include <QRectF>
#include <QSizeF>
#include <QStringList>

static QString dateFormat, dateTimeFormat;

DataSerializer::DataSerializer(QObject *parent) :
    QObject(parent)
{
}


void DataSerializer::serializeRAW(bool data, QByteArray & srcArr)
{
    quint8 dt = 0;
    QByteArray tmp;

    if(data)
    {
        dt = 1;
        tmp = tmp.setRawData((char *)&dt, 1);
    }
    else
    {
        dt = 0;
        tmp = tmp.setRawData((char *)&dt, 1);
    }

    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(quint8 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 1);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(quint16 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 2);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(quint32 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 4);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(QList<quint64> data, QByteArray & srcArr)
{
    QByteArray tmp;
    quint32 count = data.count();

    tmp = tmp.setRawData((char *)&count, 4);
    srcArr = srcArr.append(tmp);

    foreach (quint64 val, data)
    {
        tmp = tmp.setRawData((char *)&val, 8);
        srcArr = srcArr.append(tmp);
    }
}


void DataSerializer::serializeRAW(QList<quint32> data, QByteArray & srcArr)
{
    QByteArray tmp;
    quint32 count = data.count();

    tmp = tmp.setRawData((char *)&count, 4);
    srcArr = srcArr.append(tmp);

    foreach (quint32 val, data)
    {
        tmp = tmp.setRawData((char *)&val, 4);
        srcArr = srcArr.append(tmp);
    }
}


void DataSerializer::serializeRAW(quint64 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 8);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(qreal data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 8);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(qint8 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 1);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(qint16 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 2);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(qint32 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 4);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(qint64 data, QByteArray & srcArr)
{
    QByteArray tmp;

    tmp = tmp.setRawData((char *)&data, 8);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(QString data, QByteArray & srcArr)
{
    QByteArray tmp;
    QByteArray arr = data.toUtf8();
    quint32 dataSize = arr.size();

    tmp = tmp.setRawData((char *)&dataSize, 4);
    srcArr = srcArr.append(tmp);
    srcArr = srcArr.append(arr);
}


void DataSerializer::serializeRAW(QDateTime data, QByteArray & srcArr)
{
    QByteArray tmp;
    quint64 tm = data.toMSecsSinceEpoch();

    tmp = tmp.setRawData((char *)&tm, 8);
    srcArr = srcArr.append(tmp);
}


void DataSerializer::serializeRAW(QVariant data, QByteArray & srcArr)
{
    quint32 tId = data.type();

    serializeRAW(tId, srcArr);


    switch(data.type())
    {
      case QVariant::Bool:
      {
          serializeRAW((quint8)(data.toBool()), srcArr);
      } break;
      case QVariant::Int:
      {
          serializeRAW((qint64)(data.toInt()), srcArr);
      } break;
      case QVariant::UInt:
      {
          serializeRAW((quint64)(data.toUInt()), srcArr);
      } break;
      case QVariant::Double:
      {
          serializeRAW((qreal)(data.toDouble()), srcArr);
      } break;
      case QVariant::LongLong:
      {
          serializeRAW((qint64)(data.toLongLong()), srcArr);
      } break;
      case QVariant::ULongLong:
      {
          serializeRAW((quint64)(data.toULongLong()), srcArr);
      } break;
      case QVariant::String:
      {
          serializeRAW((data.toString()), srcArr);
      } break;
      //    case QVariant::Char:
      //    {
      //        serializeRAW((quint16)(data.toChar()),srcArr);
      //    }break;
      case QVariant::DateTime:
      {
          serializeRAW(data.toDateTime(), srcArr);
      } break;
      default:
      {
          qWarning() << "DataSerializer::serializeRAW(QVariant): ERROR! unsupported datatype";
      } break;
    }
}


bool DataSerializer::deserializeBOOLRAW(QByteArray & srcArr)
{
    quint8 res = *(quint8 *)srcArr.left(1).data();

    srcArr = srcArr.remove(0, 1);

    if(0 == res)
    {
        return false;
    }

    return true;
}


quint8 DataSerializer::deserializeUINT8RAW(QByteArray & srcArr)
{
    quint8 res = *(quint8 *)srcArr.left(1).data();

    srcArr = srcArr.remove(0, 1);
    return res;
}


quint16 DataSerializer::deserializeUINT16RAW(QByteArray & srcArr)
{
    quint16 res = *(quint16 *)srcArr.left(2).data();

    srcArr = srcArr.remove(0, 2);
    return res;
}


quint32 DataSerializer::deserializeUINT32RAW(QByteArray & srcArr)
{
    quint32 res = *(quint32 *)srcArr.left(4).data();

    srcArr = srcArr.remove(0, 4);
    return res;
}


quint64 DataSerializer::deserializeUINT64RAW(QByteArray & srcArr)
{
    quint64 res = *(quint64 *)srcArr.left(8).data();

    srcArr = srcArr.remove(0, 8);
    return res;
}


QString DataSerializer::deserializeQStringRAW(QByteArray & srcArr)
{
    quint32 sz = *(quint32 *)srcArr.left(4).data();

    srcArr = srcArr.remove(0, 4);
    QString str = QString::fromUtf8(srcArr.left(sz).data());
    srcArr = srcArr.remove(0, sz);
    return str;
}


qreal DataSerializer::deserializeQREALRAW(QByteArray & srcArr)
{
    qreal res = *(qreal *)srcArr.left(8).data();

    srcArr = srcArr.remove(0, 8);
    return res;
}


qint8 DataSerializer::deserializeINT8RAW(QByteArray & srcArr)
{
    quint8 res = *(qint8 *)srcArr.left(1).data();

    srcArr = srcArr.remove(0, 1);
    return res;
}


qint16 DataSerializer::deserializeINT16RAW(QByteArray & srcArr)
{
    quint16 res = *(qint16 *)srcArr.left(2).data();

    srcArr = srcArr.remove(0, 2);
    return res;
}


qint32 DataSerializer::deserializeINT32RAW(QByteArray & srcArr)
{
    quint32 res = *(qint32 *)srcArr.left(4).data();

    srcArr = srcArr.remove(0, 4);
    return res;
}


qint64 DataSerializer::deserializeINT64RAW(QByteArray & srcArr)
{
    quint64 res = *(qint64 *)srcArr.left(8).data();

    srcArr = srcArr.remove(0, 8);
    return res;
}


QDateTime DataSerializer::deserializeQDATETIMERAW(QByteArray & srcArr)
{
    quint64 res = *(qint64 *)srcArr.left(8).data();

    srcArr = srcArr.remove(0, 8);
    QDateTime tm = QDateTime::fromMSecsSinceEpoch(res);
    return tm;
}


QList<quint64> DataSerializer::deserializeQLISTQUINT64RAW(QByteArray & srcArr)
{
    QList<quint64> data;

    QByteArray tmp;
    quint32 count = deserializeUINT32RAW(srcArr);

    for(quint32 i = 0; i < count; i++)
    {
        quint64 res = *(qint64 *)srcArr.left(8).data();
        srcArr = srcArr.remove(0, 8);
        data.append(res);
    }

    return data;
}


QList<quint32> DataSerializer::deserializeQLISTQUINT32RAW(QByteArray & srcArr)
{
    QList<quint32> data;

    QByteArray tmp;
    quint32 count = deserializeUINT32RAW(srcArr);

    for(quint32 i = 0; i < count; i++)
    {
        quint32 res = *(qint64 *)srcArr.left(4).data();
        srcArr = srcArr.remove(0, 4);
        data.append(res);
    }

    return data;
}


QVariant DataSerializer::deserializeQVARIANTRAW(QByteArray & srcArr)
{
    QVariant val;
    quint32 dType = deserializeINT32RAW(srcArr);

    QVariant::Type tp = (QVariant::Type)dType;

    switch(tp)
    {
      case QVariant::Bool:
      {
          val = deserializeUINT8RAW(srcArr);
      } break;
      case QVariant::Int:
      {
          val = deserializeINT64RAW(srcArr);
      } break;
      case QVariant::Double:
      {
          val = deserializeQREALRAW(srcArr);
      } break;
      case QVariant::UInt:
      {
          val = deserializeUINT64RAW(srcArr);
      } break;
      case QVariant::LongLong:
      {
          val = deserializeINT64RAW(srcArr);
      } break;
      case QVariant::ULongLong:
      {
          val = deserializeUINT64RAW(srcArr);
      } break;
      case QVariant::String:
      {
          val = deserializeQStringRAW(srcArr);
      } break;
      //    case QVariant::Char:
      //    {
      //        serializeRAW((quint16)(data.toChar()),srcArr);
      //    }break;
      case QVariant::DateTime:
      {
          val = deserializeQDATETIMERAW(srcArr);
      } break;
      default:
      {
          qWarning() << "DataSerializer::serializeRAW(QVariant): ERROR! unsupported datatype";
      } break;
    }

    return val;
}


void DataSerializer::serializeRAW(QList<QString> data, QByteArray & srcArr)
{
    QByteArray tmp;
    quint64 cn = data.count();

    DataSerializer::serializeRAW(cn, tmp);

    foreach (QString str, data)
    {
        DataSerializer::serializeRAW(str, tmp);
    }

    srcArr = tmp;
}


void DataSerializer::serializeRAW(const QByteArray & dataArr, QByteArray &srcArr)
{
    QByteArray tmpArr, szArr;
    quint64 cn = dataArr.count();

    DataSerializer::serializeRAW(cn, szArr);

    tmpArr = szArr;
    tmpArr.append(dataArr);

    srcArr.append(tmpArr);
}


QByteArray DataSerializer::serializeJSONRAW(QVariant data)
{
    bool res = false;
    QByteArray arr = JSonSerializerPrivate::serialize(data, res);

    if(!res)
    {
        return QByteArray();
    }

    return arr;
}


QList<QString> DataSerializer::deserializeQLISTQStringRAW(QByteArray & srcArr)
{
    QList<QString> data;
    data.clear();

    quint64 bytesDeserialized = 0;
    QByteArray tmp = srcArr;
    quint64 cn = DataSerializer::deserializeUINT64RAW(tmp);

    for(quint64 i = 0; i < cn; i++)
    {
        QString res = DataSerializer::deserializeQStringRAW(tmp);
        data.append(res);
        bytesDeserialized += 8 + res.size();
    }

    srcArr = srcArr.remove(0, bytesDeserialized);

    return data;
}


QByteArray DataSerializer::deserializeQByteArrayRAW(QByteArray & srcArr)
{
    QByteArray arr;
    quint64 cn = DataSerializer::deserializeUINT64RAW(srcArr);

    arr = srcArr.mid(0, cn);
    srcArr.remove(0, cn);

    return arr;
}


quint16 DataSerializer::swapBytes(quint16 src)
{
    quint16 res = src;

    res = (res << 8) | (0x00FF & (src >> 8));
    return res;
}


QVariant DataSerializer::deserializeJSONRAW(const QByteArray &srcArr)
{
    bool res = true;
    QString str = QString(srcArr);
    QVariant var = JSonSerializerPrivate::parse(str, res);

    if(res)
    {
        return var;
    }

    return QVariant();
}


/**
 * QtJson - A simple class for parsing JSON data into a QVariant hierarchies and vice-versa.
 */


template<typename T>


QByteArray JSonSerializerPrivate::serializeMap(const T &map, bool &success)
{
    QByteArray str = "{";

    QList<QByteArray> pairs;

    for(typename T::const_iterator it = map.begin(), itend = map.end(); it != itend; ++it)
    {
        QVariant var = it.value();
        QByteArray serializedValue = serialize(it.value());

        if(serializedValue.isNull())
        {
            success = false;
            break;
        }

        pairs << sanitizeString(it.key()).toUtf8() + ":" + serializedValue;
    }

    str += join(pairs, ",");
    str += "}";
    return str;
}


template<typename T>


void JSonSerializerPrivate::cloneMap(QVariant &json, const T &map)
{
    for(typename T::const_iterator it = map.begin(), itend = map.end(); it != itend; ++it)
    {
        insert(json, it.key(), (*it));
    }
}


template<typename T>


void JSonSerializerPrivate::cloneList(QVariant &json, const T &list)
{
    for(typename T::const_iterator it = list.begin(), itend = list.end(); it != itend; ++it)
    {
        append(json, (*it));
    }
}


/**
 * parse
 */

QVariant JSonSerializerPrivate::parse(const QString &json)
{
    bool success = true;

    return parse(json, success);
}


/**
 * parse
 */

QVariant JSonSerializerPrivate::parse(const QString &json, bool &success)
{
    success = true;

    // Return an empty QVariant if the JSON data is either null or empty
    if(!json.isNull() || !json.isEmpty())
    {
        QString data = json;
        // We'll start from index 0
        int index = 0;

        // Parse the first value
        QVariant value = parseValue(data, index, success);

        // Return the parsed value
        return value;
    }
    else
    {
        // Return the empty QVariant
        return QVariant();
    }
}


/**
 * clone
 */

QVariant JSonSerializerPrivate::clone(const QVariant &data)
{
    QVariant v;

    if(data.type() == QVariant::Map)
    {
        cloneMap(v, data.toMap());
    }
    else if(data.type() == QVariant::Hash)
    {
        cloneMap(v, data.toHash());
    }
    else if(data.type() == QVariant::List)
    {
        cloneList(v, data.toList());
    }
    else if(data.type() == QVariant::StringList)
    {
        cloneList(v, data.toStringList());
    }
    else
    {
        v = QVariant(data);
    }

    return v;
}


/**
 * insert value (map case)
 */

void JSonSerializerPrivate::insert(QVariant &v, const QString &key, const QVariant &value)
{
    if(!v.canConvert<QVariantMap>())
        v = QVariantMap();

    QVariantMap *p = (QVariantMap *)v.data();
    p->insert(key, clone(value));
}


/**
 * append value (list case)
 */

void JSonSerializerPrivate::append(QVariant &v, const QVariant &value)
{
    if(!v.canConvert<QVariantList>())
        v = QVariantList();

    QVariantList *p = (QVariantList *)v.data();
    p->append(value);
}


QByteArray JSonSerializerPrivate::serialize(const QVariant &data)
{
    bool success = true;

    return serialize(data, success);
}


QByteArray JSonSerializerPrivate::serialize(const QVariant &data, bool &success)
{
    QByteArray str;

    success = true;

    if(!data.isValid())        // invalid or null?
    {
        str = "null";
    }
    else if((data.type() == QVariant::List) ||
            (data.type() == QVariant::StringList))          // variant is a list?
    {
        QList<QByteArray> values;
        const QVariantList list = data.toList();

        Q_FOREACH(const QVariant & v, list)
        {
            QByteArray serializedValue = serialize(v);

            if(serializedValue.isNull())
            {
                success = false;
                break;
            }

            values << serializedValue;
        }

        str = "[" + join( values, "," ) + "]";
    }
    else if(data.type() == QVariant::Hash)          // variant is a hash?
    {
        str = serializeMap<>(data.toHash(), success);
    }
    else if(data.type() == QVariant::Map)          // variant is a map?
    {
        str = serializeMap<>(data.toMap(), success);
    }
    else if((data.type() == QVariant::String) ||
            (data.type() == QVariant::ByteArray))         // a string or a byte array?
    {
        str = sanitizeString(data.toString()).toUtf8();
    }
    else if(data.type() == QVariant::Double)          // double?
    {
        double value = data.toDouble(&success);

        if(success)
        {
            str = QByteArray::number(value, 'g');

            if(!str.contains(".") && !str.contains("e"))
            {
                str += ".0";
            }
        }
    }
    else if(data.type() == QVariant::Bool)          // boolean value?
    {
        str = data.toBool() ? "true" : "false";
    }
    else if(data.type() == QVariant::ULongLong)          // large unsigned number?
    {
        str = QByteArray::number(data.value<qulonglong>());
    }
    else if(data.canConvert<qlonglong>())          // any signed number?
    {
        str = QByteArray::number(data.value<qlonglong>());
    }
    else if(data.canConvert<long>())          //TODO: this code is never executed because all smaller types can be converted to qlonglong
    {
        str = QString::number(data.value<long>()).toUtf8();
    }
    else if(data.type() == QVariant::Size)           // datetime value?
    {
        QSize r = data.toSize();
        QVariantMap map;
        map.insert("w", r.width());
        map.insert("h", r.height());
        map.insert(getQVarTag(), (int)data.type());
        str = serialize(map, success);
    }
    else if(data.type() == QVariant::SizeF)           // datetime value?
    {
        bool res = false;
        QSizeF r = data.toSizeF();
        QVariantMap map;
        map.insert("w", r.width());
        map.insert("h", r.height());
        map.insert(getQVarTag(), (int)data.type());
        str = serialize(map, success);
    }
    else if(data.type() == QVariant::Point)          // datetime value?
    {
        QPoint r = data.toPoint();
        QVariantMap map;
        map.insert("x", r.x());
        map.insert("y", r.y());
        map.insert(getQVarTag(), (int)data.type());
        str = serialize(map, success);
    }
    else if(data.type() == QVariant::PointF)          // datetime value?
    {
        QPointF r = data.toPointF();
        QVariantMap map;
        map.insert("x", r.x());
        map.insert("y", r.y());
        map.insert(getQVarTag(), (int)data.type());
        str = serialize(map, success);
    }
    else if(data.type() == QVariant::RectF)           // datetime value?
    {
        QRectF r = data.toRectF();
        QVariantMap map;
        map.insert("x", r.topLeft().x());
        map.insert("y", r.topLeft().y());
        map.insert("w", r.width());
        map.insert("h", r.height());
        map.insert(getQVarTag(), (int)data.type());
        str = serialize(map, success);
    }
    else if(data.type() == QVariant::Rect)           // datetime value?
    {
        QRect r = data.toRect();
        QVariantMap map;
        map.insert("x", r.topLeft().x());
        map.insert("y", r.topLeft().y());
        map.insert("w", r.width());
        map.insert("h", r.height());
        map.insert(getQVarTag(), (int)data.type());
        str = serialize(map, success);
    }
    else if(data.type() == QVariant::DateTime)                // datetime value?
    {
        str = sanitizeString(dateTimeFormat.isEmpty()
                             ? data.toDateTime().toString()
                             : data.toDateTime().toString(dateTimeFormat)).toUtf8();
    }
    else if(data.type() == QVariant::Date)          // date value?
    {
        str = sanitizeString(dateTimeFormat.isEmpty()
                             ? data.toDate().toString()
                             : data.toDate().toString(dateFormat)).toUtf8();
    }
    else if(data.canConvert<QString>())          // can value be converted to string?
    {       // this will catch QUrl, ... (all other types which can be converted to string)
        str = sanitizeString(data.toString()).toUtf8();
    }
    else
    {
        success = false;
    }

    if(success)
    {
        return str;
    }

    return QByteArray();
}


QString JSonSerializerPrivate::serializeStr(const QVariant &data)
{
    return QString::fromUtf8(serialize(data));
}


QString JSonSerializerPrivate::serializeStr(const QVariant &data, bool &success)
{
    return QString::fromUtf8(serialize(data, success));
}


/**
 * \enum JsonToken
 */
enum JsonToken
{
    JsonTokenNone = 0,
    JsonTokenCurlyOpen = 1,
    JsonTokenCurlyClose = 2,
    JsonTokenSquaredOpen = 3,
    JsonTokenSquaredClose = 4,
    JsonTokenColon = 5,
    JsonTokenComma = 6,
    JsonTokenString = 7,
    JsonTokenNumber = 8,
    JsonTokenTrue = 9,
    JsonTokenFalse = 10,
    JsonTokenNull = 11
};


QString JSonSerializerPrivate::getQVarTag()
{
    return QString("qvarianttype");
}


QString JSonSerializerPrivate::sanitizeString(QString str)
{
    str.replace(QLatin1String("\\"), QLatin1String("\\\\"));
    str.replace(QLatin1String("\""), QLatin1String("\\\""));
    str.replace(QLatin1String("\b"), QLatin1String("\\b"));
    str.replace(QLatin1String("\f"), QLatin1String("\\f"));
    str.replace(QLatin1String("\n"), QLatin1String("\\n"));
    str.replace(QLatin1String("\r"), QLatin1String("\\r"));
    str.replace(QLatin1String("\t"), QLatin1String("\\t"));
    return QString(QLatin1String("\"%1\"")).arg(str);
}


QByteArray JSonSerializerPrivate::join(const QList<QByteArray> &list, const QByteArray &sep)
{
    QByteArray res;

    Q_FOREACH(const QByteArray &i, list)
    {
        if(!res.isEmpty())
        {
            res += sep;
        }

        res += i;
    }

    return res;
}


/**
 * parseValue
 */

QVariant JSonSerializerPrivate::parseValue(const QString &json, int &index, bool &success)
{
    // Determine what kind of data we should parse by
    // checking out the upcoming token
    switch(lookAhead(json, index))
    {
      case JsonTokenString:
      {
          return parseString(json, index, success);
      } break;
      case JsonTokenNumber:
      {
          return parseNumber(json, index);
      } break;
      case JsonTokenCurlyOpen:
      {
          QVariant var = parseObject(json, index, success);
          if(var.type() == QVariant::Map)
          {
              QVariantMap mp = var.toMap();

              if(mp.contains(getQVarTag()))
              {
                  switch(mp.value(getQVarTag()).toInt())
                  {
                    case QVariant::PointF:
                    {
                        QPointF r = QPointF(mp.value("x").toDouble(), mp.value("y").toDouble());
                        var = QVariant(r);
                    } break;
                    case QVariant::Point:
                    {
                        QPoint r = QPoint(mp.value("x").toInt(), mp.value("y").toInt());
                        var = QVariant(r);
                    } break;
                    case QVariant::RectF:
                    {
                        QRectF r = QRectF(mp.value("x").toDouble(), mp.value("y").toDouble(), mp.value("w").toDouble(), mp.value("h").toDouble());
                        var = QVariant(r);
                    } break;
                    case QVariant::Rect:
                    {
                        QRect r = QRect(mp.value("x").toInt(), mp.value("y").toInt(), mp.value("w").toInt(), mp.value("h").toInt());
                        var = QVariant(r);
                    } break;
                    case QVariant::SizeF:
                    {
                        QSizeF sz = QSizeF(mp.value("w").toDouble(), mp.value("h").toDouble());
                        var = QVariant(sz);
                    } break;
                    case QVariant::Size:
                    {
                        QSize sz = QSize(mp.value("w").toInt(), mp.value("h").toInt());
                        var = QVariant(sz);
                    } break;
                    default:
                    {
                        qDebug() << "JSonSerializerPrivate::parseValue: Error! Unsupported value type " << mp.value(getQVarTag()).toInt();
                    } break;
                  }
              }
          }

          return var;
      } break;
      case JsonTokenSquaredOpen:
      {
          QVariant var = parseArray(json, index, success);



          return var;
      } break;
      case JsonTokenTrue:
          nextToken(json, index);
          return QVariant(true);
          break;
      case JsonTokenFalse:
          nextToken(json, index);
          return QVariant(false);
          break;
      case JsonTokenNull:
          nextToken(json, index);
          return QVariant();
          break;
      case JsonTokenNone:
          break;
    }

    // If there were no tokens, flag the failure and return an empty QVariant
    success = false;
    return QVariant();
}


/**
 * parseObject
 */

QVariant JSonSerializerPrivate::parseObject(const QString &json, int &index, bool &success)
{
    QVariantMap map;
    int token;

    // Get rid of the whitespace and increment index
    nextToken(json, index);

    // Loop through all of the key/value pairs of the object
    bool done = false;

    while(!done)
    {
        // Get the upcoming token
        token = lookAhead(json, index);

        if(token == JsonTokenNone)
        {
            success = false;
            return QVariantMap();
        }
        else if(token == JsonTokenComma)
        {
            nextToken(json, index);
        }
        else if(token == JsonTokenCurlyClose)
        {
            nextToken(json, index);
            return map;
        }
        else
        {
            // Parse the key/value pair's name
            QString name = parseString(json, index, success).toString();

            if(!success)
            {
                return QVariantMap();
            }

            // Get the next token
            token = nextToken(json, index);

            // If the next token is not a colon, flag the failure
            // return an empty QVariant
            if(token != JsonTokenColon)
            {
                success = false;
                return QVariant(QVariantMap());
            }

            // Parse the key/value pair's value
            QVariant value = parseValue(json, index, success);

            if(!success)
            {
                return QVariantMap();
            }

            // Assign the value to the key in the map
            map[name] = value;
        }
    }

    // Return the map successfully
    return QVariant(map);
}


/**
 * parseArray
 */

QVariant JSonSerializerPrivate::parseArray(const QString &json, int &index, bool &success)
{
    QVariantList list;

    nextToken(json, index);

    bool done = false;

    while(!done)
    {
        int token = lookAhead(json, index);

        if(token == JsonTokenNone)
        {
            success = false;
            return QVariantList();
        }
        else if(token == JsonTokenComma)
        {
            nextToken(json, index);
        }
        else if(token == JsonTokenSquaredClose)
        {
            nextToken(json, index);
            break;
        }
        else
        {
            QVariant value = parseValue(json, index, success);

            if(!success)
            {
                return QVariantList();
            }

            list.push_back(value);
        }
    }

    return QVariant(list);
}


/**
 * parseString
 */

QVariant JSonSerializerPrivate::parseString(const QString &json, int &index, bool &success)
{
    QString s;
    QChar c;

    eatWhitespace(json, index);

    c = json[index++];

    bool complete = false;

    while(!complete)
    {
        if(index == json.size())
        {
            break;
        }

        c = json[index++];

        if(c == '\"')
        {
            complete = true;
            break;
        }
        else if(c == '\\')
        {
            if(index == json.size())
            {
                break;
            }

            c = json[index++];

            if(c == '\"')
            {
                s.append('\"');
            }
            else if(c == '\\')
            {
                s.append('\\');
            }
            else if(c == '/')
            {
                s.append('/');
            }
            else if(c == 'b')
            {
                s.append('\b');
            }
            else if(c == 'f')
            {
                s.append('\f');
            }
            else if(c == 'n')
            {
                s.append('\n');
            }
            else if(c == 'r')
            {
                s.append('\r');
            }
            else if(c == 't')
            {
                s.append('\t');
            }
            else if(c == 'u')
            {
                int remainingLength = json.size() - index;

                if(remainingLength >= 4)
                {
                    QString unicodeStr = json.mid(index, 4);
                    int symbol = unicodeStr.toInt(0, 16);

                    s.append(QChar(symbol));

                    index += 4;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            s.append(c);
        }
    }

    if(!complete)
    {
        success = false;
        return QVariant();
    }

    return QVariant(s);
}


/**
 * parseNumber
 */

QVariant JSonSerializerPrivate::parseNumber(const QString &json, int &index)
{
    eatWhitespace(json, index);

    int lastIndex = lastIndexOfNumber(json, index);
    int charLength = (lastIndex - index) + 1;
    QString numberStr;

    numberStr = json.mid(index, charLength);

    index = lastIndex + 1;
    bool ok;

    if(numberStr.contains('.'))
    {
        return QVariant(numberStr.toDouble(NULL));
    }
    else if(numberStr.startsWith('-'))
    {
        int i = numberStr.toInt(&ok);

        if(!ok)
        {
            qlonglong ll = numberStr.toLongLong(&ok);
            return ok ? ll : QVariant(numberStr);
        }

        return i;
    }
    else
    {
        uint u = numberStr.toUInt(&ok);

        if(!ok)
        {
            qulonglong ull = numberStr.toULongLong(&ok);
            return ok ? ull : QVariant(numberStr);
        }

        return u;
    }
}


/**
 * lastIndexOfNumber
 */

int JSonSerializerPrivate::lastIndexOfNumber(const QString &json, int index)
{
    int lastIndex;

    for(lastIndex = index; lastIndex < json.size(); lastIndex++)
    {
        if(QString("0123456789+-.eE").indexOf(json[lastIndex]) == -1)
        {
            break;
        }
    }

    return lastIndex - 1;
}


/**
 * eatWhitespace
 */

void JSonSerializerPrivate::eatWhitespace(const QString &json, int &index)
{
    for(; index < json.size(); index++)
    {
        if(QString(" \t\n\r").indexOf(json[index]) == -1)
        {
            break;
        }
    }
}


/**
 * lookAhead
 */

int JSonSerializerPrivate::lookAhead(const QString &json, int index)
{
    int saveIndex = index;

    return nextToken(json, saveIndex);
}


/**
 * nextToken
 */

int JSonSerializerPrivate::nextToken(const QString &json, int &index)
{
    eatWhitespace(json, index);

    if(index == json.size())
    {
        return JsonTokenNone;
    }

    QChar c = json[index];
    index++;

    switch(c.toLatin1())
    {
      case '{': return JsonTokenCurlyOpen;
      case '}': return JsonTokenCurlyClose;
      case '[': return JsonTokenSquaredOpen;
      case ']': return JsonTokenSquaredClose;
      case ',': return JsonTokenComma;
      case '"': return JsonTokenString;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case '-': return JsonTokenNumber;
      case ':': return JsonTokenColon;
    }

    index--;     // ^ WTF?

    int remainingLength = json.size() - index;

    // True
    if(remainingLength >= 4)
    {
        if(json[index] == 't' && json[index + 1] == 'r' &&
           json[index + 2] == 'u' && json[index + 3] == 'e')
        {
            index += 4;
            return JsonTokenTrue;
        }
    }

    // False
    if(remainingLength >= 5)
    {
        if(json[index] == 'f' && json[index + 1] == 'a' &&
           json[index + 2] == 'l' && json[index + 3] == 's' &&
           json[index + 4] == 'e')
        {
            index += 5;
            return JsonTokenFalse;
        }
    }

    // Null
    if(remainingLength >= 4)
    {
        if(json[index] == 'n' && json[index + 1] == 'u' &&
           json[index + 2] == 'l' && json[index + 3] == 'l')
        {
            index += 4;
            return JsonTokenNull;
        }
    }

    return JsonTokenNone;
}


void JSonSerializerPrivate::setDateTimeFormat(const QString &format)
{
    dateTimeFormat = format;
}


void JSonSerializerPrivate::setDateFormat(const QString &format)
{
    dateFormat = format;
}


QString JSonSerializerPrivate::getDateTimeFormat()
{
    return dateTimeFormat;
}


QString JSonSerializerPrivate::getDateFormat()
{
    return dateFormat;
}


const Object &Object::operator[](const QString &key) const
{
    return const_cast<Object *>(this)->operator[](key);
}


Object &Object::operator=(const QVariant &rhs)
{
    /** It maybe more robust when running under Qt versions below 4.7 */
    QObject *obj = qvariant_cast<QObject *>(rhs);

    //  setValue(rhs);
    setValue(obj);
    return *this;
}


Object &Object::operator[](const QString &key)
{
    if(type() == QVariant::Map)
        return insertKey<QVariantMap>(this, key);
    else if(type() == QVariant::Hash)
        return insertKey<QVariantHash>(this, key);

    setValue(QVariantMap());

    return insertKey<QVariantMap>(this, key);
}


void Object::remove(const QString &key)
{
    if(type() == QVariant::Map)
        removeKey<QVariantMap>(this, key);
    else if(type() == QVariant::Hash)
        removeKey<QVariantHash>(this, key);
}

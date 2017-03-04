#include "embeddedutils.h"
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QLayout>
#include <QSize>
#include <QWidget>
#include "embeddedwidget.h"
#include "embeddedwindow.h"
#include "embeddedmainwindow.h"
#include "embeddedapp.h"

using namespace ew;
EmbeddedUtils::EmbeddedUtils()
{
}



QHash<QString, QString> EmbeddedUtils::fillTrHash()
{
    QHash<QString, QString>  trHash;
    trHash.insert("а", "a");    trHash.insert("б", "b");
    trHash.insert("в", "v");    trHash.insert("г", "g");
    trHash.insert("д", "d");    trHash.insert("е", "ye");
    trHash.insert("ё", "yo");    trHash.insert("ж", "zh");
    trHash.insert("з", "z");    trHash.insert("и", "i");
    trHash.insert("й", "y");    trHash.insert("к", "k");
    trHash.insert("л", "l");    trHash.insert("м", "m");
    trHash.insert("н", "n");    trHash.insert("о", "o");
    trHash.insert("п", "p");    trHash.insert("р", "r");
    trHash.insert("с", "s");    trHash.insert("т", "t");
    trHash.insert("у", "u");    trHash.insert("ф", "f");
    trHash.insert("х", "kh");    trHash.insert("ц", "c");
    trHash.insert("ч", "ch");    trHash.insert("ш", "sh");
    trHash.insert("щ", "sch");    trHash.insert("ъ", "");
    trHash.insert("ы", "y");    trHash.insert("ь", "");
    trHash.insert("э", "e");    trHash.insert("ю", "yu");
    trHash.insert("я", "ya");
    trHash.insert("А", "A");    trHash.insert("Б", "B");
    trHash.insert("В", "V");    trHash.insert("Г", "G");
    trHash.insert("Д", "D");    trHash.insert("Е", "YE");
    trHash.insert("Ё", "YO");    trHash.insert("Ж", "ZH");
    trHash.insert("З", "Z");    trHash.insert("И", "I");
    trHash.insert("Й", "Y");    trHash.insert("К", "K");
    trHash.insert("Л", "L");    trHash.insert("М", "M");
    trHash.insert("Н", "N");    trHash.insert("О", "O");
    trHash.insert("П", "P");    trHash.insert("Р", "R");
    trHash.insert("С", "S");    trHash.insert("Т", "T");
    trHash.insert("У", "U");    trHash.insert("Ф", "F");
    trHash.insert("Х", "KH");    trHash.insert("Ц", "C");
    trHash.insert("Ч", "CH");    trHash.insert("Ш", "SH");
    trHash.insert("Щ", "SCH");
    trHash.insert("Ы", "Y");
    trHash.insert("Э", "E");    trHash.insert("Ю", "YU");
    trHash.insert("Я", "YA");
    return trHash;
}


QString EmbeddedUtils::toTranslite(const QString & str)
{
    //QString str = "строка на русском"; //если убрать этот коммент, то функция нормально переведет эту строку
    QString result = "";
    static QHash<QString, QString> trHash;

    if(trHash.isEmpty())
    {
        trHash = fillTrHash();
    }

    for(int i = 0; i < str.size(); i++)
    {
        qDebug() << str.at(i);

        if(trHash.contains(str.at(i)) == true)
        {
            result.append(trHash.value(str.at(i)));
        }
        else
        {
            result.append(str.at(i));
        }
    }

    return result;
}


QSize EmbeddedUtils::getWidgetSizeHint(QWidget *wdg)
{
    QWidget *parentPtr = wdg;

    while(0 != parentPtr->parentWidget())
    {
        parentPtr = parentPtr->parentWidget();
    }

    bool needHide = false;

    if(!parentPtr->isVisible())
    {
        needHide = true;
        parentPtr->setAttribute(Qt::WA_DontShowOnScreen, true);
        parentPtr->setVisible(true);
    }

    QSize sz = wdg->size();
    QSize szHints = wdg->sizeHint();
    Q_UNUSED(szHints);

    if(needHide)
    {
        parentPtr->hide();
        parentPtr->setAttribute(Qt::WA_DontShowOnScreen, false);
    }

    return sz;
}


//QString EmbeddedUtils::fromTranslite(const QString& str)
//{
//    //QString str = "строка на русском"; //если убрать этот коммент, то функция нормально переведет эту строку
//    QString result = "";
//    static QHash<QChar,QString> trHash;

//    if(trHash.isEmpty())
//    {
//        trHash = fillTrHash();
//    }

//    for(int i=0;i<str.size();i++)
//    {
//        if(trHash.contains(str.at(i)) == true)
//        {
//            result.append(trHash.value(str.at(i)));
//        }
//        else
//        {
//            result.append(str.at(i));
//        }
//    }

//    return result;


//}



QString EmbeddedUtils::hashString(const QString & str)
{
    QByteArray hash = QCryptographicHash::hash(
        QByteArray::fromRawData((const char *)str.utf16(), str.length() * 2),
        QCryptographicHash::Md5
        );

    return QString(hash);
}


qint64 EmbeddedUtils::hashInt64(const QString & str)
{
    QByteArray hash = QCryptographicHash::hash(
        QByteArray::fromRawData((const char *)str.utf16(), str.length() * 2),
        QCryptographicHash::Md5
        );

    Q_ASSERT(hash.size() == 16);
    QBuffer buf(&hash);
    QDataStream stream(&buf);
    qint64 a, b;
    stream >> a >> b;
    return a ^ b;
}


bool EmbeddedUtils::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if(dir.exists())
    {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if(info.isDir())
            {
                result = removeDir(info.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(info.absoluteFilePath());
            }

            if(!result)
            {
                return result;
            }
        }

        result = QDir().rmdir(dirName);
    }

    return result;
}


QPointF EmbeddedUtils::convertFromAlignPoint(QPointF alignPoint, ew::EmbeddedWidgetAlign alignType
                                             , QSize widgetSize, QSize parentViewSize, QPointF parentPos)
{
    QPointF widgetPoint;


    switch(alignType)
    {
      case EWA_IN_LEFT_TOP_CORNER_OFFSET:   /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
      {
          widgetPoint = alignPoint;
      } break;
      case EWA_IN_LEFT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
      {
          widgetPoint = QPointF(alignPoint.x()
                                , parentViewSize.height() - widgetSize.height() - alignPoint.y() );
      } break;
      case EWA_IN_RIGHT_TOP_CORNER_OFFSET:  /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
      {
          widgetPoint = QPointF(parentViewSize.width() - alignPoint.x() - widgetSize.width()
                                , alignPoint.y() );
      } break;
      case EWA_IN_RIGHT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
      {
          widgetPoint = QPointF(parentViewSize.width() - alignPoint.x() - widgetSize.width()
                                , parentViewSize.height() - alignPoint.y() - widgetSize.height() );
      } break;
      case EWA_IN_PERCENTAGE_ALIGN:          /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
      {
          widgetPoint = QPointF((alignPoint.x() * parentViewSize.width()) / 100.0 - widgetSize.width() / 2.0
                                , (alignPoint.y() * parentViewSize.height()) / 100.0 - widgetSize.height() / 2.0 );
      } break;
      case EWA_OUT_FREE_WIDGET:
      {
          widgetPoint = alignPoint;
      } break;
      case EWA_OUT_LEFT_TOP_CORNER_OFFSET:
      {
          widgetPoint = QPointF(parentPos.x() - alignPoint.x() - widgetSize.width()
                                , parentPos.y() - alignPoint.y() - widgetSize.height());
      } break;
      case EWA_OUT_LEFT_BOTTOM_CORNER_OFFSET:
      {
          widgetPoint = QPointF(parentPos.x() - alignPoint.x() - widgetSize.width()
                                , parentPos.y() + parentViewSize.height() + alignPoint.y());
      } break;
      case EWA_OUT_RIGHT_BOTTOM_CORNER_OFFSET:
      {
          widgetPoint = QPointF(parentPos.x() + parentViewSize.width() + alignPoint.x()
                                , parentPos.y() + parentViewSize.height() + alignPoint.y());
      } break;
      case EWA_OUT_RIGHT_TOP_CORNER_OFFSET:          /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
      {
          widgetPoint = QPointF(parentPos.x() + parentViewSize.width() + alignPoint.x()
                                , parentPos.y() - alignPoint.y() - widgetSize.height());
      } break;
      default:
      {
          qWarning() << "EmbeddedUtils::convertFromAlignPoint(...): WARN! Unsupported politics";
      } break;
    }

    return widgetPoint;
}


bool EmbeddedUtils::correctWidgetGeometry(QRect parentRect, QRect & widgetRect, QSize minSize, QSize maxSize, bool saveSize)
{
    Q_UNUSED(saveSize);
    int parentWidth = parentRect.size().width();
    int parentHeight = parentRect.size().height();
    int widgetWidth = widgetRect.size().width();
    int widgetHeight = widgetRect.size().height();
    QRect correctedRect = widgetRect;

    if(parentRect.contains(widgetRect) && widgetWidth >= minSize.width()
       && widgetWidth < maxSize.width() &&  widgetHeight >= minSize.height()
       && widgetHeight < maxSize.height())
    {
        return true;
    }

    // виджет вылазит за пределеы родителя
    // сдвигаем виджет, если недостаточно - меняем размеры, если недостаточно

    if(widgetRect.topLeft().x() < 0)
    {
        correctedRect.moveLeft(0);
    }

    if(widgetRect.topLeft().y() < 0)
    {
        correctedRect.setTop(0);
    }

    if(parentRect.contains(correctedRect) && correctedRect.size().width() >= minSize.width()
       && correctedRect.size().width() < maxSize.width() &&  correctedRect.size().height() >= minSize.height()
       && correctedRect.size().height() < maxSize.height())
    {
        return true;
    }

    // корректируем позицию, если виджет вылазит справа или вниз
    if(parentWidth < correctedRect.right())
    {
        if(correctedRect.width() < parentWidth)
        {
            correctedRect.moveLeft(parentWidth - correctedRect.width());
        }
        else
        {
            if(minSize.width() < parentWidth)
            {
                correctedRect.moveLeft(0);
                correctedRect.setWidth(parentWidth - 1 );
            }
            else
            {
                // меняем размер, игнорируя minSize
                correctedRect.moveLeft(0);
                correctedRect.setWidth(parentWidth - 1);
            }
        }
    }

    //    else
    //    {
    //        correctedRect.moveLeft(parentWidth - correctedRect.width());
    //    }



    if(parentHeight < correctedRect.bottom())
    {
        if(correctedRect.height() < parentHeight)
        {
            correctedRect.moveTop(parentHeight - correctedRect.height());
        }
        else
        {
            if(minSize.height() < parentHeight)
            {
                correctedRect.moveTop(0);
                correctedRect.setHeight(parentHeight - 1 );
            }
            else
            {
                // меняем размер, игнорируя minSize
                correctedRect.moveTop(0);
                correctedRect.setHeight(parentHeight - 1);
            }
        }
    }/*
        else
        {
        correctedRect.moveTop(parentHeight - correctedRect.height());
        }*/

    widgetRect = correctedRect;


    return true;
}


QPointF EmbeddedUtils::convertToAlignPoint(QPointF posOnParent, ew::EmbeddedWidgetAlign alignType
                                           , QSize widgetSize, QSize parentViewSize, QPointF parentPos )
{
    QPointF alignPoint;


    switch(alignType)
    {
      case EWA_IN_LEFT_TOP_CORNER_OFFSET:
      {
          alignPoint = posOnParent;
      } break;

      case EWA_IN_LEFT_BOTTOM_CORNER_OFFSET:
      {
          alignPoint = QPointF( posOnParent.x(), (parentViewSize.height() - (posOnParent.y() + widgetSize.height())  ) );
      } break;

      case EWA_IN_RIGHT_TOP_CORNER_OFFSET:
      {
          alignPoint = QPointF( (parentViewSize.width() - (posOnParent.x() + widgetSize.width())), posOnParent.y()  );
      } break;

      case EWA_IN_RIGHT_BOTTOM_CORNER_OFFSET:
      {
          alignPoint = QPointF( (parentViewSize.width() - (posOnParent.x() + widgetSize.width()))
                                , (parentViewSize.height() - (posOnParent.y() + widgetSize.height())) );
      } break;

      case EWA_IN_PERCENTAGE_ALIGN:
      {
          QPointF percPoint(100.0 * ((posOnParent.x() * 1.0 + widgetSize.width() / 2.0) / (1.0 * parentViewSize.width()))
                            , 100.0 * ((posOnParent.y() * 1.0 + widgetSize.height() / 2.0) / (1.0 * parentViewSize.height())) );
          alignPoint = percPoint;
      } break;
      case EWA_OUT_FREE_WIDGET:
      {
          alignPoint = posOnParent;
      } break;
      case EWA_OUT_LEFT_TOP_CORNER_OFFSET:
      {
          alignPoint = QPointF( parentPos.x() - (posOnParent.x() + widgetSize.width())
                                , parentPos.y() - (posOnParent.y() + widgetSize.height())  );
      } break;
      case EWA_OUT_LEFT_BOTTOM_CORNER_OFFSET:
      {
          alignPoint = QPointF( parentPos.x() - (posOnParent.x() + widgetSize.width())
                                , (posOnParent.y() + widgetSize.height()) - parentPos.y());
      } break;
      case EWA_OUT_RIGHT_TOP_CORNER_OFFSET:
      {
          alignPoint = QPointF( (posOnParent.x() + widgetSize.width()) - parentPos.x()
                                , parentPos.y() - (posOnParent.y() + widgetSize.height()));
      } break;
      case EWA_OUT_RIGHT_BOTTOM_CORNER_OFFSET:
      {
          alignPoint = QPointF( (posOnParent.x() + widgetSize.width()) - parentPos.x()
                                , (posOnParent.y() + widgetSize.height()) - parentPos.y() );
      } break;

      default:
          qWarning() << "EmbeddedUtils::convertToAlignPoint(): unsupported politics!";
          break;
    }


    return alignPoint;
}


QSize EmbeddedUtils::getEmbeddedSize(const QSize & viewSize, const QMargins & parentMargins)
{
    return QSize(viewSize.width() + parentMargins.left() + parentMargins.right()
                 , viewSize.height() + parentMargins.top() + parentMargins.bottom());
}


QSize EmbeddedUtils::getCorrectSize(const QSize &settedSize, const QSize &minSize, const QSize &maxSize)
{
    QSize sz = settedSize;

    if( sz.width() < minSize.width() )
    {
        sz.setWidth(minSize.width());
    }

    if( sz.height() < minSize.height())
    {
        sz.setHeight(minSize.height());
    }

    if( sz.width() > maxSize.width() )
    {
        sz.setWidth(maxSize.width());
    }

    if( sz.height() > maxSize.height())
    {
        sz.setHeight(maxSize.height());
    }

    return sz;
}

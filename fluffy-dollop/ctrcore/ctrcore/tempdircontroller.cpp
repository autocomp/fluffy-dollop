#include "tempdircontroller.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>

bool TempDirController::checkTempDirForAllUsers()
{
    QVariant tempDirVar = CtrConfig::getValueByName(QString("application_settings.tempDirForAllUsers_Path"));
    if(tempDirVar.isValid())
    {
        QString tempDir;
        tempDir = tempDirVar.toString();
        QDir dir(tempDir);
        if(dir.exists())
        {
            QString dirName = QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss_zzz");
            bool res = dir.mkdir(dirName);
            if(res)
            {
                bool remove = dir.rmdir(dirName);

                bool createUserDir(false);
                QString userName(getenv("USER"));
                QString userSubDirPath = tempDir + QDir::separator() + userName;
                QDir userSubDir(userSubDirPath);
                if(userSubDir.exists() == false)
                    createUserDir = dir.mkdir(userName);

                return true;
            }
            else
            {
                QMessageBox::critical(0,
                                      QString::fromUtf8("Внимание"),
                                      QString::fromUtf8("Нет прав для записи в каталог для сохранения временных данных от всех пользователей !"),
                                      QMessageBox::Close);
                return false;
            }
        }
        else
        {
            QMessageBox::critical(0,
                                  QString::fromUtf8("Внимание"),
                                  QString::fromUtf8("В конфигурационном файле \"application_settings\" в настройке \"tempDirForAllUsers_Path\" полный путь к каталогу для сохранения временных данных от всех пользователей не задан или каталог не существует !"),
                                  QMessageBox::Close);
            return false;
        }
    }
    else
    {
        QMessageBox::critical(0,
                              QString::fromUtf8("Внимание"),
                              QString::fromUtf8("В конфигурационном файле \"application_settings\" в настройке \"tempDirForAllUsers_Path\" не задан полный путь к каталогу для сохранения временных данных от всех пользователей !"),
                              QMessageBox::Close);
        return false;
    }
}

QString TempDirController::getTempDirForAllUsers()
{
    QVariant tempDirVar = CtrConfig::getValueByName(QString("application_settings.tempDirForAllUsers_Path"));
    if(tempDirVar.isValid())
        return tempDirVar.toString();
    else
        return QString();
}

QString TempDirController::getCurrentUserTempDir()
{
    QVariant tempDirVar = CtrConfig::getValueByName(QString("application_settings.tempDirForAllUsers_Path"));
    if(tempDirVar.isValid())
    {
        QString userName(getenv("USER"));
        QString path = tempDirVar.toString() + QDir::separator() + userName;
        return path;
    }
    else
        return QString();
}

QString TempDirController::createTempDirForCurrentUser(QString appendStrToDirName)
{
    QVariant tempDirVar = CtrConfig::getValueByName(QString("application_settings.tempDirForAllUsers_Path"));
    if(tempDirVar.isValid())
    {
        QString userName(getenv("USER"));
        QString userTempDir = tempDirVar.toString() + QDir::separator() + userName;
        QDir dir(userTempDir);
        QString tmpSubDirName = QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss_zzz");
        if(appendStrToDirName.isEmpty() == false)
            tmpSubDirName.append(appendStrToDirName);
        bool res = dir.mkdir(tmpSubDirName);
        QString path = userTempDir + QDir::separator() + tmpSubDirName;
        return path;
    }
    else
        return QString();
}

void TempDirController::clearCurrentUserTempDir()
{
    QVariant tempDirVar = CtrConfig::getValueByName(QString("application_settings.tempDirForAllUsers_Path"));
    if(tempDirVar.isValid())
    {
        QString userName(getenv("USER"));
        QString path = tempDirVar.toString() + QDir::separator() + userName;
        clearDir(path);
    }
}

void TempDirController::clearDir(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList list = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QString name, list)
    {
        QString path = dirPath + QDir::separator() + name;
        QFileInfo fi(path);
        if(fi.isFile())
        {
            dir.remove(name);
        }
        else
        {
            clearDir(path);
            dir.rmdir(name);
        }
    }
}



























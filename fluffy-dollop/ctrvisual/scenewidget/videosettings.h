#ifndef VIDEOSETTINGS_H
#define VIDEOSETTINGS_H

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDialog>
 #include "videoplayertypes.h"


namespace sw {

class VideoSettings : public QDialog
{
    Q_OBJECT

public:

    explicit VideoSettings(VideoPlayerTypes::SettingsType sType, QWidget *parent = 0);
    void setSettings(VideoPlayerTypes::VideoSaveSettings sett);

private:
    QLabel * m_pFormatLbl, *m_pDestLbl, *m_pPrefixLbl;
    QCheckBox *m_pDenyMinimizeCb, *m_pDenyCloseCb, *m_pVisibleControlsCheckBox;
    QLineEdit * m_pPrefixLineEdit, *m_pFilepathLineEdit;
    QPushButton *m_pFilepathPb, *m_pOkPb, *m_pCancelPb;
    QComboBox *m_pFormatComboBox;

    VideoPlayerTypes::VideoSaveSettings m_settings;



signals:
    void  signalSavedSettings(VideoPlayerTypes::VideoSaveSettings settings);
public slots:

private slots:
    void slotOkClicked();
    void slotCancelClicked();
    void slotFilepathButtonClicked();

    void slotPrefixChanged(QString);
    void slotFilepathChanged(QString);
    void slotFormatChanged(QString);
    void slotDenyMinimizeClicked(bool);
    void slotDenyCloseClicked(bool);
    void slotRecordControlsClicked(bool);
};

}

#endif // VIDEOSETTINGS_H

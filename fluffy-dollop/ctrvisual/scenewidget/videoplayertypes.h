#ifndef VIDEOPLAYERTYPES_H
#define VIDEOPLAYERTYPES_H



namespace VideoPlayerTypes
{

enum PlayerType
{
    PT_FILE,
    PT_URL,
    PT_DEVICE
};


enum VideoPlayerPlayModes
{
    VPPM_CONTINIOUS ,         // непрерывный
    VPPM_FLIPPING   // покадровый
};

enum VideoPlayerCommandTypes
{
    VPCT_OPEN_FILE = 0, /** команда без параметров */
    VPCT_OPEN_URL,      /** команда без параметров */
    VPCT_OPEN_DEV,      /** команда без параметров */
    VPCT_PLAY_FORWARD_CLICKED,  /** команда без параметров */
    VPCT_PLAY_BACKWARD_CLICKED, /** команда без параметров */
    VPCT_PAUSE_CLICKED, /** команда без параметров */
    VPCT_SAVE_FRAGMENT_CLICKED,  /** команда без параметров */
    VPCT_MODE_CHANGED,  /** параметр - VideoPlayerPlayModes */
    VPPT_CURRENT_SPEED_CHANGED, /** параметр - qreal - текущая скорость */
    VPCT_TIME_CHANGED ,          /** параметр - qdatetime - текущее время от начала */

    VPCT_FRAME_NUM_CHANGED, /** параметр - qulonglong - текущий фрейм*/
    VPCT_NEXT_FRAME_CLICKED,
    VPCT_PREV_FRAME_CLICKED,

    VPCT_CAPTURE_PICTURE,
    VPCT_CAPTURE_VIDEO_OFF,
    VPCT_CAPTURE_VIDEO_ON
};

enum SettingsType
{
    ST_VIDEO,
    ST_PICTURE
};

struct VideoSaveSettings
{
    VideoPlayerTypes::SettingsType sType;
    QString fileFormat;
    QString fileDestination;
    QString filePrefix;
    bool denyMinimizeWindow;
    bool denyCloseWindow;

    bool recordControls;
};




}


#endif // VIDEOPLAYERTYPES_H

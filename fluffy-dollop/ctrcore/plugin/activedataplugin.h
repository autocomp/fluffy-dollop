#ifndef ACTIVEDATAPLUGIN_H
#define ACTIVEDATAPLUGIN_H

#include "ctrappvisualizerplugin.h"
#include <ctrcore/plugin/embifacenotifier.h>

class QCheckBox;
class QFrame;
class QToolButton;
class QLabel;

class ActiveDataPlugin : public CtrAppVisualizerPlugin
{
    Q_OBJECT

public:
    ActiveDataPlugin();
    ~ActiveDataPlugin();

    // Методы переопределённые из базовых классов :
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

    virtual QIcon getIconOn() = 0;
    virtual QIcon getIconOff() = 0;
    virtual QString getIconOnPath() = 0;
    virtual QString getToolTip() = 0;
    virtual QString getSystemName() = 0;
    virtual QString getUserName() = 0;
    virtual QWidget* getProcessingWidget() = 0;
    virtual void providerActivated(uint activeProvider) = 0;

    //! !!!
    // При необходимости методы должны быть реализованы в конкретном плагине обработки и зарегистрированы в пуле обработок :
    /*
     * @brief runtimeProcessingFunction - метод для обработки тайла "на лету".
     * @param tImg - матрица тайла;
     * @param psPtr - параметры обработки;
     * @param gioPtr - весь объект изображения.

    static void runtimeProcessingFunction( image_types::TImage* tImg,
                                           QSharedPointer<processing_system::ProcessingSettings> psPtr,
                                           QSharedPointer<dpf::io::GeoImageObject> gioPtr );


     * @brief imageProcessingFunction - метод для обработки всего изображения и сохранения результата в новый файл.
     * @param gioPtr - весь объект изображения;
     * @param psPtr - параметры обработки;
     * @param savePath - путь для сохранения результирующего изображения.

    static void imageProcessingFunction( QSharedPointer<dpf::io::GeoImageObject> gioPtr,
                                         QSharedPointer<processing_system::ProcessingSettings> psPtr,
                                         QString savePath );

     * @brief abortImageProcessingFunction - метод для отмены обработки всего изображения;
    static void abortImageProcessingFunction();

    */

protected slots:
    virtual void slotApplyRuntime(bool on_off) {}
    virtual void slotApplyForImage() {}

protected:
    uint getActiveProvider();
    QString getSavePath(const QString& filePath);
    virtual QSize getMinimumWidgetSize();
    virtual QSize getMaximumWidgetSize();

    QToolButton * _applyForImage;
    QCheckBox * _applyRuntime;
    QFrame * _topFrame, * _bottomFrame;
    bool _prevCheck;

private slots:
    void slotCurrentProviderChanged(uint curr, uint prev);
//    void slotSelectedProvidersChanged(QList<uint>);
    void slotProvidersRemoved(QList<uint> ids);
    void slotWidgetClosed();
    virtual void slotDisableWidget(QVariant var);
    virtual void slotEnabledWidget(QVariant var);

private:
    EmbIFaceNotifier* _iface;

    QWidget * _widget;
    QLabel * _fileNameTL;
    bool _isChecked;
    uint _activeProvider;
};

#endif

















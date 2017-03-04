#ifndef CTRLAYOUT_H
#define CTRLAYOUT_H

#include <QMainWindow>

/**
 * @brief The CtrLayout class - базовый класс компоновщик пользовательского интерфейса приложения
 */

class CtrLayout : public QMainWindow
{
    Q_OBJECT
public:
    explicit CtrLayout(QWidget *parent = 0);
    virtual ~CtrLayout();

    virtual bool init();

signals:
    
public slots:
    
};

#endif // CTRLAYOUT_H

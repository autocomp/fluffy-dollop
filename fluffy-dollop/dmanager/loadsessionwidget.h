#ifndef LOADSESSIONWIDGET_H
#define LOADSESSIONWIDGET_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

/**
 * @brief Виджет выбора загружаемой сессии
 */
class LoadSessionWidget : public QDialog
{
    Q_OBJECT

    QComboBox * m_pCbSessions;
    QPushButton *m_pBtnOk, *m_pBtnCancel;

public:
    /**
     * @brief Конструктор виджета
     * @param sessions - список доступных сессии
     */
    explicit LoadSessionWidget(QStringList sessions);

    /**
     * @brief Возвращает выбранную пользователем сессию
     * @return имя сессии
     */
    QString getSelectedSession();
signals:

public slots:
};

#endif // LOADSESSIONWIDGET_H

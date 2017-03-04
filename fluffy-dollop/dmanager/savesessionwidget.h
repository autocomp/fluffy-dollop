#ifndef SAVESESSIONWIDGET_H
#define SAVESESSIONWIDGET_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>

/**
 * @brief Виджет выбора загружаемой сессии
 */
class SaveSessionWidget : public QDialog
{
    Q_OBJECT

    QLineEdit * m_pLineEdit;
    QPushButton * m_pAddSession;
    QComboBox * m_pCbSessions;
    QPushButton *m_pBtnOk, *m_pBtnCancel;
    QStringList m_freeSessions;
    QStringList m_allSessions;

public:
    /**
     * @brief Конструктор виджета
     * @param sessions - полный список сессии
     * @param freeSession - список незалоченных сессии
     */
    explicit SaveSessionWidget(QStringList sessions, QStringList freeSession);

    /**
     * @brief Возвращает выбранную пользователем сессию
     * @return имя сессии
     */
    QString getSelectedSession();
signals:

public slots:
    void slotAddNewSession();
    void slotCheckEnteredText(QString str);
    void slotIndexChanged(QString str);
};

#endif // SAVESESSIONWIDGET_H

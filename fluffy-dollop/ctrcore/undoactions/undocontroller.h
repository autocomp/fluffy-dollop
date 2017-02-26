#ifndef UNDOCONTROLLER_H
#define UNDOCONTROLLER_H

#include <QObject>
#include <QWeakPointer>
#include <QList>
#include <QStack>
#include <QSet>
#include <QVariant>

namespace undo_actions {

/**
 * @brief The UndoAction class - базовый класс отмены операций.
 */
class UndoAction
{
public:
    /**
     * @brief UndoAction - конструктор операции отмены.
     * @param text - текст операции отмены для вывода оператору (переведённый на русский язык).
     */
    UndoAction(const QString& text);

    virtual ~UndoAction() {}

    /**
     * @brief getText - метод возвращет текст операции отмены.
     * @return
     */
    QString getText();

    /**
     * @brief getId - метод возвращет идентификатор операции отмены.
     * @return
     */
    uint getId();

    /**
     * @brief undo - метод вызываемый у конкретного экземпляра UndoAction при извлечении его из стека (по нажатию оператором ctrl+Z).
     * @return - при TRUE - считается что откат выполнен, испускаются сигналы для удаления выполненного действия и перерисовки виджета с текстом операций отката,
     *           при FALSE - само действие обязано как-то сообщить оператору о невозможности отката, конкретный экземпляр UndoAction укладывается обратно в стек.
     */
    virtual bool undo() = 0;

protected:
    const uint _id;
    QString _text;

private:
    static uint ID;
};


/**
 * @brief The RasterMatchingUndoAction class - класс отмены выполненых операций над растрами (изменения матрицы \ метаданных).
 */
class RasterUndoAction : public QObject, public undo_actions::UndoAction
{
    Q_OBJECT

public:
    RasterUndoAction(const QString& _name, const QList<uint>& _provIds);
    ~RasterUndoAction() {}
    virtual bool undo();
    void providerSavedOrDestroyed(uint rasterId);
    bool isValid();

signals:
    void chechUndoMatchingPossible(bool& ok);

protected:
    QSet<uint> provIds;
};


/**
 * @brief The UndoActionInfo struct - структура описывающая операцию отмены.
 */
struct UndoActionInfo
{
    UndoActionInfo(QString _text, uint _id);

    QString text;
    uint id;
};

/**
 * @brief The UndoController class - конроллер управления стеком отмены операций.
 */
class UndoController : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief instance - метод доступа к единственному экземпляру класса в приложении.
     * @return - экземпляр класса контроллера.
     */
    static UndoController *instance();

    /**
     * @brief pushUndoAction - метод добавления действия отмены в стек.
     * @param action - слабая ссылка на базовый класс операции отмены.
     */
    void pushUndoAction(QSharedPointer<UndoAction> action);

    /**
     * @brief getActionsInfo - получить информацию о всех операциях, последняя операция в конце списка.
     * @return
     */
    QList<UndoActionInfo> getActionsInfo();

    /**
     * @brief getActionById - возвращает действие отмены по заданному идентификатору, при отсутствии - возвращает не валидное действие.
     * @param id - идентификатор дейсвия отмены.
     * @return - действие отмены, необходимо проверить на валидность.
     */
    QSharedPointer<UndoAction> getActionById(uint id);

public slots:
    /**
     * @brief undoAction - отменить последнюю операцию, вызывается из "AppComposer" при нажатии "CTRL + Z".
     */
    void undoAction();

    /**
     * @brief removeUndoAction - удалить действия отмены. После удаления испускается сигнал "providerDestroyed" на каждое удаленное действие.
     * @param ids - идентификаторы дейсвий отмены для удаления из контроллера.
     */
    void removeUndoAction(const QSet<uint> &ids);

signals:
    /**
     * @brief undoControllerChanged - испускается после добовления операции или обновлении стека.
     *                                Принимается в виджете выводящем пользователю содержимое стека.
     */
    void undoControllerChanged();

    /**
     * @brief undoActionRemoved - испускается после выполнения действия отмены, вытеснения его из стека при переполнении или успешном удалалении из стека при вызове метода "removeUndoAction".
     *                           При необходимости принимается в модулях порождающих экземпляры классов на конкретные действия.
     *                           После получения сигнала необходимо удалить данные ассоциированные с данным действием отрмены операции.
     * @param id - идентификатор операции отмены.
     */
    void undoActionRemoved(uint id);

protected slots:
    void providerSaved(uint id);
    void providerSavedOrDestroyed(uint visualSystemRasterId, uint64_t objreprRasterId);
    void disableController(QVariant var);
    void enableController(QVariant var);

private:
    UndoController();
    static UndoController * _instance;
    uint _maxStackSize;
    QStack< QSharedPointer<UndoAction> > _stack;
    bool _controllerEnabled;
};

}

#endif // UNDOCONTROLLER_H

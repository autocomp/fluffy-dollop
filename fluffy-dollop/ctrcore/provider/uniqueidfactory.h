#ifndef UNIQUEIDFACTORY_H
#define UNIQUEIDFACTORY_H

/**
 * Класс предоставляет идентификатор для запроса на получение тайла.
 * Нулевой идентификатор считается не установленным ( т.е. фабрика возвращает номер ИД с 1 по масимальное значение int\uint )
 **/
class UniqueIdFactory
{
public:
    static UniqueIdFactory *instance()
    {
        if(!m_Instance)
        {
            m_Instance = new UniqueIdFactory;
        }

        return m_Instance;
    }

    ~UniqueIdFactory();

    unsigned int getIdQuery();
    int getListener();

private:
    UniqueIdFactory();
    static UniqueIdFactory *m_Instance;
    unsigned int idQueryCounter;
    int listenerCounter;
};

#endif // ifndef UNIQUEIDFACTORY_H


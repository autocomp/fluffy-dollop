#ifndef PARAMS_OBSERVER_H
#define PARAMS_OBSERVER_H

#include <QMap>
#include <QString>
#include <QObject>

//! максимальная глубина стека
#define SIZE 255
//! глубина вхложенности функции вызова
#define DEEP 3

//! Аргументы командной строки
typedef QMap<QString,QString>* Args;

//! Хранит значения макросов положения вызова
struct FunctionData
{
    QString func;
    QString file;
    int line;
};

/**
 * @brief The ParamsObserver class
 * Обозреватель вызова параметров,
 * сохраняет имена вызовов, анализирует стек вызова
 * и использует макросы положения вызова
 * для уведомления об ожидаемых параметрах командной строки,
 * а также параметрах из .xml файла
 */
class ParamsObserver: public QObject
{
    Q_OBJECT
public:
    ParamsObserver( QObject* parent = 0 );

    /**
     * @brief checkFunctionInMap проверка, есть ли параметр в списке параметров, помещение в список
     * @param param имя параметра
     * @param is_cmd флаг - обработка командной строки или xml
     * @return да, если флаг был впервые помещен
     */
    bool checkFunctionInMap(const QString &param, bool is_cmd = true);
    /**
     * @brief startTimerToPrint вызов таймера отображения начений параметров
     * @param delay задержка перед отображением, в мс
     */
    void startTimerToPrint( const int delay );
     /**
     * @brief timerStarted проверка, запущен ли таймер
     * @return да, если таймер был запущен
     */
    bool timerStarted();

    /**
     * @brief setVerbose установка степени подробности вывода (см. _verbose)
     * @param verbose степень подробности
     */
    void setVerbose( const int verbose );
    /**
     * @brief addFuncFileLine добавление значений макросов положения вызова
     * @param param ключ - имя параметра
     * @param func имя функции
     * @param file имя файла
     * @param line номер строки
     */
    void addFuncFileLine( const QString& param, const QString& func,
                          const QString& file, const int& line );
    /**
     * @brief setArgs устанавливается указатель на карту аргументов командной строки
     */
    void setArgs( Args args ){ _args = args; }

private Q_SLOTS:
    //! вызов вывода на экран
    void letPrint();

private:
    /**
     * @brief printParams вывод на экран параметров
     * @param verbose степень подробности
     */
    void printParams( const int verbose );
    /**
     * @brief printMapOfParam вывод одной карты параметров (cmd или xml)
     * @param map словарь параметров
     * @param verbose степень подробности
     * @param using_args нужно ли уведомлять об использовании
     */
    void printMapOfParam( QMap<QString, QString>& map, const int verbose ,
                          bool using_args = false );
    /**
     * @brief getFunctionFromBacktrace имя функции из анализа стека
     * @param step глубина вложения функции вызова
     * @return имя функции в системной сигнатуре
     */
    QString getFunctionFromBacktrace( const int step );

    //! параметры командной строки
    QMap<QString, QString> _bts;
    //! параметры из xml файлов
    QMap<QString, QString> _bts_xml;
    //! значения макросов вызова функции
    QMap<QString, FunctionData> _fds;

    /**
     * @brief _verbose параметр, отвечающий а подробность вывода
     * 0 - вывод откоючен
     * 1 - выводятся только имена параметров, факт использования
     * 2 - выводятся имена функций
     * 3 - выводятся значение макросов положения вывода
     */
    int _verbose;
    bool _timer_started = false;
    Args _args;
};

#endif // PARAMS_OBSERVER_H

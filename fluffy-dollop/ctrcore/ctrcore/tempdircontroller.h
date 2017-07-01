#ifndef TEMPDIRCONTROLLER_H
#define TEMPDIRCONTROLLER_H

#include <QString>

/**
 * @brief The TempDirController class - класс для взаимодействия с каталогами для временных данных.
 */
class TempDirController
{
public:
    /**
     * @brief checkTempDir - метод необходимо вызвать в самом начале запуска комплекса.
     * Метод проверяет в конфиге путь к общей временной папке и права на запись в ней.
     * В случае неудачи метод выводит сообщение и возвращает FALSE, после чего комплекс должен быть програмно закрыт (дальнейшая работа не возможна).
     */
    static bool checkTempDirForAllUsers();

    /**
     * @brief getTempDirForAllUsers - метод возвращает путь к каталогу для временных данных всех пользователей на данном компьютере.
     * @return - путь.
     */
    static QString getTempDirForAllUsers();

    /**
     * @brief getCurrentUserTempDir - метод возвращает путь к каталогу для временных данных текущего пользователя на данном компьютере.
     * @return - путь.
     */
    static QString getCurrentUserTempDir();

    /**
     * @brief createTempDirForCurrentUser - метод создает каталог в каталоге для временных данных текущего пользователя и возвращает полный путь к нему.
     * @param appendStrToDirName - строка добавляется в конец названия сгенерированного каталога. Необходимо если одновременно нужно создать несколько каталогов.
     * @return - путь.
     */
    static QString createTempDirForCurrentUser(QString appendStrToDirName = QString());

    /**
     * @brief clearCurrentUserTempDir -метод рекурсивно удаляет все временные данные из каталаго текущего пользователя.
     * Метод вызывается из деструктора класса-приложения (композер).
     */
    static void clearCurrentUserTempDir();

private:
    static void clearDir(const QString &dirPath);

};

#endif // TEMPDIRCONTROLLER_H

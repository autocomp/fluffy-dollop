#ifndef RBP_XLSX_TRANSLATOR_H
#define RBP_XLSX_TRANSLATOR_H

#include <regionbiz/rb_data_translator.h>
#include <regionbiz/rb_plugin_register.h>

namespace regionbiz {

REGISTER_PLUGIN( xlsx_tranlator )

class XlsxTranlsator: public QObject, public BaseDataTranslator
{
    Q_OBJECT
public:
    XlsxTranlsator();

    QString getTranslatorName() override
    {
        return "xlsx";
    }

    bool initBySettings( QVariantMap settings ) override;

protected:
    void loadFunctions() override;

private Q_SLOTS:
    void onObjectSelected(uint64_t /*old_id*/, uint64_t new_id );

private:
    struct RoomEntry
    {
        QString floor;
        QString name;
        QString descr;
        double area;
        QString arend;
    };

    void loadData(const QString &file_path );
    void clearData();

    FloorPtrs loadFloors();
    RoomPtrs loadRooms();

    QStringList _floor_names;
    std::map< QString, uint64_t > _floor_name_to_id;
    std::vector< RoomEntry > _entrys;

    uint _start_row = 0;
    QString _col_template = "";
    uint64_t _parent_id = 0;
};

}

#endif // RBP_XLSX_TRANSLATOR_H

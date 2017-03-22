#include <QtCore>
#include <qtxlsx/xlsx/xlsxdocument.h>

int main()
{
    QXlsx::Document xlsx_r("input.xlsx");
    QXlsx::Document xlsx_w("output.xlsx");
    for (uint col = 0; col < 15; col++)
        for (uint row = 0; row < 15; ++row) {
            if (QXlsx::Cell *cell=xlsx_r.cellAt(row, col))
            {
                if(cell->value().isValid())
                {
                    qDebug()<<cell->value();
                    xlsx_w.write(col,row, cell->value());
                }
            }
            xlsx_w.save();
        }

    return 0;
}

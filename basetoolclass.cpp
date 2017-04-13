#include "basetoolclass.h"

BaseToolClass::BaseToolClass(QSqlDatabase db, QTableWidget *navigatorReciversTable,
                             QTableWidget *changesReciversTable, QWidget* parent)
    : db(db),
      navigatorReciversTable(navigatorReciversTable),
      changesReciversTable(changesReciversTable),
      QToolBox(parent)
{

}

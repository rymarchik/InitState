#include "basetoolclass.h"

BaseToolClass::BaseToolClass(QSqlDatabase db, QTableWidget *navigatorReceiversTable,
                             QTableWidget *changesReceiversTable, QWidget* parent)
    : db(db),
      navigatorReceiversTable(navigatorReceiversTable),
      changesReceiversTable(changesReceiversTable),
      QToolBox(parent)
{

}

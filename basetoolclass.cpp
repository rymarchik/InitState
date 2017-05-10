#include "basetoolclass.h"

BaseToolClass::BaseToolClass(QSqlDatabase db, QTableWidget *navigatorReceiversTable,
                             QTableWidget *changesReceiversTable, QWidget* parent) : 
      QToolBox(parent),
      db(db),
      navigatorReceiversTable(navigatorReceiversTable),
      changesReceiversTable(changesReceiversTable)
{

}

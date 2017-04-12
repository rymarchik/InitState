#include "basetoolclass.h"

BaseToolClass::BaseToolClass(QSqlDatabase db, QTableWidget *navigatorReciversTable,
                             QTableWidget *changesReciversTable, QWidget* parent = 0) : QToolBox(parent)
{
    this->db = db;
    this->navigatorReciversTable = navigatorReciversTable;
    this->changesReciversTable = changesReciversTable;
}

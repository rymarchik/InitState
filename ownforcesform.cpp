#include "ownforcesform.h"
#include "ui_ownforcesform.h"

OwnForcesForm::OwnForcesForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OwnForcesForm)
{
    ui->setupUi(this);
}

/*
void OwnForcesForm::connectionDB(QSqlDatabase DB)
{
    db = DB;

    QSqlQuery sql;
    QStringList list;
    ui->treeWidget->clear();
    ui->treeWidget->setHeaderLabel("");

    QTreeWidgetItem *rootItem, *classifItem;
    QHash <QString,QTreeWidgetItem*> hashItems;

    list.clear();
    list << "Наименование" << "Состояние" << "Режим";
    rootItem = new QTreeWidgetItem(list);

    ui->treeWidget->setHeaderLabels(list);
    classifItem = rootItem;

    sql = QSqlQuery(db);
    qDebug() << db;

    if (!sql.exec("SELECT c.combat_hierarchy, c.object_number||' '||t1.termname "
                  "FROM own_forces.combatstructure c "
                  "LEFT JOIN reference_data.terms t1 ON object_name = t1.termhierarchy "
                  "LEFT JOIN reference_data.terms t2 ON type_army   = t2.termhierarchy "
                  "WHERE c.date_delete is NULL "))
    {
        db.rollback();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("warning"), sql.lastError().text());
        return;
    }

    while(sql.next())
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0,sql.value(1).toString().trimmed());
        //item->setText(0,sql.value(1).toString().trimmed());
        //item->setText(1,sql.value(2).toString().trimmed());
        //item->setToolTip(0, sql.value(2).toString().trimmed());
        //item->setText(2, sql.value(3).toString().trimmed());

        //настройка ширины колонки(0, //1, 2):
        ui->treeWidget->resizeColumnToContents(0);
        //ui->treeWidget->resizeColumnToContents(1);
        //ui->treeWidget->resizeColumnToContents(2);

        QString termhierarchy = sql.value(0).toString().trimmed();
        termhierarchy.contains(".", Qt::CaseInsensitive);
        QString sParentCode;
        if (!termhierarchy.contains(".", Qt::CaseInsensitive))
        {
            ui->treeWidget->addTopLevelItem(item);
            classifItem->addChild(item);
        }
        else
        {
            QString sCode = termhierarchy.right(2);
            int lendth=termhierarchy.length();
            lendth=lendth-3;
            sParentCode=termhierarchy.left(lendth);
            QHash <QString,QTreeWidgetItem*>::const_iterator i = hashItems.find(sParentCode);
            if (i != hashItems.end())
            {
                rootItem=i.value();
                rootItem->addChild(item);
                //ui->treeWidget->collapseItem(rootItem);
                ui->treeWidget->expandItem( rootItem );
            }
        }
        hashItems[termhierarchy] = item;
    }

    db.commit();

    //скрыть 4-ую колонку:
    ui->treeWidget->hideColumn(3);

    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);

    QApplication::restoreOverrideCursor();
    classifItem->setExpanded(true);
}
*/

OwnForcesForm::~OwnForcesForm()
{
    delete ui;
}

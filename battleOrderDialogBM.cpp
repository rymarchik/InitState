#include "battleOrderDialogBM.h"
#include "ui_battleOrderDialogBM.h"
#include <QtGui>

battleOrderDialogBM::battleOrderDialogBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    QDialog(parent),
    ui(new Ui::battleOrderDialogBM),
    m_Dialog(new battleOrderDialog(db, combatHierarchy, munition="", numDialog=0, this))
{
    ui->setupUi(this);

    connect(ui->ButtonAdd,    SIGNAL(clicked(bool)),    this, SLOT(slotAdd())    );
    connect(ui->ButtonEdit,   SIGNAL(clicked(bool)),    this, SLOT(slotEdit())   );
    connect(ui->ButtonDelete, SIGNAL(clicked(bool)),    this, SLOT(slotDelete()) );
    connect(m_Dialog,         SIGNAL(signalRecordDB()), this, SLOT(slotSend()) );
}

void battleOrderDialogBM::slotAdd()
{
    close();                                        //закрытие диалога m_DialogBM

    m_Dialog->numDlg = 1;                           //инициализация поля класса battleOrderDialog

    m_Dialog->setWindowTitle(tr("Добавить"));    
    showDialog(m_Dialog->numDlg);
    m_Dialog->setDisabledTextEdit(false, false);
    m_Dialog->getComboBoxName();
    m_Dialog->getCountPlace(m_Dialog->numDlg);
}

void battleOrderDialogBM::slotEdit()
{
    close();                                        //закрытие диалога m_DialogBM

    m_Dialog->numDlg = 2;                           //инициализация поля класса battleOrderDialog
    m_Dialog->munition = munition;                  //инициализация поля класса battleOrderDialog

    m_Dialog->setWindowTitle(tr("Изменить"));    
    showDialog(m_Dialog->numDlg);
    m_Dialog->setDisabledTextEdit(true, false);
    m_Dialog->getCountPlace(m_Dialog->numDlg);
}

void battleOrderDialogBM::slotDelete()
{
    close();                                        //закрытие диалога m_DialogBM

    m_Dialog->numDlg = 3;                           //инициализация поля класса battleOrderDialog
    m_Dialog->munition = munition;                  //инициализация поля класса battleOrderDialog

    m_Dialog->setWindowTitle(tr("Удалить"));    
    showDialog(m_Dialog->numDlg);
    m_Dialog->setDisabledTextEdit(true, true);
    m_Dialog->getCountPlace(m_Dialog->numDlg);
}

void battleOrderDialogBM::showDialog(int num)
{
    switch (num) {
    case 1:
        m_Dialog->show();
        //m_dlgDialog->startProcess();
        break;
    case 2:
        m_Dialog->show();
        //m_Dialoged->startProcess();
        break;
    case 3:
        m_Dialog->show();
        //m_Dialoged->startProcess();
        break;
    }
}

battleOrderDialogBM::~battleOrderDialogBM()
{
    delete m_Dialog;
    delete ui;
}

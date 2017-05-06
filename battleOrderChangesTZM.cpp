#include "battleOrderChangesTZM.h"
#include "ui_battleOrderChangesTZM.h"

battleOrderChangesTZM::battleOrderChangesTZM(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::battleOrderChangesTZM)
{
    ui->setupUi(this);
}

battleOrderChangesTZM::~battleOrderChangesTZM()
{
    delete ui;
}

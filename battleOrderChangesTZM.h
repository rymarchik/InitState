#ifndef BATTLEORDERCHANGESTZM_H
#define BATTLEORDERCHANGESTZM_H

#include <QWidget>

namespace Ui {
class battleOrderChangesTZM;
}

class battleOrderChangesTZM : public QWidget
{
    Q_OBJECT

public:
    explicit battleOrderChangesTZM(QWidget *parent = 0);
    ~battleOrderChangesTZM();

private:
    Ui::battleOrderChangesTZM *ui;
};

#endif // BATTLEORDERCHANGESTZM_H

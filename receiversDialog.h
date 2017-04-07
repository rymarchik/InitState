#ifndef RECEIVERSDIALOG_H
#define RECEIVERSDIALOG_H

#include <QtWidgets>

class ReceiversDialog : public QDialog
{
    Q_OBJECT

public:
    ReceiversDialog(QWidget* parent = 0);

    void clearLayout(QLayout*);
    void setCheckBoxes(int);
    void setLabelNames(QStringList);

private:
    QList<QCheckBox*>* checkBoxList;
    QList<QLabel*>* labelList;

    QVBoxLayout* checkBoxLayout;
    QVBoxLayout* labelLayout;

    QPushButton* okButton;
    QPushButton* cancelButton;

private slots:

};

#endif // RECEIVERSDIALOG_H

#include "positions.h"
#include "utility.h"

Positions::Positions(QWidget* parent) : QToolBox(parent)
{

}

void Positions::fillNavigator() {

}

void Positions::fillChanges() {

}

void Positions::onAdd() {
    //реализация кнопки добавить
}

void Positions::onEdit() {
    //реализация кнопки править
}

bool Positions::onDelete() {
    //реализация кнопки удалить
    return false;
}

bool Positions::onSave(int) {
    //реализация кнопки сохранить
    return false;
}

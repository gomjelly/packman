#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_packman.h"

class packman : public QMainWindow
{
    Q_OBJECT

public:
    packman(QWidget *parent = nullptr);
    ~packman();

private:
    Ui::packmanClass ui;
};

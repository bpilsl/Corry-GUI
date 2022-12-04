#include "runmanager.h"
#include "ui_runmanager.h"

RunManager::RunManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunManager)
{
    ui->setupUi(this);
}

RunManager::~RunManager()
{
    delete ui;
}

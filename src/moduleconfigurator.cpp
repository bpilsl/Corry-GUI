#include "moduleconfigurator.h"
#include "ui_moduleconfigurator.h"

ModuleConfigurator::ModuleConfigurator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModuleConfigurator)
{
    ui->setupUi(this);
}

ModuleConfigurator::~ModuleConfigurator()
{
    delete ui;
}

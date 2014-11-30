#include "configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    ui->setupUi(this);
}

void Configuration::on_buttonBox_accepted()
{
  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}

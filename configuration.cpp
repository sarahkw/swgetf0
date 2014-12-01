#include "configuration.h"
#include "ui_configuration.h"

#include <QDebug>

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    ui->setupUi(this);
}

void Configuration::on_buttonBox_accepted()
{
  QFile initFile(":/tinyscheme/init.scm");
  Q_ASSERT(initFile.open(QFile::ReadOnly));
  QByteArray initFileTextBa(initFile.readAll());

  scheme* sc = scheme_init_new();
  Q_ASSERT(sc != nullptr);

  scheme_set_input_port_file(sc, stdin);
  scheme_set_output_port_file(sc, stdout);


  QString configText = ui->txtConfig->toPlainText();
  QByteArray configTextBa = configText.toLatin1();
  scheme_load_string(sc, configTextBa.data());

  // init.scm
  scheme_load_string(sc, initFileTextBa.data());

  scheme_load_string(sc, "(define (get-sample-rate) (cadr (assv 'sample-rate config)))");

  pointer func = scheme_eval(sc, mk_symbol(sc, "get-sample-rate"));
  pointer ret = scheme_call(sc, func, sc->NIL);

  qDebug() << ret->_flag;
  qDebug() << sc->vptr->ivalue(ret);

  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}

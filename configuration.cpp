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
  scheme* sc = scheme_init_new();
  Q_ASSERT(sc != nullptr);

  scheme_set_input_port_file(sc, stdin);
  scheme_set_output_port_file(sc, stdout);


  scheme_load_string(sc, "(define config '((sample-rate 44100) (happy #t)))");

  scheme_load_string(sc,
		     "(define (caar x) (car (car x)))\n"
		     ";;;; generic-assoc\n"
		     "(define (generic-assoc cmp obj alst)\n"
		     "     (cond\n"
		     "          ((null? alst) #f)\n"
		     "          ((cmp obj (caar alst)) (car alst))\n"
		     "          (else (generic-assoc cmp obj (cdr alst)))))\n"
		     "\n"
		     "(define (assq obj alst)\n"
		     "     (generic-assoc eq? obj alst))\n"
		     "(define (assv obj alst)\n"
		     "     (generic-assoc eqv? obj alst))\n"
		     "(define (assoc obj alst)\n"
		     "     (generic-assoc equal? obj alst))\n");

  scheme_load_string(sc, "(define (get-sample-rate) (car (cdr (assv 'sample-rate config))))");

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

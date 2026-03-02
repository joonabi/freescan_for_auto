#include "einscantabwidget.h"
#include "ui_einscantabwidget.h"

EinScanTabWidget::EinScanTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EinScanTabWidget)
{
    ui->setupUi(this);
}

EinScanTabWidget::~EinScanTabWidget()
{
    delete ui;
}

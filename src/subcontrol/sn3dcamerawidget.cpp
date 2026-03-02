#include "sn3dcamerawidget.h"
#include "ui_sn3dcamerawidget.h"

Sn3DCameraWidget::Sn3DCameraWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sn3DCameraWidget)
{
    ui->setupUi(this);
}

Sn3DCameraWidget::~Sn3DCameraWidget()
{
    delete ui;
}

void Sn3DCameraWidget::updateImage(QPixmap&pixmap)
{
	ui->widget_back->setMaximumHeight(0);
	ui->label_img->setPixmap(pixmap);
	ui->label_img->setMinimumHeight(this->height());
	ui->label_img->setMaximumHeight(this->height());
}

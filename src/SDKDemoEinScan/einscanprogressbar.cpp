#include "einscanprogressbar.h"
#include "ui_einscanprogressbar.h"

EinscanProgressBar::EinscanProgressBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EinscanProgressBar)
{
    ui->setupUi(this);

}

EinscanProgressBar::~EinscanProgressBar()
{
    delete ui;
}

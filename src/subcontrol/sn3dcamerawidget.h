#pragma once

#include <QWidget>
#include <QPixmap>
namespace Ui {
class Sn3DCameraWidget;
}

class Sn3DCameraWidget : public QWidget
{
    Q_OBJECT

public:
    Sn3DCameraWidget(QWidget *parent = 0);
    ~Sn3DCameraWidget();
	void updateImage(QPixmap&pixmap);

private:
    Ui::Sn3DCameraWidget *ui;
};

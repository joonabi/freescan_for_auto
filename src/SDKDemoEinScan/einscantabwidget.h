#ifndef EINSCANTABWIDGET_H
#define EINSCANTABWIDGET_H

#include <QWidget>

namespace Ui {
class EinScanTabWidget;
}

class EinScanTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EinScanTabWidget(QWidget *parent = nullptr);
    ~EinScanTabWidget();

private:
    Ui::EinScanTabWidget *ui;
};

#endif // EINSCANTABWIDGET_H

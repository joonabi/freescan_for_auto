#ifndef EINSCANPROGRESSBAR_H
#define EINSCANPROGRESSBAR_H

#include <QWidget>

namespace Ui {
class EinscanProgressBar;
}

class EinscanProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit EinscanProgressBar(QWidget *parent = nullptr);
    ~EinscanProgressBar();

private:
    Ui::EinscanProgressBar *ui;
};

#endif // EINSCANPROGRESSBAR_H

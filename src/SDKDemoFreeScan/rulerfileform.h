#ifndef RULERFILEFORM_H
#define RULERFILEFORM_H

#include <QWidget>
#include "qstringlist.h"

namespace Ui {
class RulerFileForm;
}

class PGFItemModel;
class PGFItemDelegate;
class QAbstractListModel;

class RulerFileForm : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QStringList rulerFileList READ rulerFileList WRITE setRulerFileList NOTIFY rulerFileListChanged)
public:
    explicit RulerFileForm(QWidget *parent = nullptr);
    ~RulerFileForm();
Q_SIGNALS:
    void rulerFileListChanged(const QStringList& rulerFileList);
public:
    QStringList rulerFileList();
    void setRulerFileList(const QStringList& rulerFileList);
public Q_SLOTS:
    virtual void on_btnApply_clicked(bool checked = false);
    virtual void on_btnDelLast_clicked(bool checked = false);
private:
    Ui::RulerFileForm *ui;
    QStringList m_runlerFileList;
    QAbstractListModel* m_pPGFIemModel;
    PGFItemDelegate* m_pPGFItemDelegate;
};

#endif // RULERFILEFORM_H

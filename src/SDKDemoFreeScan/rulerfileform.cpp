#include "rulerfileform.h"
#include "ui_rulerfileform.h"
#include "qfiledialog.h"
#include "qabstractbutton.h"
#include "qabstractlistmodel"
// #include "qabstractitemmodel.h"
#include "qstringlistmodel.h"
#include "pgfitemmodel.h"
#include "pgfitemdelegate.h"

RulerFileForm::RulerFileForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RulerFileForm),
    m_pPGFIemModel(new QStringListModel(this)),
    m_pPGFItemDelegate(new PGFItemDelegate(this))
{
    ui->setupUi(this);
    ui->lvwPGF->setModel(m_pPGFIemModel);
    // ui->lvwPGF->setItemDelegate(m_pPGFItemDelegate);
    connect(ui->btnOpenPGF, &QAbstractButton::clicked, [this]() {
        QFileDialog dlg;
        auto listPGF = dlg.getOpenFileNames(this, tr("Select ruler files"), qApp->applicationDirPath(), "PGF files(*.pgf);;All files(*.*)");
        foreach(const auto v, listPGF) {
            m_pPGFIemModel->insertRow(m_pPGFIemModel->rowCount());
            auto index = m_pPGFIemModel->index(m_pPGFIemModel->rowCount() - 1, 0);
            QFileInfo fileInfo(v);
            m_pPGFIemModel->setData(index, fileInfo.fileName());
            m_runlerFileList.append(v);
        }
    });
}

RulerFileForm::~RulerFileForm()
{
    delete ui;
}

QStringList RulerFileForm::rulerFileList()
{
    return m_runlerFileList;
}

void RulerFileForm::setRulerFileList(const QStringList& rulerFileList)
{
    m_runlerFileList = rulerFileList;
    m_pPGFIemModel->removeRows(0, m_pPGFIemModel->rowCount());
    foreach(const auto& v, m_runlerFileList) {
        m_pPGFIemModel->insertRow(m_pPGFIemModel->rowCount());
        QFileInfo fileInfo(v);
        auto index = m_pPGFIemModel->index(m_pPGFIemModel->rowCount() - 1, 0);
        m_pPGFIemModel->setData(index, fileInfo.fileName());
    }
}

void RulerFileForm::on_btnApply_clicked(bool checked)
{
    emit rulerFileListChanged(m_runlerFileList);
}

void RulerFileForm::on_btnDelLast_clicked(bool checked)
{
    if (m_runlerFileList.count() > 0) {
        m_runlerFileList.removeLast();
        m_pPGFIemModel->removeRow(m_pPGFIemModel->rowCount() - 1);
    }
}

#ifndef SN3DSDKMESH_H
#define SN3DSDKMESH_H

#include <QDialog>
#include <unordered_map>
#include "qvariant.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Sn3DSDKMesh; }
QT_END_NAMESPACE

// filter type [11/14/2023 sbc]
enum class FILTER_TYPE
{
	INVALID = -1,
	NONE,
	LOW,
	MED,
	HIGH
};

enum class SMOOTH_TYPE
{
	INVALID = 0,
	LOW,
	MED,
	HIGH
};

enum class WATERTIGHT_TYPE
{
	INVALID = -1,
	UNWATERTIGHT,
	HALFWATERTIGHT,
	WATERTIGHT
};

class SDKWrapperBase;
class Sn3DSDKMesh : public QDialog

{
    Q_OBJECT

public:
    Sn3DSDKMesh(QWidget *parent = nullptr);
    ~Sn3DSDKMesh();
Q_SIGNALS:
    void messageReady(QVariantMap mapParam);
	void sigPrintInfo(QString);
private Q_SLOTS:
	void on_btnApply_clicked(bool bChecked = false);
	void on_btnUndo_clicked(bool bChecked = false);
	void on_btnConfirm_clicked(bool bChecked = false);
	void on_btnSaveMesh_clicked(bool bChecked = false);
    void on_radioButton_unwatertight_clicked();

    void on_radioButton_halfwatertight_clicked();

    void on_radioButton_watertight_clicked();

    void on_radioButton_smoothLow_clicked();

    void on_radioButton_smoothMed_clicked();

    void on_radioButton_smoothHigh_clicked();

private:
    Ui::Sn3DSDKMesh *ui;
	
	FILTER_TYPE m_filterType = FILTER_TYPE::LOW;	// filter type [11/14/2023 sbc]
    WATERTIGHT_TYPE m_watertightType=WATERTIGHT_TYPE::UNWATERTIGHT;
    SMOOTH_TYPE m_smoothType=SMOOTH_TYPE::LOW;
	SDKWrapperBase* sdkWrapper_;
};

#endif // SN3DSDKMESH_H

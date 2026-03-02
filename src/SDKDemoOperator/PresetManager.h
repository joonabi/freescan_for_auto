#pragma once
#include <QString>
#include <QList>
#include "sn3dsdkpluginpub.h"   // SNSDK_DEVTYPE_*, SNSDK_SCANMODE_*

// ─────────────────────────────────────────────────────────────────
// ScanPreset : 한 개의 스캔 프리셋에 저장되는 모든 설정값
// ─────────────────────────────────────────────────────────────────
struct ScanPreset
{
    // [식별]
    QString name          = QString::fromUtf8("기본 프리셋");
    QString description   = "";

    // [저장 경로] – 프로젝트 폴더의 상위 디렉토리
    QString saveDirectory = "C:/ScanResults";

    // [기기 설정]
    int  deviceType  = SNSDK_DEVTYPE_COMBO;   // COMBO, UEP2, TRIO ...
    int  scanMode    = SNSDK_SCANMODE_LASER;  // 0=LASER, 1=INFRARED

    // [스캔 파라미터]
    int  brightStep  = 3;    // 밝기 단계 (FreeScanPropPanel 기준 인덱스)
    int  laserGrade  = 2;    // 0=OFF, 1=SINGLE, 2=26선
    int  objectType  = 0;    // 0=Normal, 1=Black, 2=Reflect
    bool highSpeed   = false;
    bool strongLight = false;
    bool useMarker   = true;
    bool useTexture  = false;

    // [메시 파라미터] – FreeScanMeshPar 구조체와 1:1 대응
    int  watertightType      = 0;    // 0=open, 1=watertight
    int  filterLevel         = 1;    // 0=LOW, 1=MED, 2=HIGH
    int  smoothValue         = 1;    // 0~3
    bool fillSmallHole       = true;
    int  fillSmallHoleValue  = 3;
    bool removeFloating      = true;
    bool removeSpike         = false;
    bool markerHoleFilling   = true;

    // [저장 형식]
    bool saveStl = true;
    bool saveObj = false;
    bool savePly = false;
};

// ─────────────────────────────────────────────────────────────────
// PresetManager : 프리셋 목록을 JSON 파일로 관리하는 싱글턴
//   저장 경로: <실행파일 폴더>/presets.json
// ─────────────────────────────────────────────────────────────────
class PresetManager
{
public:
    static PresetManager& instance();

    bool load();                            // presets.json → m_presets
    bool save();                            // m_presets → presets.json

    const QList<ScanPreset>& presets() const;
    ScanPreset  currentPreset() const;
    void        setCurrentPreset(const QString& name);
    void        addOrUpdate(const ScanPreset& p);
    void        remove(const QString& name);

private:
    PresetManager();
    QString filePath() const;

    QList<ScanPreset> m_presets;
    QString           m_currentName;
};

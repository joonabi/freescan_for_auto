#include "PresetManager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

// ─────────────────────────────────────────────────────────────────
// 생성자: 기본 프리셋 하나를 추가
// ─────────────────────────────────────────────────────────────────
PresetManager::PresetManager()
{
    ScanPreset def;
    m_presets.append(def);
    m_currentName = def.name;
}

PresetManager& PresetManager::instance()
{
    static PresetManager inst;
    return inst;
}

QString PresetManager::filePath() const
{
    return QCoreApplication::applicationDirPath() + "/presets.json";
}

// ─────────────────────────────────────────────────────────────────
// load : presets.json → m_presets
// ─────────────────────────────────────────────────────────────────
bool PresetManager::load()
{
    QFile f(filePath());
    if (!f.open(QIODevice::ReadOnly)) return false;

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (doc.isNull()) return false;

    auto root = doc.object();
    m_currentName = root["current"].toString();
    m_presets.clear();

    for (const auto& v : root["presets"].toArray()) {
        auto o = v.toObject();
        ScanPreset p;

        p.name             = o["name"].toString(p.name);
        p.description      = o["description"].toString(p.description);
        p.saveDirectory    = o["saveDirectory"].toString(p.saveDirectory);

        p.deviceType       = o["deviceType"].toInt(p.deviceType);
        p.scanMode         = o["scanMode"].toInt(p.scanMode);

        p.brightStep       = o["brightStep"].toInt(p.brightStep);
        p.laserGrade       = o["laserGrade"].toInt(p.laserGrade);
        p.objectType       = o["objectType"].toInt(p.objectType);
        p.highSpeed        = o["highSpeed"].toBool(p.highSpeed);
        p.strongLight      = o["strongLight"].toBool(p.strongLight);
        p.useMarker        = o["useMarker"].toBool(p.useMarker);
        p.useTexture       = o["useTexture"].toBool(p.useTexture);

        p.watertightType     = o["watertightType"].toInt(p.watertightType);
        p.filterLevel        = o["filterLevel"].toInt(p.filterLevel);
        p.smoothValue        = o["smoothValue"].toInt(p.smoothValue);
        p.fillSmallHole      = o["fillSmallHole"].toBool(p.fillSmallHole);
        p.fillSmallHoleValue = o["fillSmallHoleValue"].toInt(p.fillSmallHoleValue);
        p.removeFloating     = o["removeFloating"].toBool(p.removeFloating);
        p.removeSpike        = o["removeSpike"].toBool(p.removeSpike);
        p.markerHoleFilling  = o["markerHoleFilling"].toBool(p.markerHoleFilling);

        p.saveStl  = o["saveStl"].toBool(p.saveStl);
        p.saveObj  = o["saveObj"].toBool(p.saveObj);
        p.savePly  = o["savePly"].toBool(p.savePly);

        m_presets.append(p);
    }

    if (m_presets.isEmpty()) {
        m_presets.append(ScanPreset());
        m_currentName = m_presets.first().name;
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────
// save : m_presets → presets.json
// ─────────────────────────────────────────────────────────────────
bool PresetManager::save()
{
    QJsonArray arr;
    for (const auto& p : m_presets) {
        QJsonObject o;
        o["name"]             = p.name;
        o["description"]      = p.description;
        o["saveDirectory"]    = p.saveDirectory;

        o["deviceType"]       = p.deviceType;
        o["scanMode"]         = p.scanMode;

        o["brightStep"]       = p.brightStep;
        o["laserGrade"]       = p.laserGrade;
        o["objectType"]       = p.objectType;
        o["highSpeed"]        = p.highSpeed;
        o["strongLight"]      = p.strongLight;
        o["useMarker"]        = p.useMarker;
        o["useTexture"]       = p.useTexture;

        o["watertightType"]     = p.watertightType;
        o["filterLevel"]        = p.filterLevel;
        o["smoothValue"]        = p.smoothValue;
        o["fillSmallHole"]      = p.fillSmallHole;
        o["fillSmallHoleValue"] = p.fillSmallHoleValue;
        o["removeFloating"]     = p.removeFloating;
        o["removeSpike"]        = p.removeSpike;
        o["markerHoleFilling"]  = p.markerHoleFilling;

        o["saveStl"]  = p.saveStl;
        o["saveObj"]  = p.saveObj;
        o["savePly"]  = p.savePly;

        arr.append(o);
    }

    QJsonObject root;
    root["current"] = m_currentName;
    root["presets"] = arr;

    QFile f(filePath());
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

// ─────────────────────────────────────────────────────────────────
// 조회 / 수정
// ─────────────────────────────────────────────────────────────────
const QList<ScanPreset>& PresetManager::presets() const { return m_presets; }

ScanPreset PresetManager::currentPreset() const
{
    for (const auto& p : m_presets)
        if (p.name == m_currentName) return p;
    return m_presets.isEmpty() ? ScanPreset() : m_presets.first();
}

void PresetManager::setCurrentPreset(const QString& name) { m_currentName = name; }

void PresetManager::addOrUpdate(const ScanPreset& p)
{
    for (auto& existing : m_presets) {
        if (existing.name == p.name) { existing = p; return; }
    }
    m_presets.append(p);
}

void PresetManager::remove(const QString& name)
{
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].name == name) { m_presets.removeAt(i); return; }
    }
}

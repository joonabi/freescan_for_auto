#include "DataManager.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>

DataManager::DataManager(QObject* parent)
    : QObject(parent)
{
    QString iniPath = QCoreApplication::applicationDirPath() + "/qmlcontroller.ini";
    m_settings = new QSettings(iniPath, QSettings::IniFormat, this);
    m_settings->setIniCodec("UTF-8");
}

// ── 수요처 (clients) ──────────────────────────────────────────
void DataManager::saveClients(const QVariantList& clients)
{
    saveMapList("Clients/list", clients);
    emit dataSaved();
}

QVariantList DataManager::loadClients()
{
    auto list = loadMapList("Clients/list");
    if (list.isEmpty()) {
        // 기본값
        return QVariantList{
            QVariantMap{{"id",1},{"name","현대자동차"},{"type","자동차 제조사"},{"logo",""}},
            QVariantMap{{"id",2},{"name","기아자동차"},{"type","자동차 제조사"},{"logo",""}},
            QVariantMap{{"id",3},{"name","삼성전자"},{"type","전자 제조사"},{"logo",""}}
        };
    }
    return list;
}

// ── 작업장소 (locations) ──────────────────────────────────────
void DataManager::saveLocations(const QVariantList& locations)
{
    saveStringList("Workplace/locations", locations);
    emit dataSaved();
}

QVariantList DataManager::loadLocations()
{
    return loadStringList("Workplace/locations",
        QVariantList{"본사공장", "1공장", "2공장", "검사실"});
}

// ── 작업업무 (tasks) ──────────────────────────────────────────
void DataManager::saveTasks(const QVariantList& tasks)
{
    saveStringList("Workplace/tasks", tasks);
    emit dataSaved();
}

QVariantList DataManager::loadTasks()
{
    return loadStringList("Workplace/tasks",
        QVariantList{"외관검사", "치수측정", "역설계", "품질검증"});
}

// ── 작업자 (workers) ──────────────────────────────────────────
void DataManager::saveWorkers(const QVariantList& workers)
{
    saveStringList("Workplace/workers", workers);
    emit dataSaved();
}

QVariantList DataManager::loadWorkers()
{
    return loadStringList("Workplace/workers",
        QVariantList{"김철수", "이영희", "박민수", "최지현"});
}

// ── 시스템 설정 ───────────────────────────────────────────────
void DataManager::saveSettings(const QVariantMap& settings)
{
    m_settings->beginGroup("System");
    for (auto it = settings.cbegin(); it != settings.cend(); ++it)
        m_settings->setValue(it.key(), it.value());
    m_settings->endGroup();
    m_settings->sync();
    emit dataSaved();
}

QVariantMap DataManager::loadSettings()
{
    QVariantMap result;
    m_settings->beginGroup("System");
    for (const QString& key : m_settings->childKeys())
        result[key] = m_settings->value(key);
    m_settings->endGroup();

    // 기본값
    if (!result.contains("clientName"))   result["clientName"]   = "J3D";
    if (!result.contains("wifiIp"))       result["wifiIp"]       = "192.168.1.100";
    if (!result.contains("wifiPort"))     result["wifiPort"]     = 11080;
    if (!result.contains("selectedClient"))   result["selectedClient"]   = "현대자동차";
    if (!result.contains("selectedLocation")) result["selectedLocation"] = "본사공장";
    if (!result.contains("selectedTask"))     result["selectedTask"]     = "외관검사";
    if (!result.contains("selectedWorker"))   result["selectedWorker"]   = "김철수";
    return result;
}

// ── 스캔 설정 ─────────────────────────────────────────────────
void DataManager::saveScanSettings(const QVariantMap& settings)
{
    m_settings->beginGroup("ScanSettings");
    for (auto it = settings.cbegin(); it != settings.cend(); ++it)
        m_settings->setValue(it.key(), it.value());
    m_settings->endGroup();
    m_settings->sync();
    emit dataSaved();
}

QVariantMap DataManager::loadScanSettings()
{
    QVariantMap result;
    m_settings->beginGroup("ScanSettings");
    for (const QString& key : m_settings->childKeys())
        result[key] = m_settings->value(key);
    m_settings->endGroup();

    // 기본값
    if (!result.contains("saveDirectory"))   result["saveDirectory"]   = "C:/3DScan/Results";
    if (!result.contains("lightSource"))     result["lightSource"]     = "Laser";
    if (!result.contains("scanMode"))        result["scanMode"]        = "mesh";
    if (!result.contains("laserLine"))       result["laserLine"]       = "cross";
    if (!result.contains("material"))        result["material"]        = "normal";
    if (!result.contains("filterLevel"))     result["filterLevel"]     = "2";
    if (!result.contains("smoothLevel"))     result["smoothLevel"]     = "1";
    if (!result.contains("fillMarkerHoles")) result["fillMarkerHoles"] = true;
    if (!result.contains("fillSmallHoles"))  result["fillSmallHoles"]  = true;
    if (!result.contains("removeAnomalies")) result["removeAnomalies"] = true;
    if (!result.contains("removeSpikes"))    result["removeSpikes"]    = true;
    if (!result.contains("fileFormat"))      result["fileFormat"]      = "stl";
    return result;
}

// ── 내부 유틸 ─────────────────────────────────────────────────
void DataManager::saveStringList(const QString& key, const QVariantList& list)
{
    QStringList sl;
    for (const QVariant& v : list)
        sl << v.toString();
    m_settings->setValue(key, sl);
    m_settings->sync();
}

QVariantList DataManager::loadStringList(const QString& key, const QVariantList& defaultValue)
{
    if (!m_settings->contains(key))
        return defaultValue;
    QStringList sl = m_settings->value(key).toStringList();
    QVariantList result;
    for (const QString& s : sl)
        result << s;
    return result;
}

void DataManager::saveMapList(const QString& key, const QVariantList& list)
{
    QJsonArray arr;
    for (const QVariant& v : list)
        arr.append(QJsonObject::fromVariantMap(v.toMap()));
    m_settings->setValue(key, QJsonDocument(arr).toJson(QJsonDocument::Compact));
    m_settings->sync();
}

QVariantList DataManager::loadMapList(const QString& key)
{
    QVariantList result;
    QString json = m_settings->value(key).toString();
    if (json.isEmpty())
        return result;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isArray())
        return result;
    for (const QJsonValue& v : doc.array())
        result << v.toObject().toVariantMap();
    return result;
}

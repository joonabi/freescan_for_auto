#pragma once
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QSettings>

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    // 수요처 관리
    Q_INVOKABLE void saveClients(const QVariantList& clients);
    Q_INVOKABLE QVariantList loadClients();

    // 작업장소
    Q_INVOKABLE void saveLocations(const QVariantList& locations);
    Q_INVOKABLE QVariantList loadLocations();

    // 작업업무
    Q_INVOKABLE void saveTasks(const QVariantList& tasks);
    Q_INVOKABLE QVariantList loadTasks();

    // 작업자
    Q_INVOKABLE void saveWorkers(const QVariantList& workers);
    Q_INVOKABLE QVariantList loadWorkers();

    // 시스템 설정 (수요처명, 현재 작업장, WiFi IP/Port 등)
    Q_INVOKABLE void saveSettings(const QVariantMap& settings);
    Q_INVOKABLE QVariantMap loadSettings();

    // 스캔 설정 (광원, 모드, 후처리 등)
    Q_INVOKABLE void saveScanSettings(const QVariantMap& settings);
    Q_INVOKABLE QVariantMap loadScanSettings();

signals:
    void dataSaved();

private:
    QSettings* m_settings = nullptr;

    void saveStringList(const QString& key, const QVariantList& list);
    QVariantList loadStringList(const QString& key, const QVariantList& defaultValue = {});
    void saveMapList(const QString& key, const QVariantList& list);
    QVariantList loadMapList(const QString& key);
};

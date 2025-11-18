#include "levelmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

LevelManager::LevelManager()
    : m_currentWaveIndex(0)
{
    // 파일 로드 실패 등에 대비한 기본값 설정 (Level 1)
    m_currentWaveData = WaveData(1, 10, 2.0f, 3.0f, 0);
}

bool LevelManager::loadLevelsFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open level file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON";
        return false;
    }

    if (!doc.isArray()) {
        qDebug() << "JSON root is not an array";
        return false;
    }

    // 기존 데이터 초기화
    m_waves.clear();
    QJsonArray array = doc.array();

    for (const QJsonValue &val : array) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();
            m_waves.append(WaveData(
                obj["level"].toInt(),
                obj["totalEnemies"].toInt(),
                (float)obj["spawnInterval"].toDouble(),
                (float)obj["missileSpeed"].toDouble(),
                obj["specialEnemyCount"].toInt()
                ));
        }
    }

    qDebug() << "Levels loaded:" << m_waves.size();

    // 첫 번째 웨이브 로드
    // todo:
    if (!m_waves.isEmpty()) {
        m_currentWaveIndex = 2;
        m_currentWaveData = m_waves[2];
        return true;
    }

    return false;
}

const WaveData& LevelManager::getCurrentWave() const
{
    return m_currentWaveData;
}

void LevelManager::decreaseEnemyCount()
{
    if (m_currentWaveData.totalEnemies > 0) {
        m_currentWaveData.totalEnemies--;
    }
}

// [구현 추가] 특수 적 카운트 감소
void LevelManager::decreaseSpecialEnemyCount()
{
    if (m_currentWaveData.specialEnemyCount > 0) {
        m_currentWaveData.specialEnemyCount--;
    }
}

// [구현 추가] 남은 특수 적 개수 반환
int LevelManager::getRemainingSpecialEnemies() const
{
    return m_currentWaveData.specialEnemyCount;
}

bool LevelManager::isWaveFinished() const
{
    return m_currentWaveData.totalEnemies <= 0;
}

void LevelManager::nextWave()
{
    m_currentWaveIndex++;

    if (m_currentWaveIndex < m_waves.size()) {
        // 1. JSON에 정의된 다음 레벨이 있으면 로드
        m_currentWaveData = m_waves[m_currentWaveIndex];
        qDebug() << "Level Up! Now Level:" << m_currentWaveData.levelIndex;
    } else {
        // 2. JSON 데이터가 떨어지면? -> 무한 모드 (절차적 난이도 상승)
        m_currentWaveData.levelIndex++;
        m_currentWaveData.totalEnemies += 5;        // 적 5마리 추가
        m_currentWaveData.missileSpeed *= 1.1f;     // 속도 10% 증가
        m_currentWaveData.spawnInterval *= 0.9f;    // 스폰 주기 10% 단축 (더 자주 나옴)
        m_currentWaveData.specialEnemyCount += 1;   // 특수 적 추가

        // 스폰 주기가 너무 짧아지는 것 방지 (최소 0.2초)
        if (m_currentWaveData.spawnInterval < 0.2f)
            m_currentWaveData.spawnInterval = 0.2f;

        qDebug() << "Infinite Mode! Procedural Level:" << m_currentWaveData.levelIndex;
    }
}

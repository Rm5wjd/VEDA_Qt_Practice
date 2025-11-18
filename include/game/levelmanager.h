#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <QVector>
#include <QString>
#include <QJsonObject>

// 1. 웨이브 데이터 구조체
struct WaveData {
    int levelIndex;          // 레벨 번호
    int totalEnemies;        // 총 적 숫자
    float spawnInterval;     // 스폰 간격 (초)
    float missileSpeed;      // 미사일 속도
    int specialEnemyCount;   // 특수 적 숫자 (비행기 등)

    // 기본 생성자
    WaveData() : levelIndex(1), totalEnemies(10), spawnInterval(2.0f), missileSpeed(2.0f), specialEnemyCount(0) {}

    WaveData(int lvl, int total, float interval, float speed, int special)
        : levelIndex(lvl), totalEnemies(total), spawnInterval(interval),
        missileSpeed(speed), specialEnemyCount(special) {}
};

class LevelManager
{
public:
    LevelManager();

    // JSON 파일에서 레벨 데이터 로드 (성공 시 true 반환)
    bool loadLevelsFromFile(const QString& filePath);

    // 현재 웨이브 정보 가져오기
    const WaveData& getCurrentWave() const;

    // 다음 웨이브로 진행
    void nextWave();

    // 현재 웨이브의 모든 적 스폰이 끝났는지 확인
    bool isWaveFinished() const;

    // 적 스폰 시 카운트 감소
    void decreaseEnemyCount();

    // [추가] 특수 적(비행기) 카운트 감소
    void decreaseSpecialEnemyCount();

    // [추가] 남은 특수 적 개수 반환
    int getRemainingSpecialEnemies() const;

    // 현재 레벨 번호 반환
    int getCurrentLevelIndex() const { return m_currentWaveData.levelIndex; }

    void reset();

private:
    QVector<WaveData> m_waves;  // 로드된 레벨 리스트
    int m_currentWaveIndex;     // 리스트 인덱스
    WaveData m_currentWaveData; // 현재 활성화된 웨이브 데이터 (복사본)
};

#endif // LEVELMANAGER_H

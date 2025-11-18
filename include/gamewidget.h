#ifndef GAMEWIDGET_H
#define GAMEWidget_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QVector2D>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

#include "cannon.h"
#include "enemy.h"
#include "player.h"
#include "explosion.h"
#include "levelmanager.h"
#include "item.h"

struct FloatingText {
    QPointF pos;
    QString text;
    QColor color;
    int lifeTime = 60; // 60프레임 (약 1초) 동안 유지

    void update() {
        pos.ry() -= 1.0f; // 위로 올라감
        lifeTime--;
    }
};

// (E) 메인 게임 위젯
class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateGameLogic(); // 메인 게임 루프

signals:
    void gameFinished(int finalScore);

private:
    LevelManager m_levelManager;

    // 게임 관리
    QTimer *m_gameTimer;
    QElapsedTimer m_gameClock;
    PlayerStats m_stats;
    QPointF m_mousePos;
    qint64 m_nextEnemySpawnTime = 0; // 다음 적 스폰 시간
    int m_score = 0;

    int m_timerBigBoom = 0;   // 폭발 범위 증가 남은 시간(ms)
    int m_timerRapidFire = 0; // 고속 발사 남은 시간(ms)

    const int ITEM_DURATION_MS = 5000; // 지속시간 10초

    // 게임 객체 리스트
    Cannon m_cannons[3];
    Building m_buildings[6];
    QList<PlayerProjectile> m_projectiles;
    QList<Explosion> m_explosions;
    QList<EnemyMissile> m_enemyMissiles;

    QList<Enemy> m_enemies;

    QList<Item> m_items;                // 아이템 리스트
    QList<FloatingText> m_floatingTexts; // 플로팅 텍스트 리스트

    // 배경 이미지
    QPixmap m_backgroundImage;

    // 헬퍼 함수
    void setupCannons();
    void setupBuildings();
    void spawnEnemies();
    void checkCollisions();
    void cleanupObjects();
    void checkGameOver();
    Cannon* getCannonToFire(QPointF targetPos, int cooldown);

    // 아이템
    void spawnItems();
    void activateItemEffect(Item::Type type, QPointF pos); // 효과 발동
};

#endif // GAMEWIDGET_H

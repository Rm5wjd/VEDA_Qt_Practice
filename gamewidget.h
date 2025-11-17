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

private:
    LevelManager m_levelManager;

    // 게임 관리
    QTimer *m_gameTimer;
    QElapsedTimer m_gameClock;
    PlayerStats m_stats;
    QPointF m_mousePos;
    qint64 m_nextEnemySpawnTime = 0; // 다음 적 스폰 시간

    // 게임 객체 리스트
    Cannon m_cannons[3];
    Building m_buildings[6];
    QList<PlayerProjectile> m_projectiles;
    QList<Explosion> m_explosions;
    QList<EnemyMissile> m_enemyMissiles;

    QList<Enemy> m_enemies;

    // 배경 이미지
    QPixmap m_backgroundImage;

    // 헬퍼 함수
    void setupCannons();
    void setupBuildings();
    void spawnEnemies();
    void checkCollisions();
    void cleanupObjects();
    Cannon* getCannonToFire(QPointF targetPos);
};

#endif // GAMEWIDGET_H

#ifndef PLAYER_H
#define PLAYER_H

#include <QRectF>
#include <QPixmap>
#include <QVector2D>
#include <QPainter>
#include <QPen>
#include <QLinearGradient>

// (A) 유틸리티 / 데이터 클래스
struct PlayerStats
{
    int m_cannonCooldownMs = 500; // 0.5초 쿨타임
    float m_projectileSpeed = 8.0f;
    float m_explosionMaxRadius = 50.0f;
    float m_explosionGrowthRate = 1.5f;
};

// (B-2) 건물 객체 클래스 (추가)
struct Building
{
    QRectF m_rect;
    bool m_isDestroyed = false;

    // [추가] 건물 이미지
    QPixmap m_imgNormal;     // 정상 상태 이미지
    QPixmap m_imgDestroyed;  // 파괴 상태 이미지 (선택 사항: 나중에 구현)

    // 생성자
    Building() = default;
    Building(QRectF rect);

    void draw(QPainter* painter);
    void takeHit();
    QPointF getCenter() const { return m_rect.center(); }
};

struct PlayerProjectile
{
    QPointF m_startPos;
    QPointF m_currentPos;
    QPointF m_targetPos;
    QVector2D m_direction;
    float m_speed;
    bool m_isAlive = true;

    void update();
    bool hasReachedTarget();
    void draw(QPainter* painter);
};

class Player
{
public:
    Player();
};

#endif // PLAYER_H

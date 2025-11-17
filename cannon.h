#ifndef CANNON_H
#define CANNON_H

#include <QPointF>
#include <QRectF>
#include <QPixmap>
#include <QPainter>

// (B) 아군 객체 클래스
class Cannon
{
public:
    QPointF m_pos;
    QRectF m_hitbox;
    qint64 m_lastFireTime = -1000; // 마지막 발사 시간 (쿨타임 계산용)
    bool m_isDestroyed = false;

    // [추가] 이미지 저장용 변수
    QPixmap m_imgBody;
    QPixmap m_imgBottom;

    // [추가] 부활 타이머 (밀리초 단위)
    int m_respawnTimer = 0;
    static const int RESPAWN_TIME_MS = 5000; // 5초 후 부활

    Cannon() = default;
    Cannon(QPointF pos, QSizeF size);

    void draw(QPainter* painter, QPointF mousePos);
    bool canFire(qint64 currentTime, int cooldown);
    void fire(qint64 currentTime);
    void takeHit();

    // [추가] 매 프레임 호출될 업데이트 함수
    void update(int deltaTimeMs);
};

#endif // CANNON_H

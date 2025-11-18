#ifndef ITEM_H
#define ITEM_H

#include <QPointF>
#include <QVector2D>
#include <QPainter>
#include <QRectF>
#include <QPixmap> // <--- 필수

class Item
{
public:
    enum Type {
        BigBoom,    // 폭발 범위 증가 (/item/range)
        RapidFire   // 쿨타임 삭제 (/item/cooldown)
    };

    Item(Type type, int gameWidth);

    void update();
    void draw(QPainter* painter);
    QRectF getHitbox() const;

    bool isAlive() const { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    Type getType() const { return m_type; }
    QPointF getPos() const { return m_pos; }

    // [추가] 아이템 이미지 (모든 아이템 객체가 공유)
    static QPixmap* s_imgRange;
    static QPixmap* s_imgCooldown;

private:
    Type m_type;
    QPointF m_pos;
    QVector2D m_velocity;
    bool m_isAlive = true;
    float m_size = 40.0f; // 이미지 크기에 맞춰 적절히 조절 (예: 40px)
};

#endif // ITEM_H

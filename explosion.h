#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <QPointF>
#include <QPainter>

class Explosion
{
public:
    // 생성자에서 '최대 반경'과 '성장 속도'를 인자로 받습니다.
    // -> 아이템 먹어서 스탯이 바뀌면, 생성할 때 바뀐 값을 넣어주면 됩니다.
    Explosion(QPointF center, float maxRadius, float growthRate);
    Explosion() = delete;

    void update();
    void draw(QPainter* painter);

    // Getter (충돌 체크용)
    bool isAlive() const { return m_isAlive; }
    QPointF center() const { return m_center; }
    float currentRadius() const { return m_currentRadius; }
    float maxRadius() const { return m_maxRadius; }

private:
    QPointF m_center;
    float m_currentRadius = 0.0f;
    float m_maxRadius;
    float m_growthRate;
    bool m_isAlive = true;
};

#endif // EXPLOSION_H

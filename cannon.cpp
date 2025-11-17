#include "cannon.h"

// =========================================================
// (B) Cannon 클래스 구현
// =========================================================

Cannon::Cannon(QPointF pos, QSizeF size) : m_pos(pos)
{
    // 히트박스 설정
    // pos는 대포의 '바닥 중앙' 좌표입니다.
    // 히트박스는 pos에서 위쪽으로 size만큼 올라간 영역입니다.
    m_hitbox = QRectF(pos.x() - size.width() / 2, pos.y() - size.height(), size.width(), size.height());

    // 리소스에서 이미지 로드
    // (경로는 assets.qrc 설정에 따름)
    m_imgBody.load(":/player/cannon_body.png");
    m_imgBottom.load(":/player/cannon_bottom.png");
}

void Cannon::draw(QPainter* painter, QPointF mousePos)
{
    // 1. 파괴 상태 오버레이 색상 결정
    QColor overlayColor = Qt::transparent;

    if (m_isDestroyed) {
        overlayColor = QColor(255, 0, 0, 180); // 기본: 진한 반투명 빨강

        // 부활 1초 전(1000ms 이하) 깜빡임 효과
        if (m_respawnTimer <= 1000 && m_respawnTimer > 0) {
            // 0.1초 간격으로 회색(원래 색 비슷한)과 빨강 교차
            if ((m_respawnTimer / 100) % 2 == 0) {
                overlayColor = QColor(100, 100, 100, 150);
            }
        }
    }

    float w = m_hitbox.width();
    float h = m_hitbox.height();
    QPointF center = m_hitbox.center();

    // -------------------------------------------------
    // [Layer 1] 바닥 (Bottom) - 고정
    // -------------------------------------------------
    if (!m_imgBottom.isNull()) {
        painter->drawPixmap(m_hitbox.toRect(), m_imgBottom);
    } else {
        // 이미지 없을 시 백업 도형
        painter->fillRect(m_hitbox, Qt::darkGray);
    }

    // -------------------------------------------------
    // [Layer 2] 몸체 (Body) - 마우스 방향 회전
    // -------------------------------------------------
    painter->save(); // 좌표계 저장

    painter->translate(center); // 1. 중심으로 이동

    // 2. 각도 계산
    QLineF line(center, mousePos);
    qreal angle = line.angle();

    // 3. 회전 적용
    // 이미지가 '왼쪽(180도)'을 보고 있다고 가정 -> +180도 보정
    painter->rotate(-angle + 180);

    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    if (!m_imgBody.isNull()) {
        // [위치 보정] 포신을 받침대 위로 살짝 올리기 위해 yOffset 적용
        // (0.3f는 상황에 맞춰 조절 가능)
        float xOffset = h * 1.0f;
        painter->drawPixmap(-w/2 - xOffset, -h/2, w, h, m_imgBody);
    } else {
        // 백업 도형
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::NoPen);
        painter->drawRect(-w/2, -h/2, w, h);
    }

    painter->restore(); // 좌표계 복구

    // -------------------------------------------------
    // [Layer 3] 파괴 상태 오버레이
    // -------------------------------------------------
    if (overlayColor != Qt::transparent) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(overlayColor);
        painter->drawRect(m_hitbox);
    }
}

bool Cannon::canFire(qint64 currentTime, int cooldown)
{
    // 파괴되지 않았고, 쿨타임이 지났으면 true
    return !m_isDestroyed && (currentTime - m_lastFireTime > cooldown);
}

void Cannon::fire(qint64 currentTime)
{
    m_lastFireTime = currentTime;
}

void Cannon::takeHit()
{
    if (m_isDestroyed) return; // 이미 터졌으면 무시

    m_isDestroyed = true;
    m_respawnTimer = RESPAWN_TIME_MS; // 부활 타이머 시작 (5초)
}

void Cannon::update(int deltaTimeMs)
{
    if (m_isDestroyed)
    {
        m_respawnTimer -= deltaTimeMs;

        if (m_respawnTimer <= 0) {
            // 부활!
            m_isDestroyed = false;
            m_respawnTimer = 0;
        }
    }
}

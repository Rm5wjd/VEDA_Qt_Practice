#include "player.h"

// --- (B) PlayerProjectile 클래스 구현 ---
void PlayerProjectile::update()
{
    if (!m_isAlive) return;
    m_currentPos += m_direction.toPointF() * m_speed;
    if (hasReachedTarget()) {
        m_isAlive = false;
    }
}

bool PlayerProjectile::hasReachedTarget()
{
    // 목표지점을 지났는지 내적으로 판별
    return QVector2D::dotProduct(QVector2D(m_targetPos - m_currentPos), m_direction) < 0;
}

void PlayerProjectile::draw(QPainter* painter)
{
    if (!m_isAlive) return;

    static QPixmap imgRocket;
    if (imgRocket.isNull()) {
        imgRocket.load(":/player/player_rocket.png");
        // (참고: 이미지 로드 실패 시 디버깅을 위해 qWarning() 추가 가능)
        // if (imgRocket.isNull()) qWarning() << "Failed to load player_rocket.png";
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    // --- 1. 네온 궤적 그리기 (기존 코드 유지) ---
    QLinearGradient trailGradient(m_startPos, m_currentPos);
    trailGradient.setColorAt(0.0, Qt::transparent);
    trailGradient.setColorAt(0.3, QColor(0, 255, 255, 50));
    trailGradient.setColorAt(1.0, QColor(0, 255, 255, 255));

    QPen glowPen;
    glowPen.setBrush(trailGradient);
    glowPen.setWidth(6);
    glowPen.setCapStyle(Qt::RoundCap);
    painter->setPen(glowPen);
    painter->drawLine(m_startPos, m_currentPos);

    QLinearGradient coreGradient(m_startPos, m_currentPos);
    coreGradient.setColorAt(0.0, Qt::transparent);
    coreGradient.setColorAt(1.0, QColor(200, 255, 255));

    QPen corePen;
    corePen.setBrush(coreGradient);
    corePen.setWidth(2);
    corePen.setCapStyle(Qt::RoundCap);
    painter->setPen(corePen);
    painter->drawLine(m_startPos, m_currentPos);


    // --- 2. 로켓 이미지 그리기 (회전 적용) ---
    if (!imgRocket.isNull())
    {
        painter->save();

        painter->translate(m_currentPos);

        QLineF vectorLine(0, 0, m_direction.x(), m_direction.y());
        qreal angle = vectorLine.angle();

        // [수정] 이미지가 '왼쪽(180도)'을 보고 있다고 가정 -> +180도 보정
        painter->rotate(-angle + 180);

        float size = 24.0f;
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(-size/2, -size/2, size, size, imgRocket);

        painter->restore();
    }
}

// --- (B-2) Building 구현 ---

Building::Building(QRectF rect) : m_rect(rect)
{
    // [추가] 리소스에서 이미지 로드
    // assets.qrc에 "/player/building.png"로 등록되어 있다고 가정합니다.
    m_imgNormal.load(":/player/building2.png");

    // 파괴된 건물 이미지는 일단 동일한 이미지를 사용하거나,
    // 검은색/투명한 이미지로 대체할 수 있습니다.
    // 여기서는 간단히 정상 이미지를 크기만 줄여서 사용하도록 처리하거나,
    // 아예 없는 경우를 대비해 isNull() 체크로 도형을 그리게 합니다.
}

void Building::draw(QPainter* painter)
{
    if (m_isDestroyed) {
        // [수정] 파괴된 건물 처리
        // 1. 반투명한 검은색 오버레이로 파괴된 느낌 주기
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 180)); // 반투명 검정
        painter->drawRect(m_rect);

        // 2. 파괴된 이미지(m_imgDestroyed)가 있다면 그리기
        // if (!m_imgDestroyed.isNull()) {
        //     painter->drawPixmap(m_rect.toRect(), m_imgDestroyed);
        // } else { ... } (현재는 m_imgDestroyed가 없음)
    } else {
        // [수정] 정상 건물 이미지 그리기
        if (!m_imgNormal.isNull()) {
            painter->drawPixmap(m_rect.toRect(), m_imgNormal);
        } else {
            // 이미지 로드 실패 시 백업용: 기존의 파란색 사각형
            painter->setBrush(Qt::blue);
            painter->drawRect(m_rect);
        }
    }
}

void Building::takeHit() {
    m_isDestroyed = true;
}

Player::Player() {}

#include "explosion.h"
#include <QRandomGenerator>
#include <QPen>

Explosion::Explosion(QPointF center, float maxRadius, float growthRate)
    : m_center(center), m_currentRadius(0.0f),
    m_maxRadius(maxRadius), m_growthRate(growthRate),
    m_isAlive(true)
{
}

void Explosion::update()
{
    if (!m_isAlive) return;

    m_currentRadius += m_growthRate;

    if (m_currentRadius >= m_maxRadius) {
        m_isAlive = false;
    }
}

void Explosion::draw(QPainter* painter)
{
    if (!m_isAlive) return;

    painter->setRenderHint(QPainter::Antialiasing, true);

    float progress = m_currentRadius / m_maxRadius;

    // 1. 색상 설정 (밝은 청록색 네온)
    QColor baseColor(0, 191, 255);

    // 기본 투명도 (서서히 사라짐)
    //int baseAlpha = static_cast<int>(255 * (1.0 - progress));
    int baseAlpha = 200;
    if (baseAlpha < 0) baseAlpha = 0;

    // 2. 링 갯수 (여러 겹으로 그려서 잔상/네온 효과)
    int numRings = 5;

    for (int i = 0; i < numRings; ++i)
    {
        // 3. 흔들림 강도 설정 (반지름의 15% 정도)
        float shakePower = m_currentRadius * 0.25f;

        // 랜덤 오프셋 생성 (X, Y축 흔들림)
        float offsetX = (QRandomGenerator::global()->generateDouble() - 0.5) * shakePower;
        float offsetY = (QRandomGenerator::global()->generateDouble() - 0.5) * shakePower;

        // [핵심] 반지름 자체도 미세하게 떨림 (Jitter)
        float radiusJitter = (QRandomGenerator::global()->generateDouble() - 0.5) * shakePower * 0.5;

        // 링 간격 (안쪽 링, 바깥쪽 링 구분)
        float ringGap = i * 2.0f;

        float finalRadius = m_currentRadius + radiusJitter - ringGap;
        if (finalRadius <= 0) continue;

        // 펜 설정
        QPen outlinePen;
        outlinePen.setWidthF(5); // 얇은 선
        // 겹치면 더 밝아 보이게 알파값 조절
        outlinePen.setColor(QColor(baseColor.red(), baseColor.green(), baseColor.blue(), baseAlpha / numRings * 2));
        outlinePen.setCapStyle(Qt::RoundCap);

        painter->setPen(outlinePen);
        painter->setBrush(Qt::NoBrush);

        // 흔들린 중심점 + 흔들린 반지름으로 그리기
        painter->drawEllipse(m_center + QPointF(offsetX, offsetY), finalRadius, finalRadius);
    }

    painter->setRenderHint(QPainter::Antialiasing, false);
}

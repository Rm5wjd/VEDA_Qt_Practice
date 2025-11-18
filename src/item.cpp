#include "item.h"
#include <QRandomGenerator>
#include <QDebug>

// [필수] Static 변수 초기화
QPixmap* Item::s_imgRange = nullptr;
QPixmap* Item::s_imgCooldown = nullptr;

Item::Item(Type type, int gameWidth) : m_type(type)
{
    // 1. 이미지 최초 로드 (한 번만 실행됨)
    // (확장자가 .png라고 가정했습니다. 다르면 수정해주세요)
    if (s_imgRange == nullptr) {
        s_imgRange = new QPixmap(":/item/range.png");
        if (s_imgRange->isNull()) qWarning() << "Failed to load item/range.png";
    }
    if (s_imgCooldown == nullptr) {
        s_imgCooldown = new QPixmap(":/item/cooldown.png");
        if (s_imgCooldown->isNull()) qWarning() << "Failed to load item/cooldown.png";
    }

    // 2. 스폰 로직 (기존과 동일)
    // 좌/우 랜덤 스폰 (높이는 상단 100~300 사이)
    bool fromLeft = (QRandomGenerator::global()->generateDouble() < 0.5);
    float startY = 400 + (QRandomGenerator::global()->generate() % 200);

    m_pos = QPointF(fromLeft ? -50 : gameWidth + 50, startY);
    m_velocity = QVector2D(fromLeft ? 2.0f : -2.0f, 0); // 속도 4.0
}

void Item::update()
{
    if (!m_isAlive) return;
    m_pos += m_velocity.toPointF();

    // 화면 밖으로 나가면 삭제 (넉넉하게)
    if (m_pos.x() < -100 || m_pos.x() > 3000) {
        m_isAlive = false;
    }
}

void Item::draw(QPainter* painter)
{
    if (!m_isAlive) return;

    painter->save();
    // 이미지 렌더링 품질 향상
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::Antialiasing);

    QPixmap* currentImg = nullptr;
    if (m_type == BigBoom) currentImg = s_imgRange;
    else if (m_type == RapidFire) currentImg = s_imgCooldown;

    if (currentImg && !currentImg->isNull()) {
        // 중심점 기준으로 이미지 그리기
        painter->drawPixmap(
            m_pos.x() - m_size / 2,
            m_pos.y() - m_size / 2,
            m_size,
            m_size,
            *currentImg
            );
    } else {
        // [백업] 이미지 로드 실패 시 기존 네온 박스 그리기
        QColor color = (m_type == BigBoom) ? Qt::yellow : Qt::cyan;
        painter->setPen(QPen(color, 3));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(QRectF(m_pos.x() - m_size/2, m_pos.y() - m_size/2, m_size, m_size));
        painter->setPen(Qt::white);
        painter->drawText(QRectF(m_pos.x() - m_size/2, m_pos.y() - m_size/2, m_size, m_size),
                          Qt::AlignCenter, (m_type == BigBoom ? "RNG" : "CD"));
    }

    painter->restore();
}

QRectF Item::getHitbox() const {
    return QRectF(m_pos.x() - m_size/2, m_pos.y() - m_size/2, m_size, m_size);
}

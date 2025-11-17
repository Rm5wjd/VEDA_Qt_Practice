#include "enemy.h"
#include <QRandomGenerator>

// --- (D) EnemyMissile 클래스 구현 ---
void EnemyMissile::update()
{
    if (!m_isAlive) return;
    m_currentPos += m_direction.toPointF() * m_speed;
    if (hasReachedTarget()) { // 목표 도달 (건물/대포 타격)
        m_isAlive = false;
        // (추후 구현: 타겟(건물/대포)에 데미지 주기)
    }
}

bool EnemyMissile::hasReachedTarget()
{
    return QVector2D::dotProduct(QVector2D(m_targetPos - m_currentPos), m_direction) < 0;
}

void EnemyMissile::draw(QPainter* painter)
{
    if (!m_isAlive) return;

    static QPixmap imgEnemy;
    if (imgEnemy.isNull()) {
        imgEnemy.load(":/enemy/enemy_rocket.png");
        // if (imgEnemy.isNull()) qWarning() << "Failed to load enemy_rocket.png";
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    // --- 1. 네온 궤적 그리기 (기존 코드 유지 - 빨간색) ---
    QLinearGradient trailGradient(m_startPos, m_currentPos);
    trailGradient.setColorAt(0.0, Qt::transparent);
    trailGradient.setColorAt(0.3, QColor(255, 0, 0, 50));
    trailGradient.setColorAt(1.0, QColor(255, 0, 0, 255));

    QPen glowPen;
    glowPen.setBrush(trailGradient);
    glowPen.setWidth(6);
    glowPen.setCapStyle(Qt::RoundCap);
    painter->setPen(glowPen);
    painter->drawLine(m_startPos, m_currentPos);

    QLinearGradient coreGradient(m_startPos, m_currentPos);
    coreGradient.setColorAt(0.0, Qt::transparent);
    coreGradient.setColorAt(1.0, QColor(255, 200, 200));

    QPen corePen;
    corePen.setBrush(coreGradient);
    corePen.setWidth(2);
    corePen.setCapStyle(Qt::RoundCap);
    painter->setPen(corePen);
    painter->drawLine(m_startPos, m_currentPos);


    // --- 2. 적 미사일 이미지 그리기 (회전 적용) ---
    if (!imgEnemy.isNull())
    {
        painter->save();

        painter->translate(m_currentPos);

        QLineF vectorLine(0, 0, m_direction.x(), m_direction.y());
        qreal angle = vectorLine.angle();

        // [수정] 이미지가 '왼쪽(180도)'을 보고 있다고 가정 -> +180도 보정
        painter->rotate(-angle + 180);

        float size = 24.0f; // 적 미사일 크기
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(-size/2, -size/2, size, size, imgEnemy);

        painter->restore();
    }
}

// [수정] 포인터 초기화 (nullptr로 설정하여 main 전에는 아무것도 안 하도록 함)
QPixmap* Enemy::s_imgType1 = nullptr;
QPixmap* Enemy::s_imgType2 = nullptr;
QPixmap* Enemy::s_imgType3 = nullptr;

Enemy::Enemy(Type type, int gameWidth, int gameHeight) : m_type(type)
{
    // [수정] 최초 1회 할당 (포인터가 비어있으면 new로 생성)
    // 이제 이 코드는 main() 이후 게임이 실행될 때 호출되므로 안전합니다.
    if (s_imgType1 == nullptr) {
        s_imgType1 = new QPixmap(":/enemy/type1.png");
        if (s_imgType1->isNull()) qWarning() << "Failed to load type1.png";
    }
    if (s_imgType2 == nullptr) {
        s_imgType2 = new QPixmap(":/enemy/type2.png");
        if (s_imgType2->isNull()) qWarning() << "Failed to load type2.png";
    }
    if (s_imgType3 == nullptr) {
        s_imgType3 = new QPixmap(":/enemy/type3.png");
        if (s_imgType3->isNull()) qWarning() << "Failed to load type3.png";
    }

    // 타입별 초기화 로직 (변수 사용 시 -> 사용)
    if (m_type == BOMBER_V) {
        bool fromLeft = (QRandomGenerator::global()->generateDouble() < 0.5);
        m_pos = QPointF(fromLeft ? -50 : gameWidth + 50, 60);
        m_velocity = QVector2D(fromLeft ? 3.0f : -3.0f, 0);
        m_fireInterval = 1500;
        m_hp = 1;

        if (s_imgType1 && !s_imgType1->isNull()) {
            m_width = s_imgType1->width();   // . 대신 -> 사용
            m_height = s_imgType1->height(); // . 대신 -> 사용
        }
    }
    else if (m_type == BOSS_3WAY) {
        m_pos = QPointF(gameWidth / 2, 80);
        m_velocity = QVector2D(0, 0);
        m_fireInterval = 1000;
        m_hp = 10;

        if (s_imgType2 && !s_imgType2->isNull()) {
            m_width = s_imgType2->width();
            m_height = s_imgType2->height();
        }
    }
    else if (m_type == BOMBER_BURST) {
        bool fromLeft = (QRandomGenerator::global()->generateDouble() < 0.5);
        m_pos = QPointF(fromLeft ? -50 : gameWidth + 50, 100);
        m_velocity = QVector2D(fromLeft ? 4.0f : -4.0f, 0);
        m_hasFired = false;
        m_hp = 2;

        if (s_imgType3 && !s_imgType3->isNull()) {
            m_width = s_imgType3->width();
            m_height = s_imgType3->height();
        }
    }
}

QList<EnemyMissile> Enemy::update(int deltaTimeMs)
{
    // ... (기존 코드와 동일, 변경 없음) ...
    // update 함수는 이미지 변수를 직접 안 쓰므로 그대로 두셔도 됩니다.
    QList<EnemyMissile> newMissiles;
    if (!m_isAlive) return newMissiles;

    m_pos += m_velocity.toPointF();
    m_fireTimer += deltaTimeMs;

    // --- [타입 1] 수직 폭격기 ---
    if (m_type == BOMBER_V) {
        if (m_fireTimer >= m_fireInterval) {
            m_fireTimer = 0;
            EnemyMissile m;
            m.m_startPos = m_pos; m.m_currentPos = m_pos;
            m.m_targetPos = m_pos + QPointF(0, 1000);
            m.m_direction = QVector2D(0, 1);
            m.m_speed = 3.0f;
            m.m_isAlive = true;
            newMissiles.append(m);
        }
    }
    // --- [타입 2] 중앙 보스 (3갈래) ---
    else if (m_type == BOSS_3WAY) {
        if (m_fireTimer >= m_fireInterval) {
            m_fireTimer = 0;
            float angles[] = { -15.0f, 0.0f, 15.0f };
            for (float deg : angles) {
                EnemyMissile m;
                m.m_startPos = m_pos; m.m_currentPos = m_pos;
                QLineF vecLine(0, 0, 0, 1);
                vecLine.setAngle(270 + deg);
                QPointF dirPoint = vecLine.p2();
                m.m_direction = QVector2D(dirPoint).normalized();
                m.m_targetPos = m_pos + (dirPoint * 1000);
                m.m_speed = 6.0f;
                m.m_isAlive = true;
                newMissiles.append(m);
            }
        }
    }
    // --- [타입 3] 5갈래 폭격기 (1회성) ---
    else if (m_type == BOMBER_BURST) {
        if (!m_hasFired && m_pos.x() > 200 && m_pos.x() < 1080) {
            m_hasFired = true;
            float angles[] = { -10.0f, -5.0f, 0.0f, 5.0f, 10.0f };
            for (float deg : angles) {
                EnemyMissile m;
                m.m_startPos = m_pos; m.m_currentPos = m_pos;
                QLineF vecLine(0, 0, 0, 1);
                vecLine.setAngle(270 + deg);
                QPointF dirPoint = vecLine.p2();
                m.m_direction = QVector2D(dirPoint).normalized();
                m.m_targetPos = m_pos + (dirPoint * 1000);
                m.m_speed = 4.0f;
                m.m_isAlive = true;
                newMissiles.append(m);
            }
        }
    }

    if (m_pos.x() < -200 || m_pos.x() > 1500) {
        m_isAlive = false;
    }

    return newMissiles;
}

void Enemy::draw(QPainter* painter)
{
    if (!m_isAlive) return;

    painter->save();
    painter->translate(m_pos);

    // [수정] 포인터 사용
    QPixmap* currentImg = nullptr;
    if (m_type == BOMBER_V) currentImg = s_imgType1;
    else if (m_type == BOSS_3WAY) currentImg = s_imgType2;
    else if (m_type == BOMBER_BURST) currentImg = s_imgType3;

    // [수정] 포인터 체크 및 역참조(*) 사용
    if (currentImg && !currentImg->isNull()) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        if (m_velocity.x() != 0 || m_velocity.y() != 0) {
            QLineF velocityLine(0, 0, m_velocity.x(), m_velocity.y());
            qreal angle = velocityLine.angle();
            painter->rotate(-angle + 180);
        }

        // *currentImg 로 역참조하여 그리기
        painter->drawPixmap(-m_width/2, -m_height/2, m_width, m_height, *currentImg);
    } else {
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawRect(-m_width/2, -m_height/2, m_width, m_height);
    }

    painter->restore();
}

QRectF Enemy::getHitbox() const {
    return QRectF(m_pos.x() - m_width/2, m_pos.y() - m_height/2, m_width, m_height);
}

void Enemy::takeHit() {
    m_hp--;
    if (m_hp <= 0) m_isAlive = false;
}

#ifndef ENEMY_H
#define ENEMY_H

#include <QPointF>
#include <QVector2D>
#include <QPainter>
#include <QDebug>

// (D) 적군 객체 클래스
struct EnemyMissile
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
    QPointF getHitboxCenter() const { return m_currentPos; } // 단순화를 위해 점으로 판정
};

class Enemy
{
public:
    // 적 타입 정의: 수직 폭격기, 3갈래 보스, 5갈래 버스트
    enum Type { BOMBER_V, BOSS_3WAY, BOMBER_BURST };

    Type m_type;
    QPointF m_pos;       // 현재 위치
    QVector2D m_velocity;// 이동 속도 및 방향

    float m_width = 40.0f;
    float m_height = 30.0f;

    // 발사 관련 변수
    int m_fireTimer = 0;
    int m_fireInterval = 0;
    bool m_hasFired = false; // 일회성 발사 체크용

    // 상태 변수
    bool m_isAlive = true;
    int m_hp = 1;

    // [리소스] 타입별 이미지 (모든 Enemy 객체가 공유)
    static QPixmap *s_imgType1;
    static QPixmap *s_imgType2;
    static QPixmap *s_imgType3;

    // 생성자: 게임 화면 크기를 받아 초기 위치 설정에 사용
    Enemy(Type type, int gameWidth, int gameHeight);
    Enemy() = delete;

    // 업데이트: 로직 처리 후, 새로 발사된 미사일 리스트를 반환
    QList<EnemyMissile> update(int deltaTimeMs);

    // 그리기
    void draw(QPainter* painter);

    // 충돌 판정 영역
    QRectF getHitbox() const;

    // 피격 처리
    void takeHit();
};

#endif // ENEMY_H

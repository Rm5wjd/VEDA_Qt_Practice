#include "gamewidget.h"
#include <cstdlib> // for rand()
#include <ctime>   // for time()
#include <QDebug>
#include <cfloat>

// --- 게임 상수 ---
const int GAME_FPS = 60;
const int ENEMY_SPAWN_INTERVAL_MS = 2000; // 2초마다 적 스폰
const float ENEMY_MISSILE_SPEED = 2.0f;

// --- (E) GameWidget 클래스 구현 ---

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(800, 600);
    setMouseTracking(true);
    setCursor(Qt::BlankCursor);

    // 난수 시드 초기화
    srand(static_cast<unsigned int>(time(nullptr)));

    setupCannons();
    setupBuildings();

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameWidget::updateGameLogic);
    m_gameTimer->start(1000 / GAME_FPS);

    m_gameClock.start();
    m_nextEnemySpawnTime = m_gameClock.elapsed(); // 즉시 스폰

    // assets.qrc에 "/player/background.png"로 등록되어 있다고 가정합니다.
    m_backgroundImage.load(":/player/background.png");

    if (m_backgroundImage.isNull()) {
        qWarning() << "Failed to load background image: :/player/background.png";
    }

    if (!m_levelManager.loadLevelsFromFile(":/level.json")) {
        qDebug() << "Failed to load levels! Using default.";
    }
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    setupCannons(); // 창 크기가 바뀔 때 (고정크기라 최초 1회만)
    setupBuildings(); // <-- 리사이즈 시 재배치
    QWidget::resizeEvent(event);
}

void GameWidget::setupCannons()
{
    int w = width();
    int h = height();
    QSizeF cannonSize(60, 20);
    // 대포 위치 (하단)
    m_cannons[0] = Cannon(QPointF(w * 0.15, h), cannonSize);
    m_cannons[1] = Cannon(QPointF(w * 0.50, h), cannonSize);
    m_cannons[2] = Cannon(QPointF(w * 0.85, h), cannonSize);
}

void GameWidget::setupBuildings()
{
    int w = width();
    int h = height();

    float buildingW = 40;
    float buildingH = 60;
    float baseY = h - buildingH; // 바닥에 딱 붙게

    // 배치 로직:
    // [대포0] - 빌딩0 - 빌딩1 - 빌딩2 - [대포1] - 빌딩3 - 빌딩4 - 빌딩5 - [대포2]

    // 간격 계산을 위한 구역 설정
    float zone1Start = m_cannons[0].m_hitbox.right(); // 왼쪽 대포 끝
    float zone1End = m_cannons[1].m_hitbox.left();    // 중앙 대포 시작
    float zone1Width = zone1End - zone1Start;
    float gap1 = (zone1Width - (buildingW * 3)) / 4; // 건물 사이 간격

    for (int i = 0; i < 3; ++i) {
        float x = zone1Start + gap1 + (i * (buildingW + gap1));
        m_buildings[i] = Building(QRectF(x, baseY, buildingW, buildingH));
    }

    float zone2Start = m_cannons[1].m_hitbox.right(); // 중앙 대포 끝
    float zone2End = m_cannons[2].m_hitbox.left();    // 오른쪽 대포 시작
    float zone2Width = zone2End - zone2Start;
    float gap2 = (zone2Width - (buildingW * 3)) / 4;

    for (int i = 0; i < 3; ++i) {
        float x = zone2Start + gap2 + (i * (buildingW + gap2));
        m_buildings[i + 3] = Building(QRectF(x, baseY, buildingW, buildingH));
    }
}

// (핵심) 메인 게임 루프
void GameWidget::updateGameLogic()
{
    // 1. 스폰
    spawnEnemies();

    // [추가] 1-1. 대포 업데이트 (부활 타이머 체크)
    // 타이머 간격(16ms)을 넘겨줌
    int dt = 1000 / GAME_FPS;
    for (Cannon &c : m_cannons) {
        c.update(dt);
    }

    // 2. 객체 업데이트
    for (PlayerProjectile &p : m_projectiles) {
        p.update();
        if (!p.m_isAlive) {
            // 포탄이 목표에 도달 -> 폭발 생성
            m_explosions.append(Explosion(
                p.m_targetPos,
                m_stats.m_explosionMaxRadius,
                m_stats.m_explosionGrowthRate
                ));
        }
    }
    for (Explosion &e : m_explosions) {
        e.update();
    }
    for (EnemyMissile &em : m_enemyMissiles) {
        em.update();
    }

    // 3. 충돌 검사
    checkCollisions();

    // 4. 죽은 객체 제거
    cleanupObjects();

    // 5. 화면 갱신 요청
    this->update();
}

/**
void GameWidget::spawnEnemies()
{
    // 1. 현재 웨이브의 물량을 다 쏟아냈는지 확인
    if (m_levelManager.isWaveFinished())
    {
        // 물량은 다 나왔는데, 화면에 아직 적이 남아있다면? -> 다 죽일 때까지 대기
        if (!m_enemyMissiles.isEmpty()) {
            return;
        }

        // 화면도 깨끗해졌다면? -> 다음 웨이브 시작 (레벨 업!)
        m_levelManager.nextWave();

        // (선택 사항) 여기서 "Level 2 Start!" 같은 텍스트를 띄우는 함수 호출 가능
        qDebug() << "Next Wave Started! Level:" << m_levelManager.getCurrentLevelIndex();
        return;
    }

    // 2. 스폰 타이밍 체크
    qint64 now = m_gameClock.elapsed();
    if (now < m_nextEnemySpawnTime) {
        return; // 아직 때가 아님
    }

    // 3. 레벨 매니저에서 현재 웨이브 데이터 가져오기
    const WaveData& currentWave = m_levelManager.getCurrentWave();

    // 다음 스폰 시간 설정 (초 단위이므로 * 1000)
    // 약간의 랜덤성을 주어 기계적인 느낌 제거 (0.8배 ~ 1.2배)
    float randomFactor = 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
    m_nextEnemySpawnTime = now + (currentWave.spawnInterval * 1000 * randomFactor);


    // 4. 적 객체 생성 부분 수정
    EnemyMissile em;
    em.m_startPos = QPointF(rand() % width(), 0);
    em.m_currentPos = em.m_startPos;

    // [타겟 선정 로직 개선]
    // 살아있는 건물과 대포 리스트를 취합
    QVector<QPointF> validTargets;

    // 살아있는 건물 위치 추가
    for (const Building& b : m_buildings) {
        if (!b.m_isDestroyed) validTargets.append(b.getCenter());
    }
    // 살아있는 대포 위치 추가
    for (const Cannon& c : m_cannons) {
        if (!c.m_isDestroyed) validTargets.append(c.m_hitbox.center());
    }

    if (!validTargets.isEmpty()) {
        // 유효한 타겟 중 랜덤 선택
        int idx = rand() % validTargets.size();
        em.m_targetPos = validTargets[idx];
    } else {
        // 다 파괴됐으면 그냥 화면 바닥 아무데나
        em.m_targetPos = QPointF(rand() % width(), height());
    }

    em.m_direction = QVector2D(em.m_targetPos - em.m_startPos).normalized();
    em.m_speed = currentWave.missileSpeed;
    em.m_isAlive = true;

    m_enemyMissiles.append(em);
    m_levelManager.decreaseEnemyCount();
}
**/

void GameWidget::spawnEnemies()
{
    // 1. 웨이브 종료 체크
    // (미사일도 없고, 적 기체도 없어야 다음 웨이브로 진행)
    if (m_levelManager.isWaveFinished())
    {
        if (m_enemyMissiles.isEmpty() && m_enemies.isEmpty()) {
            m_levelManager.nextWave();
            // (선택) qDebug() << "Wave Started:" << m_levelManager.getCurrentLevelIndex();
        }
        return;
    }

    // 2. 스폰 타이밍 체크
    qint64 now = m_gameClock.elapsed();
    if (now < m_nextEnemySpawnTime) return;

    // 3. 레벨 데이터 가져오기
    const WaveData& currentWave = m_levelManager.getCurrentWave();

    // 다음 스폰 시간 설정
    float randomFactor = 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
    m_nextEnemySpawnTime = now + (currentWave.spawnInterval * 1000 * randomFactor);


    // =========================================================
    // 4. [핵심] 소환 로직 분기 (비행기 vs 미사일)
    // =========================================================

    bool spawnSpecial = false;

    // 특수 적(비행기) 물량이 남아있다면?
    if (m_levelManager.getRemainingSpecialEnemies() > 0) {
        // 20% 확률로 비행기 출격 (확률은 조절 가능)
        if (rand() % 100 < 20) {
            spawnSpecial = true;
        }
    }

    if (spawnSpecial)
    {
        // -------------------------------------------------
        // [A] 적 기체(Enemy) 소환
        // -------------------------------------------------
        int typeRoll = rand() % 3;
        Enemy::Type type = Enemy::BOMBER_V; // 기본

        if (typeRoll == 1) type = Enemy::BOSS_3WAY;
        else if (typeRoll == 2) type = Enemy::BOMBER_BURST;

        // [중요] m_enemies 리스트에 추가!
        m_enemies.append(Enemy(type, width(), height()));

        // 카운트 감소 (특수 적 & 전체 적 둘 다 감소)
        m_levelManager.decreaseSpecialEnemyCount();
        m_levelManager.decreaseEnemyCount();
    }
    else
    {
        // -------------------------------------------------
        // [B] 일반 미사일(EnemyMissile) 소환 (기존 코드)
        // -------------------------------------------------
        EnemyMissile em;
        em.m_startPos = QPointF(rand() % width(), 0);
        em.m_currentPos = em.m_startPos;

        // 타겟 선정 (건물/대포 중 랜덤)
        QVector<QPointF> validTargets;
        for (const Building& b : m_buildings) {
            if (!b.m_isDestroyed) validTargets.append(b.getCenter());
        }
        for (const Cannon& c : m_cannons) {
            if (!c.m_isDestroyed) validTargets.append(c.m_hitbox.center());
        }

        if (!validTargets.isEmpty()) {
            int idx = rand() % validTargets.size();
            em.m_targetPos = validTargets[idx];
        } else {
            em.m_targetPos = QPointF(rand() % width(), height());
        }

        em.m_direction = QVector2D(em.m_targetPos - em.m_startPos).normalized();
        em.m_speed = currentWave.missileSpeed;
        em.m_isAlive = true;

        m_enemyMissiles.append(em); // 미사일 리스트에 추가

        // 전체 적 카운트만 감소
        m_levelManager.decreaseEnemyCount();
    }
}


// (핵심) 충돌 로직
void GameWidget::checkCollisions()
{
    // 1. 아군 폭발 vs 적 미사일
    for (Explosion &exp : m_explosions)
    {
        if (!exp.isAlive()) continue;

        // 폭발 반경의 제곱 (sqrt 연산 방지)
        float radiusSq = exp.currentRadius() * exp.currentRadius();

        for (EnemyMissile &enemy : m_enemyMissiles)
        {
            if (!enemy.m_isAlive) continue;

            // 적 미사일 위치(점)와 폭발 중심(원) 사이의 거리 제곱
            float distSq = QVector2D(exp.center() - enemy.getHitboxCenter()).lengthSquared();

            if (distSq <= radiusSq)
            {
                // 충돌!
                enemy.m_isAlive = false;
                // (추후 구현: 점수 획득)
            }
        }
    }

    // 2. (추가) 적 미사일 vs 아군 건물/대포 (직접 충돌)
    for (EnemyMissile &enemy : m_enemyMissiles)
    {
        if (!enemy.m_isAlive) continue;

        // A. 건물 충돌 체크
        for (Building &b : m_buildings) {
            if (!b.m_isDestroyed && b.m_rect.contains(enemy.m_currentPos)) {
                b.takeHit();       // 건물 파괴
                enemy.m_isAlive = false; // 미사일 소멸 (자폭)

                // (선택) 여기서 작은 폭발 이펙트 생성 가능
                m_explosions.append(Explosion(enemy.m_currentPos, 20.0f, 2.0f));
                break;
            }
        }
        if (!enemy.m_isAlive) continue; // 건물에 박았으면 다음 검사 생략

        // B. 대포 충돌 체크 (기존에 구현 안 했다면 추가)
        for (Cannon &c : m_cannons) {
            if (!c.m_isDestroyed && c.m_hitbox.contains(enemy.m_currentPos)) {
                c.takeHit();
                enemy.m_isAlive = false;
                m_explosions.append(Explosion(enemy.m_currentPos, 30.0f, 2.0f));
                break;
            }
        }
    }
}

void GameWidget::cleanupObjects()
{
    // QList::removeIf 사용 (C++11 람다)
    m_projectiles.removeIf([](const PlayerProjectile &p){ return !p.m_isAlive; });
    m_explosions.removeIf([](const Explosion &e){ return !e.isAlive(); });
    m_enemyMissiles.removeIf([](const EnemyMissile &em){ return !em.m_isAlive; });
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    // 왼쪽 버튼 클릭만 처리
    if (event->button() != Qt::LeftButton) return;

    QPointF targetPos = event->position();

    // 1. 발사할 최적의 대포 찾기
    //    (거리, 생존 여부, 쿨타임까지 모두 고려해서 1개만 딱 골라옴)
    Cannon* cannonToFire = getCannonToFire(targetPos);

    // 쏠 수 있는 대포가 없으면 무시 (먹통)
    if (!cannonToFire) return;

    // 2. 발사! (내부 변수 갱신)
    qint64 now = m_gameClock.elapsed();
    cannonToFire->fire(now);

    // 3. 포탄 생성 로직
    PlayerProjectile p;
    // [중요] 발사 위치는 선택된 대포의 중심
    p.m_startPos = cannonToFire->m_hitbox.center() - QPointF(0, cannonToFire->m_hitbox.height()/2);
    p.m_currentPos = p.m_startPos;
    p.m_targetPos = targetPos;
    p.m_speed = m_stats.m_projectileSpeed;
    p.m_direction = QVector2D(p.m_targetPos - p.m_startPos).normalized();
    p.m_isAlive = true;

    m_projectiles.append(p);
}

Cannon* GameWidget::getCannonToFire(QPointF targetPos)
{
    Cannon* bestCannon = nullptr;
    float minDistSq = FLT_MAX; // 무한대 값으로 초기화
    qint64 now = m_gameClock.elapsed();

    // 대포 3개를 순회하며 가장 적합한 후보 탐색
    for (int i = 0; i < 3; ++i)
    {
        Cannon& c = m_cannons[i];

        // 1. 파괴된 대포는 후보에서 제외 (죽은 대포 영역 문제 해결)
        if (c.m_isDestroyed) continue;

        // 2. 쿨타임 중인 대포도 후보에서 제외 (더블 클릭 발사 문제 해결)
        //    (가까운 애가 쿨타임이면, 조금 멀어도 준비된 다른 애가 대신 쏨)
        if (!c.canFire(now, m_stats.m_cannonCooldownMs)) continue;

        // 3. 거리 계산 (거리의 제곱 비교)
        //    대포 히트박스 중심 ~ 마우스 클릭 위치 거리
        float distSq = QVector2D(c.m_hitbox.center() - targetPos).lengthSquared();

        // 4. 지금까지 찾은 것보다 더 가까우면 갱신
        if (distSq < minDistSq) {
            minDistSq = distSq;
            bestCannon = &c;
        }
    }

    // 모든 대포가 터졌거나, 살아있는 모든 대포가 쿨타임 중이면 nullptr 반환
    return bestCannon;
}

void GameWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->position();
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- 1. 배경 그리기 (가장 먼저) ---
    if (!m_backgroundImage.isNull()) {
        // 이미지를 위젯의 현재 크기에 맞춰 그립니다. (1280x720)
        painter.drawPixmap(0, 0, width(), height(), m_backgroundImage);
    } else {
        // 이미지 로드 실패 시, 대체 배경색 (어두운 보라색 등)
        painter.fillRect(rect(), QColor(20, 10, 30));
    }
    // 2. 대포 그리기
    for (Cannon &c : m_cannons) {
        c.draw(&painter, m_mousePos);
    }

    // 2-1. 건물 그리기 (추가)
    for (Building &b : m_buildings) {
        b.draw(&painter);
    }

    // 3. 적 미사일 그리기
    for (EnemyMissile &em : m_enemyMissiles) {
        em.draw(&painter);
    }

    // 4. 아군 포탄 그리기
    for (PlayerProjectile &p : m_projectiles) {
        p.draw(&painter);
    }

    // 5. 폭발 그리기
    for (Explosion &e : m_explosions) {
        e.draw(&painter);
    }

    // 6. 십자선 그리기
    painter.setPen(QPen(Qt::white, 1));
    painter.drawLine(m_mousePos.x() - 10, m_mousePos.y(), m_mousePos.x() + 10, m_mousePos.y());
    painter.drawLine(m_mousePos.x(), m_mousePos.y() - 10, m_mousePos.x(), m_mousePos.y() + 10);
}

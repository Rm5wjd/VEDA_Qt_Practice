#include "UI/rankingwidget.h"

// 필요한 Qt 클래스들
#include <QSqlQuery>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QSqlError>
#include <QFile> // QSS 파일 로드 시 사용
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QStyle>
#include <QVBoxLayout>
#include <QHBoxLayout>

RankingWidget::RankingWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    loadStyles();

    // 뒤로가기 버튼 연결 (부모 위젯에게 페이지 전환 요청)
    connect(backButton, &QPushButton::clicked, this, &RankingWidget::backClicked);

}

RankingWidget::~RankingWidget()
{
}

/**
 * @brief DB에서 최고 점수 순으로 데이터를 불러와 라벨에 표시합니다.
 * (현재는 화면 구성을 위해 기본적인 텍스트만 설정)
 */
void RankingWidget::loadRanks(const QString& currentUsername)
{
    // === 1. 하이라이트 초기화 ===
    // 랭킹을 새로 불러올 때마다 이전에 설정된 하이라이트를 모두 제거합니다.

    // 1위 프레임 초기화
    topCommanderFrame->setProperty("isCurrentUser", false);

    // 2~10위 라벨 초기화
    for (QLabel* label : rankNameLabels) {
        label->setProperty("isCurrentUser", false);
        // 빈 텍스트로 초기화 (DB에 10명 미만일 경우 대비)
        label->setText("");
    }
    topCommanderName->setText("-"); // 1위 이름 초기화

    // === 2. 실제 DB 연동 ===
    // (이전 MyPageWidget에서 사용한 'users_score' 테이블을 기반으로 쿼리 작성)
    QSqlQuery query;
    bool success = query.exec("SELECT username, high_score FROM users_score ORDER BY high_score DESC LIMIT 10");

    if (!success) {
        qWarning() << "Ranking Load Error:" << query.lastError().text();
        return;
    }

    int rankIndex = 0;
    while (query.next()) {
        QString username = query.value(0).toString();
        int score = query.value(1).toInt();
        QString displayText = QString("%1. %2 (%3점)").arg(rankIndex + 1).arg(username).arg(score);

        if (rankIndex == 0) {
            // === 1위 ===
            topCommanderName->setText(QString("%1 (%2점)").arg(username).arg(score));
            if (username == currentUsername) {
                topCommanderFrame->setProperty("isCurrentUser", true);
            }
        } else {
            // === 2위 ~ 10위 ===
            // rankNameLabels는 0~8 인덱스 (2위~10위)
            rankNameLabels[rankIndex - 1]->setText(displayText);
            if (username == currentUsername) {
                rankNameLabels[rankIndex - 1]->setProperty("isCurrentUser", true);
            }
        }
        rankIndex++;
    }

    // === 3. 스타일 적용 (매우 중요) ===
    // 동적 속성(isCurrentUser)을 QSS에 반영하려면 style을 갱신해야 합니다.

    // 1위 프레임 갱신
    style()->unpolish(topCommanderFrame);
    style()->polish(topCommanderFrame);

    // 2~10위 라벨 갱신
    for (QLabel* label : rankNameLabels) {
        style()->unpolish(label);
        style()->polish(label);
    }
}

/**
 * @brief UI를 생성하고 레이아웃을 설정합니다. (이미지 기반)
 */
void RankingWidget::initUI()
{
    // 1. 전체 레이아웃 (가로 분할)
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    // 2. 왼쪽 섹션 (타이틀, TOP COMMANDER)
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft); // 왼쪽 섹션 전체 정렬 유지

    // 타이틀
    rankingTitle = new QLabel("RANKING", this);
    rankingTitle->setObjectName("rankingTitle");
    rankingTitle->setAlignment(Qt::AlignCenter); // 타이틀 텍스트 정렬

    // TOP COMMANDER 텍스트 라벨
    topCommanderLabel = new QLabel("TOP COMMANDER", this);
    topCommanderLabel->setObjectName("topCommanderLabel");
    topCommanderLabel->setAlignment(Qt::AlignCenter); // 텍스트 중앙 정렬

    // 1위 이름 패널 (프레임)
    topCommanderFrame = new QFrame(this); // <--- "QFrame *" 제거
    topCommanderFrame->setObjectName("topCommanderFrame");

    // 1위 이름 라벨 (프레임 안에 생성)
    topCommanderName = new QLabel("-", topCommanderFrame); // <--- 여기서 한 번만 생성!
    topCommanderName->setObjectName("topCommanderName");
    topCommanderName->setAlignment(Qt::AlignCenter); // <--- 텍스트 중앙 정렬

    // QFrame의 레이아웃을 사용하여 이름을 중앙에 배치
    QVBoxLayout *frameLayout = new QVBoxLayout(topCommanderFrame);
    frameLayout->addWidget(topCommanderName);


    // --- 왼쪽 레이아웃 조립 (중복 제거 및 정렬 플래그 사용) ---
    leftLayout->addWidget(rankingTitle, 0, Qt::AlignHCenter); // 타이틀은 왼쪽에 붙임
    leftLayout->addSpacing(50);

    // TOP COMMANDER 텍스트를 수평 중앙에 정렬하여 추가
    leftLayout->addWidget(topCommanderLabel, 0, Qt::AlignHCenter);

    // 1위 이름 프레임을 수평 중앙에 정렬하여 추가
    leftLayout->addWidget(topCommanderFrame, 0, Qt::AlignHCenter);

    // 뒤로가기 버튼 추가
    backButton = new QPushButton("뒤로가기", this);
    backButton->setObjectName("backButton");
    leftLayout->addWidget(backButton);
    leftLayout->addStretch(1); // 아래쪽으로 밀기


    // 3. 오른쪽 섹션 (랭킹 목록)
    // ... (rankPanel 생성 코드는 동일) ...
    rankPanel = new QFrame(this);
    rankPanel->setObjectName("rankPanel");
    QVBoxLayout *rankListLayout = new QVBoxLayout(rankPanel);
    rankListLayout->setSpacing(5);
    rankListLayout->setAlignment(Qt::AlignTop);

    // 2위부터 10위까지의 라벨을 동적으로 생성하고 QVector에 저장
    for (int i = 2; i <= 10; ++i) {
        QLabel *rankRow = new QLabel(QString("%1.").arg(i), rankPanel);
        rankRow->setObjectName("rankRow");
        rankListLayout->addWidget(rankRow);
        rankNameLabels.append(rankRow); // 포인터를 벡터에 저장
    }
    rankListLayout->addStretch(1);


    // 4. 메인 레이아웃에 추가 (비율 조정)
    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addSpacing(50);
    mainLayout->addWidget(rankPanel, 4);
}

/**
 * @brief 스타일을 로드하고 적용합니다. (이미지 기반의 네온/픽셀 스타일 적용)
 */
void RankingWidget::loadStyles()
{
    // --- 1. 폰트 로드 ---
    QString fontFamily = "Arial";
    int fontId = QFontDatabase::addApplicationFont("resources/pixel_font.ttf");

    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        qWarning() << "RankingWidget: 픽셀 폰트 로드 실패! Arial 사용.";
    }

    // --- 2. 폰트 및 네온 효과 적용 ---
    if (fontId != -1) {
        // RANKING 타이틀
        QFont titleFont(fontFamily, 60);
        rankingTitle->setFont(titleFont);
        QGraphicsDropShadowEffect *neonEffect = new QGraphicsDropShadowEffect();
        neonEffect->setColor(QColor(0, 255, 255, 200));
        neonEffect->setBlurRadius(30);
        neonEffect->setOffset(0, 0);
        rankingTitle->setGraphicsEffect(neonEffect);

        // TOP COMMANDER
        QFont commanderFont(fontFamily, 24);
        topCommanderLabel->setFont(commanderFont);

        // 1위 이름
        QFont topNameFont(fontFamily, 20);
        topCommanderName->setFont(topNameFont);

        // 랭킹 목록 라벨
        QFont rankRowFont(fontFamily, 18);
        for(QLabel* label : rankNameLabels) {
            label->setFont(rankRowFont);
        }
    }

    // --- 3. QSS 스타일시트 ---
    // 이미지의 네온 컬러와 픽셀 배경에 맞게 스타일 정의
    QString qssTemplate = QStringLiteral(
        "RankingWidget { background-color: transparent; }"

        // 메인 타이틀
        "#rankingTitle { color: #00FFFF; font-family: '%1'; }" // 폰트 크기는 코드에서 설정

        // TOP COMMANDER 텍스트
        "#topCommanderLabel { color: white; font-family: '%1'; }"

        // 1위 이름이 들어갈 프레임 (이미지의 파란색 테두리 사각형)
        "#topCommanderFrame {"
        "    background-color: rgba(10, 30, 80, 150);"
        "    border: 2px solid #00FFFF;" // 네온 블루 테두리
        "    border-radius: 10px;"
        "    max-width: 50px;"
        "    min-width: 350px;"
        "    min-height: 100px;"
        "    margin-top: 10px;"
        "    padding: 15px;"
        "}"

        // 1위 이름 라벨
        "#topCommanderName {"
        "    color: white;"
        "    background-color: transparent;"
        "    font-family: '%1';"
        "}"

        // 오른쪽 랭킹 목록 패널 (이미지의 큰 사각형)
        "#rankPanel {"
        "    background-color: rgba(0, 0, 0, 100);" // 반투명 배경
        "    border: 1px solid #00FFFF;"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "    max-width: 400px;" // 폭 제한
        "}"

        // 각 랭킹 줄의 스타일
        "#rankRow {"
        "    color: #00FFFF;"
        "    padding: 8px 0;"
        "    border-bottom: 1px solid rgba(0, 255, 255, 50);" // 줄 구분선
        "}"

        "#topCommanderFrame[isCurrentUser=\"true\"] {"
        "    border: 3px solid #FFFF00; /* 밝은 노란색 테두리 */"
        "    background-color: rgba(255, 255, 0, 30); /* 희미한 노란 배경 */"
        "}"

        "#rankRow[isCurrentUser=\"true\"] {"
        "    color: #FFFF00; /* 밝은 노란색 텍스트 */"
        "    background-color: rgba(255, 255, 0, 30); /* 희미한 노란 배경 */"
        "    border-bottom: 1px solid #FFFF00; /* 노란색 밑줄 */"
        "}"

        // 뒤로가기 버튼 스타일 (다른 메뉴 버튼과 유사하게)
        "QPushButton#backButton {"
        "    font-family: '%1';"
        "    background-color: transparent;"
        "    border: none;"
        "    color: white;"
        "    font-size: 18px;"
        "    text-align: left;"
        "    padding: 5px;"
        "}"
        "QPushButton#backButton:hover {"
        "    color: #00FFFF;"
        "    text-decoration: underline;"
        "}"
        "QPushButton#menuButton:focus {"
        "    outline: none; /* 포커스 테두리(하얀 점선) 제거 */"
        "}"
        );

    QString qss = qssTemplate.arg(fontFamily);
    this->setStyleSheet(qss);
}

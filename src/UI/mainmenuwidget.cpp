#include "UI/mainmenuwidget.h"

// 필요한 Qt 클래스들
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication> // qApp
#include <QFontDatabase> // 폰트
#include <QGraphicsDropShadowEffect> // 네온 효과
#include <QDebug> // qWarning

MainMenuWidget::MainMenuWidget(QWidget *parent)
    : QWidget(parent)
{
    // 1. UI 위젯 생성 및 레이아웃 설정
    initUI();

    // 2. 폰트 및 QSS 스타일 적용
    loadStyles();

    // 3. 버튼 시그널 연결
    connect(startButton, &QPushButton::clicked, this, &MainMenuWidget::startGameClicked);
    connect(rankingButton, &QPushButton::clicked, this, &MainMenuWidget::rankingClicked);
    connect(myPageButton, &QPushButton::clicked, this, &MainMenuWidget::myPageClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainMenuWidget::logoutRequested);
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

}

MainMenuWidget::~MainMenuWidget()
{
}

/**
 * @brief LoginWidget의 UI 구조를 그대로 사용
 */
void MainMenuWidget::initUI()
{
    // === 1. 메인 세로 레이아웃 (LoginWidget과 동일) ===
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // === 2. 타이틀/서브타이틀 (LoginWidget과 동일) ===
    titleLabel = new QLabel("MISSILE COMMAND");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel = new QLabel("미사일 커맨드");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    // === 3. 중간 가로 레이아웃 (LoginWidget과 동일) ===
    QHBoxLayout *middleLayout = new QHBoxLayout();

    // --- [교체 1] 'loginPanel' 대신 'welcomePanel' ---
    welcomePanel = new QFrame();
    welcomePanel->setObjectName("loginPanel"); // QSS 재사용을 위해 ID를 "loginPanel"로 설정

    welcomeLabel = new QLabel("WELCOME,\nGUEST");
    welcomeLabel->setObjectName("welcomeLabel"); // QSS ID
    welcomeLabel->setAlignment(Qt::AlignCenter);

    // 환영 메시지를 패널 중앙에 배치
    QVBoxLayout *panelLayout = new QVBoxLayout(welcomePanel);
    panelLayout->addStretch(1);
    panelLayout->addWidget(welcomeLabel);
    panelLayout->addStretch(1);

    // --- [교체 2] 'buttonPanel' 대신 'menuPanel' ---
    menuPanel = new QWidget();
    menuPanel->setObjectName("buttonPanel"); // QSS 재사용을 위해 ID를 "buttonPanel"로 설정

    QVBoxLayout *menuLayout = new QVBoxLayout(menuPanel);
    menuLayout->setSpacing(5);

    startButton = new QPushButton("게임시작");
    startButton->setObjectName("menuButton"); // 새 QSS ID

    rankingButton = new QPushButton("랭킹");
    rankingButton->setObjectName("menuButton"); // 새 QSS ID

    myPageButton = new QPushButton("마이페이지");
    myPageButton->setObjectName("menuButton");

    logoutButton = new QPushButton("로그아웃");
    logoutButton->setObjectName("menuButton");

    exitButton = new QPushButton("게임종료");
    exitButton->setObjectName("menuButton"); // 새 QSS ID

    menuLayout->addWidget(startButton);
    menuLayout->addWidget(rankingButton);
    menuLayout->addWidget(myPageButton);
    menuLayout->addWidget(logoutButton);
    menuLayout->addWidget(exitButton);
    menuLayout->addStretch(1); // 버튼을 위로 정렬

    // === 4. 레이아웃 조립 (LoginWidget과 동일한 비율) ===
    middleLayout->addStretch(3);
    middleLayout->addWidget(welcomePanel, 4); // loginPanel과 동일한 비율(4)
    middleLayout->addSpacing(20);
    middleLayout->addWidget(menuPanel, 2);    // buttonPanel과 동일한 비율(2)
    middleLayout->addStretch(3);

    // === 5. 전체 조립 (LoginWidget과 동일) ===
    mainLayout->addStretch(2);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(middleLayout, 1);
    mainLayout->addStretch(3);
}

/**
 * @brief LoginWidget의 스타일을 기반으로 수정
 */
void MainMenuWidget::loadStyles()
{
    // --- 1. 폰트 로드 ---
    QString fontFamily = "Arial";
    int fontId = QFontDatabase::addApplicationFont("resources/pixel_font.ttf");

    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        qWarning() << "MainMenuWidget: 픽셀 폰트 로드 실패!";
    }

    // --- 2. 네온 효과 (LoginWidget과 동일) ---
    if (fontId != -1) {

        // 타이틀
        QFont pixelFont(fontFamily, 48);
        titleLabel->setFont(pixelFont);
        QGraphicsDropShadowEffect *neonEffect = new QGraphicsDropShadowEffect();
        neonEffect->setColor(QColor(200, 255, 255, 200));
        neonEffect->setBlurRadius(30);
        neonEffect->setOffset(0, 0);
        titleLabel->setGraphicsEffect(neonEffect);

        // 서브타이틀
        QGraphicsDropShadowEffect *smallGlow1 = new QGraphicsDropShadowEffect();
        smallGlow1->setColor(QColor(255, 255, 255, 180));
        smallGlow1->setBlurRadius(15);
        smallGlow1->setOffset(0, 0);
        subtitleLabel->setGraphicsEffect(smallGlow1);
    }

    // "WELCOME" 라벨 폰트/효과
    QFont welcomeFont(fontFamily, 20); // 크기 조절
    welcomeLabel->setFont(welcomeFont);
    QGraphicsDropShadowEffect *neonEffect2 = new QGraphicsDropShadowEffect();
    neonEffect2->setColor(QColor(200, 255, 255, 200));
    neonEffect2->setBlurRadius(25);
    neonEffect2->setOffset(0, 0);
    welcomeLabel->setGraphicsEffect(neonEffect2);

    // 메뉴 버튼 폰트
    QFont buttonFont(fontFamily, 18);
    startButton->setFont(buttonFont);
    rankingButton->setFont(buttonFont);
    myPageButton->setFont(buttonFont);
    logoutButton->setFont(buttonFont);
    exitButton->setFont(buttonFont);
    // (버튼 네온 효과는 QSS :hover로 처리)


    // --- 3. QSS 스타일시트 ---
    QString qssTemplate = QStringLiteral(
        // 이 페이지 배경 투명
        "MainMenuWidget { background-color: transparent; }"

        // LoginWidget에서 복사 (폰트 arg(%1) 추가)
        "#titleLabel { background-color: transparent; color: #00FFFF; font-family: '%1'; }"
        "#subtitleLabel { font-family: '%1'; color: #0AFFF7; font-size: 36px; background-color: transparent; }"

        // LoginWidget의 #loginPanel 스타일 재사용 (내용물 크기에 맞춰 높이 자동조절)
        "#loginPanel {"
        "    background-color: rgba(160, 255, 247, 50);"
        "    border: 1px solid #00FFFF;"
        "    border-radius: 8px;"
        "    max-width: 350px;"
        "    min-height: 120px;" // LoginWidget과 동일하게 유지
        "    padding: 10px;" // 내부 여백
        "}"

        // 'WELCOME' 라벨
        "QLabel#welcomeLabel {"
        "    font-family: '%1';"
        "    color: #FFFFFF;"
        "    background-color: transparent;"
        "    border: none;"
        "    padding: 10px;"
        "}"

        // LoginWidget의 #buttonPanel 스타일 재사용
        "#buttonPanel {"
        "    background-color: transparent;"
        "    max-width: 150px;"
        "}"

        // 새 메뉴 버튼 스타일 (Login의 버튼 스타일과 동일하게)
        "QPushButton#menuButton {"
        "    font-family: '%1';"
        "    background-color: transparent;"
        "    border: none;"
        "    color: white;"
        "    font-size: 18px;"
        "    text-align: left;"
        "    padding: 5px;"
        "}"
        "QPushButton#menuButton:hover {"
        "    color: #00FFFF;"
        "    text-decoration: underline;"
        "}"
        "QPushButton#menuButton:focus {"
        "    outline: none; /* 포커스 테두리(하얀 점선) 제거 */"
        "}"
        );

    // 폰트 이름을 QSS에 적용
    QString qss = qssTemplate.arg(fontFamily);
    this->setStyleSheet(qss);
}

/**
 * @brief 부모(Widget)가 호출할 슬롯. 유저 이름으로 텍스트 업데이트
 */
void MainMenuWidget::setUsername(const QString& username)
{
    // "WELCOME, [USERNAME]" 형식으로 텍스트 변경
    QString welcomeText = QString("WELCOME,\n%1").arg(username.toUpper());
    welcomeLabel->setText(welcomeText);
}

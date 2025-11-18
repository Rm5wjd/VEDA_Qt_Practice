#include "UI/loginwidget.h"

// 필요한 Qt 클래스들
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QFontDatabase>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    loadStyles();
}

LoginWidget::~LoginWidget()
{
}

/**
 * @brief '회원가입' 버튼 클릭 시 시그널 발생
 */
void LoginWidget::onSignupButtonClicked()
{
    // 메인 창(Widget)에 '회원가입' 버튼이 눌렸다고 알림
    emit signupClicked();

}

void LoginWidget::initUI()
{
    // 배경 업데이트 함수 호출 삭제

    // 레이아웃 코드는 기존과 동일
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 페이지는 여백 없이

    titleLabel = new QLabel("MISSILE COMMAND");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel = new QLabel("미사일 커맨드");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *middleLayout = new QHBoxLayout();
    QFrame *loginPanel = new QFrame();
    loginPanel->setObjectName("loginPanel");
    QGridLayout *loginLayout = new QGridLayout(loginPanel);

    QLabel *idIconLabel = new QLabel();
    idIconLabel->setPixmap(QPixmap("resources/icon_user.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    idLineEdit = new QLineEdit();
    idLineEdit->setPlaceholderText("ID");
    idLineEdit->setObjectName("loginLineEdit");

    QLabel *pwIconLabel = new QLabel();
    pwIconLabel->setPixmap(QPixmap("resources/icon_lock.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setObjectName("loginLineEdit");

    loginLayout->addWidget(idIconLabel, 0, 0);
    loginLayout->addWidget(idLineEdit, 0, 1);
    loginLayout->addWidget(pwIconLabel, 1, 0);
    loginLayout->addWidget(passwordLineEdit, 1, 1);
    loginLayout->setColumnStretch(1, 1);

    QWidget *buttonPanel = new QWidget();
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonPanel);
    buttonPanel->setObjectName("buttonPanel");

    loginButton = new QPushButton("로그인");
    signupButton = new QPushButton("회원가입");
    quitButton = new QPushButton("게임종료");

    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    // [수정 1] "로그인" 버튼에 대한 connect 추가
    connect(loginButton, &QPushButton::clicked, this, [=]() {
        // 'ui->'가 아니라 헤더에 선언된 멤버 변수 'idLineEdit'를 사용합니다.
        QString username = idLineEdit->text();
        QString password = passwordLineEdit->text();

        // 'loginRequested' 시그널을 발생시켜 부모(Widget)에게 알림
        emit loginRequested(username, password);
    });

    // '회원가입' 버튼을 누르면 이 클래스의 onSignupButtonClicked 슬롯에 연결
    connect(signupButton, &QPushButton::clicked, this, &LoginWidget::onSignupButtonClicked);

    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(signupButton);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    middleLayout->addStretch(3);
    middleLayout->addWidget(loginPanel, 4);
    middleLayout->addSpacing(20);
    middleLayout->addWidget(buttonPanel, 2);
    middleLayout->addStretch(3);

    mainLayout->addStretch(2);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(middleLayout, 1);
    mainLayout->addStretch(3);
}

void LoginWidget::loadStyles()
{
    // 폰트 로드 및 네온 효과 코드는 기존과 동일
    QString fontFamily = "Arial";
    int fontId = QFontDatabase::addApplicationFont("resources/pixel_font.ttf");

    if (fontId == -1) {
        qWarning() << "Pixel 폰트 로드 실패! 'resources/pixel_font.ttf' 경로를 확인하세요.";
    } else {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);

        QFont pixelFont(fontFamily, 48);
        titleLabel->setFont(pixelFont);

        QGraphicsDropShadowEffect *neonEffect = new QGraphicsDropShadowEffect();
        neonEffect->setColor(QColor(200, 255, 255, 200));
        neonEffect->setBlurRadius(30);
        neonEffect->setOffset(0, 0);
        titleLabel->setGraphicsEffect(neonEffect);

        // ... (다른 위젯 네온 효과들 ...
        QGraphicsDropShadowEffect *smallGlow1 = new QGraphicsDropShadowEffect();
        smallGlow1->setColor(QColor(255, 255, 255, 180));
        smallGlow1->setBlurRadius(15);
        smallGlow1->setOffset(0, 0);
        subtitleLabel->setGraphicsEffect(smallGlow1);

        QGraphicsDropShadowEffect *smallGlow2 = new QGraphicsDropShadowEffect();
        smallGlow2->setColor(QColor(255, 255, 255, 180));
        smallGlow2->setBlurRadius(15);
        smallGlow2->setOffset(0, 0);
        loginButton->setGraphicsEffect(smallGlow2);

        QGraphicsDropShadowEffect *smallGlow3 = new QGraphicsDropShadowEffect();
        smallGlow3->setColor(QColor(255, 255, 255, 180));
        smallGlow3->setBlurRadius(15);
        smallGlow3->setOffset(0, 0);
        signupButton->setGraphicsEffect(smallGlow3);

        QGraphicsDropShadowEffect *smallGlow4 = new QGraphicsDropShadowEffect();
        smallGlow4->setColor(QColor(255, 255, 255, 180));
        smallGlow4->setBlurRadius(15);
        smallGlow4->setOffset(0, 0);
        quitButton->setGraphicsEffect(smallGlow4);
    }

    // === 2. QSS 스타일시트 적용 ===
    QString qssTemplate = QStringLiteral(
        // 이 페이지의 배경을 투명하게 하여 메인 창의 배경이 보이도록 함
        "LoginWidget {"
        "    background-color: transparent;"
        "}"

        "#titleLabel {"
        "    background-color: transparent;"
        "    color: #00FFFF;"
        "}"
        "#subtitleLabel {"
        "    font-family: '%1';"
        "    color: #0AFFF7;"
        "    font-size: 36px;"
        "    background-color: transparent;"
        "}"
        "#loginPanel {"
        "    background-color: rgba(160, 255, 247, 50);"
        "    border: 1px solid #00FFFF;"
        "    border-radius: 8px;"
        "    max-width: 350px;"
        "    min-height: 120px;"
        "}"
        "#loginPanel QLabel {"
        "    background-color: transparent;"
        "    padding-left: 5px;"
        "}"
        "QLineEdit#loginLineEdit {"
        "    font-family: '%1';"
        "    background-color: rgba(160, 255, 247, 100);"
        "    border: none;"
        "    border-radius: 4px;"
        "    color: white;"
        "    font-size: 16px;"
        "    padding: 8px;"
        "}"
        "QLineEdit#loginLineEdit:focus {"
        "    background-color: rgba(160, 255, 247, 150);"
        "}"
        "#buttonPanel {"
        "    background-color: transparent;"
        "    max-width: 150px;"
        "}"
        "#buttonPanel QPushButton {"
        "    font-family: '%1';"
        "    background-color: transparent;"
        "    border: none;"
        "    color: white;"
        "    font-size: 18px;"
        "    text-align: left;"
        "    padding: 5px;"
        "}"
        "#buttonPanel QPushButton:hover {"
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

void LoginWidget::clearFields()
{
    // idLineEdit와 passwordLineEdit는
    // LoginWidget의 멤버 변수여야 합니다. (이전 코드 기준)

    if (idLineEdit) {
        idLineEdit->clear();
    }
    if (passwordLineEdit) {
        passwordLineEdit->clear();
    }
}

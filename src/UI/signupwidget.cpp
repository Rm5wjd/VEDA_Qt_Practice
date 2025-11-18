#include "UI/signupwidget.h"

#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QDebug> // qWarning용

SignupWidget::SignupWidget(QWidget *parent)
    : QWidget(parent) // QDialog가 아닌 QWidget
{
    // 윈도우 플래그, 투명화 코드를 모두 삭제

    initUI();
    loadStyles();
}

SignupWidget::~SignupWidget()
{
}

/**
 * @brief 'Back' 버튼 클릭 시 시그널 발생
 */
void SignupWidget::onBackButtonClicked()
{
    // 메인 창(Widget)에 'Back' 버튼이 눌렸다고 알림
    emit backClicked();
}

void SignupWidget::onSignupButtonClicked()
{
    // 1. UI에서 텍스트 가져오기 (멤버 변수 이름을 사용)
    QString username = idEdit->text();
    QString password = passEdit->text();
    QString confirm = passConfirmEdit->text();

    // 2. [필수] 입력값 검증
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "아이디와 비밀번호를 모두 입력하세요.");
        return; // 함수 종료
    }

    if (password != confirm) {
        QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
        return; // 함수 종료
    }

    // 3. 검증 통과 시, 부모(Widget)에게 DB 처리를 요청하는 시그널 전송
    emit signupRequested(username, password);
}

void SignupWidget::initUI()
{
    // 레이아웃 코드는 기존과 동일
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 페이지는 여백 없이

    titleLabel = new QLabel("NEW COMMANDER REGISTRATION");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    mainPanel = new QFrame();
    mainPanel->setObjectName("mainPanel");

    QGridLayout *panelLayout = new QGridLayout(mainPanel);
    panelLayout->setContentsMargins(30, 30, 30, 30);

    QPixmap userIcon("resources/icon_user.png");
    QPixmap lockIcon("resources/icon_lock.png");

    QLabel *idIcon = new QLabel();
    idIcon->setPixmap(userIcon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    idEdit = new QLineEdit();
    idEdit->setPlaceholderText("ID");
    idEdit->setObjectName("signupEdit");

    QLabel *passIcon = new QLabel();
    passIcon->setPixmap(lockIcon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    passEdit = new QLineEdit();
    passEdit->setPlaceholderText("Password");
    passEdit->setEchoMode(QLineEdit::Password);
    passEdit->setObjectName("signupEdit");

    QLabel *confirmIcon = new QLabel();
    confirmIcon->setPixmap(lockIcon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    passConfirmEdit = new QLineEdit();
    passConfirmEdit->setPlaceholderText("Password Confirm");
    passConfirmEdit->setEchoMode(QLineEdit::Password);
    passConfirmEdit->setObjectName("signupEdit");

    activateButton = new QPushButton("Activate Commander");
    activateButton->setObjectName("activateButton");
    backButton = new QPushButton("Back");
    backButton->setObjectName("backButton");

    // [수정] 'Back' 버튼을 누르면 이 클래스의 onBackButtonClicked 슬롯에 연결
    connect(backButton, &QPushButton::clicked, this, &SignupWidget::onBackButtonClicked);

    connect(activateButton, &QPushButton::clicked, this, &SignupWidget::onSignupButtonClicked);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(activateButton, 2);
    buttonLayout->addSpacing(15);
    buttonLayout->addWidget(backButton, 2);

    panelLayout->addWidget(idIcon, 0, 0);
    panelLayout->addWidget(idEdit, 0, 1);
    panelLayout->addWidget(passIcon, 1, 0);
    panelLayout->addWidget(passEdit, 1, 1);
    panelLayout->addWidget(confirmIcon, 2, 0);
    panelLayout->addWidget(passConfirmEdit, 2, 1);
    panelLayout->addLayout(buttonLayout, 3, 1);
    panelLayout->setColumnStretch(1, 1);

    mainLayout->addStretch(1);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    // [수정] 패널이 너무 커지지 않도록 중앙에 적절히 배치
    QHBoxLayout *panelHLayout = new QHBoxLayout();
    panelHLayout->addStretch(1);
    panelHLayout->addWidget(mainPanel, 2); // 비율 2
    panelHLayout->addStretch(1);

    mainLayout->addLayout(panelHLayout);
    mainLayout->addStretch(2);
}

void SignupWidget::loadStyles()
{
    // 폰트 로드 및 네온 효과 코드는 기존과 동일
    QString fontFamily = "Arial";
    int fontId = QFontDatabase::addApplicationFont("resources/pixel_font.ttf");

    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);

        QFont titleFont(fontFamily, 36);
        titleLabel->setFont(titleFont);

        QGraphicsDropShadowEffect *neonEffect = new QGraphicsDropShadowEffect();
        neonEffect->setColor(QColor(200, 255, 255, 200));
        neonEffect->setBlurRadius(30);
        neonEffect->setOffset(0, 0);
        titleLabel->setGraphicsEffect(neonEffect);
    }

    // === 2. QSS 스타일시트 ===
    QString qssTemplate = QStringLiteral(
        // [수정] 이 페이지의 배경을 투명하게 하여 메인 창의 배경이 보이도록 함
        "SignupWidget {"
        "    background-color: transparent;"
        "}"

        // 나머지 스타일은 기존과 동일
        "#titleLabel {"
        "    background-color: transparent;"
        "    color: #00FFFF;"
        "}"
        "QFrame#mainPanel {"
        "    background-color: rgba(10, 30, 80, 220);"
        "    border: 1px solid #00FFFF;"
        "    border-radius: 25px;"
        "}"
        "QFrame#mainPanel QLabel {"
        "    background-color: transparent;"
        "}"
        "QLineEdit#signupEdit {"
        "    font-family: '%1';"
        "    background-color: #0A294E;"
        "    border: none;"
        "    border-radius: 4px;"
        "    color: white;"
        "    font-size: 16px;"
        "    padding: 8px;"
        "}"
        "QLineEdit#signupEdit:focus {"
        "    background-color: #1A4A8E;"
        "}"
        "QPushButton {"
        "    font-family: '%1';"
        "    background-color: #0A4A8E;"
        "    color: white;"
        "    font-size: 16px;"
        "    border: 1px solid #00FFFF;"
        "    border-radius: 4px;"
        "    padding: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1A6AEE;"
        "}"
        "QPushButton#backButton {"
        "    background-color: #333333;"
        "    border-color: #AAAAAA;"
        "}"
        "QPushButton#backButton:hover {"
        "    background-color: #555555;"
        "}"
        "QPushButton#menuButton:focus {"
        "    outline: none; /* 포커스 테두리(하얀 점선) 제거 */"
        "}"
        );

    QString qss = qssTemplate.arg(fontFamily);
    this->setStyleSheet(qss);
}

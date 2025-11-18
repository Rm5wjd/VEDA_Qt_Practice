#include "UI/mypagewidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QMessageBox>
#include <QFrame>

MyPageWidget::MyPageWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    loadStyles();

    // 시그널 연결
    connect(backButton, &QPushButton::clicked, this, &MyPageWidget::backClicked); // [다시 연결]
    connect(changeUsernameButton, &QPushButton::clicked, this, &MyPageWidget::onChangeButtonClicked);

    connect(deleteAccountButton, &QPushButton::clicked, this, [=](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "계정 삭제",
                                     "정말로 계정을 삭제하시겠습니까?\n이 작업은 되돌릴 수 없습니다.",
                                     QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            emit deleteAccountRequested();
        }
    });
}

void MyPageWidget::initUI()
{
    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setContentsMargins(50, 50, 50, 50);

    // === 1. 왼쪽 섹션: "MY PAGE" 타이틀 ===
    QVBoxLayout *leftSection = new QVBoxLayout();
    leftSection->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    titleLabel = new QLabel("MY PAGE", this);
    titleLabel->setObjectName("leftTitleLabel");
    leftSection->addWidget(titleLabel);
    leftSection->addStretch(1);

    // === 2. 오른쪽 섹션: 정보 패널 및 버튼들 ===
    QVBoxLayout *rightSection = new QVBoxLayout();
    rightSection->setAlignment(Qt::AlignTop | Qt::AlignRight);

    // infoPanel 생성 및 스타일 설정
    infoPanel = new QFrame(this);
    infoPanel->setObjectName("infoPanel");
    infoPanel->setFixedSize(350, 350); // 이미지에 맞춰 고정 크기 (조절 가능)

    QVBoxLayout *panelLayout = new QVBoxLayout(infoPanel);
    panelLayout->setContentsMargins(20, 20, 20, 20);
    panelLayout->setSpacing(15);
    panelLayout->setAlignment(Qt::AlignTop);

    // 내부의 각 라벨/LineEdit/버튼들
    currentUsernameLabel = new QLabel("ID : -", this); // [수정] "ID : VEDA"를 담을 라벨
    currentUsernameLabel->setObjectName("fieldValueLabel"); // 이미지상 ID는 값이므로 fieldValueLabel 스타일

    QLabel *highScorePrefixLabel = new QLabel("HIGH SCORE :", this);
    highScorePrefixLabel->setObjectName("fieldPrefixLabel"); // 접두사 스타일
    highScoreLabel = new QLabel("9999", this); // 실제 최고 점수
    highScoreLabel->setObjectName("fieldValueLabel"); // 값 스타일

    QLabel *enterNewIdLabel = new QLabel("Enter new ID", this);
    enterNewIdLabel->setObjectName("fieldPrefixLabel");

    newUsernameEdit = new QLineEdit(this);
    newUsernameEdit->setPlaceholderText(""); // [수정] 이미지상 플레이스홀더 텍스트 없음
    newUsernameEdit->setObjectName("idLineEdit");

    changeUsernameButton = new QPushButton("변경", this);
    changeUsernameButton->setObjectName("changeIdButton");

    // "ID" 필드 레이아웃
    QVBoxLayout *idSectionLayout = new QVBoxLayout(); // 전체 ID 섹션
    idSectionLayout->addWidget(currentUsernameLabel);
    idSectionLayout->addSpacing(5); // 라벨과 다음 필드 간격

    // "HIGH SCORE" 필드 레이아웃
    QVBoxLayout *highScoreSectionLayout = new QVBoxLayout();
    QHBoxLayout *highScoreDisplayLayout = new QHBoxLayout(); // 'HIGH SCORE :'와 '9999'를 가로로
    highScoreDisplayLayout->addWidget(highScorePrefixLabel);
    highScoreDisplayLayout->addStretch(1); // 'HIGH SCORE :'와 값 사이 공간
    highScoreDisplayLayout->addWidget(highScoreLabel);
    highScoreSectionLayout->addLayout(highScoreDisplayLayout);
    highScoreSectionLayout->addSpacing(5);

    // "Enter new ID" 및 "변경" 버튼 레이아웃
    QVBoxLayout *changeIdSectionLayout = new QVBoxLayout();
    changeIdSectionLayout->addWidget(enterNewIdLabel); // 'Enter new ID' 라벨
    QHBoxLayout *changeIdLayout = new QHBoxLayout();
    changeIdLayout->addWidget(newUsernameEdit);
    changeIdLayout->addWidget(changeUsernameButton);
    changeIdSectionLayout->addLayout(changeIdLayout);


    panelLayout->addLayout(idSectionLayout);
    panelLayout->addLayout(highScoreSectionLayout);
    panelLayout->addSpacing(30);
    panelLayout->addLayout(changeIdSectionLayout);
    panelLayout->addStretch(1);

    rightSection->addWidget(infoPanel);

    // 계정 삭제 버튼 (패널 바로 아래)
    deleteAccountButton = new QPushButton("계정 삭제", this);
    deleteAccountButton->setObjectName("deleteButton");
    deleteAccountButton->setFixedSize(infoPanel->width(), 40);
    rightSection->addWidget(deleteAccountButton);

    rightSection->addStretch(1); // 이 스트레치로 뒤로가기 버튼을 아래로 밀어냄

    // [추가] 뒤로가기 버튼 (오른쪽 하단)
    backButton = new QPushButton("뒤로가기", this); // [추가]
    backButton->setObjectName("backButton"); // [추가] QSS ID
    rightSection->addWidget(backButton, 0, Qt::AlignRight | Qt::AlignBottom); // [수정] 오른쪽 아래 정렬

    // 메인 레이아웃에 왼쪽/오른쪽 섹션 추가
    topLayout->addLayout(leftSection, 1);
    topLayout->addSpacing(50);
    topLayout->addLayout(rightSection, 1);
}

void MyPageWidget::loadStyles()
{
    QString fontFamily = "Arial";
    int fontId = QFontDatabase::addApplicationFont("resources/pixel_font.ttf");

    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        qWarning() << "MyPageWidget: 픽셀 폰트 로드 실패!";
    }

    // 폰트 적용
    QFont leftTitleFont(fontFamily, 48);
    titleLabel->setFont(leftTitleFont);

    QFont fieldFont(fontFamily, 16);
    currentUsernameLabel->setFont(fieldFont); // [수정]
    highScoreLabel->setFont(fieldFont);
    newUsernameEdit->setFont(fieldFont);

    QFont buttonFont(fontFamily, 14);
    changeUsernameButton->setFont(buttonFont);
    deleteAccountButton->setFont(buttonFont);
    backButton->setFont(buttonFont); // [추가] 폰트 적용


    // QSS (이미지 스타일 최대한 반영)
    QString qssTemplate = QStringLiteral(
        "MyPageWidget { background: transparent; }"

        "#leftTitleLabel { "
        "   font-family: '%1'; "
        "   color: #00FFFF; "
        "   background-color: transparent; "
        "   font-size: 48px; "
        "   qproperty-alignment: 'AlignLeft | AlignVCenter'; "
        "}"

        "#infoPanel {"
        "   background-color: rgba(160, 255, 247, 50);"
        "   border: 1px solid #00FFFF;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "}"

        // 패널 내부의 'ID :', 'HIGH SCORE :', 'Enter new ID' 같은 라벨
        // 이미지상 "ID : VEDA" 자체가 하나의 값 라벨처럼 보이므로 fieldPrefixLabel은 High Score Prefix에만 사용
        "QLabel#fieldPrefixLabel {"
        "   font-family: '%1';"
        "   color: #00FFFF;" // 하늘색
        "   font-size: 16px;"
        "   background-color: transparent;"
        "   qproperty-alignment: 'AlignLeft | AlignVCenter'; "
        "}"

        // 패널 내부의 실제 값 라벨 ("ID : VEDA", "9999")
        "QLabel#fieldValueLabel {"
        "   font-family: '%1';"
        "   color: white;" // 흰색
        "   font-size: 16px;"
        "   background-color: transparent;"
        "   qproperty-alignment: 'AlignLeft | AlignVCenter'; " // [수정] 왼쪽 정렬
        "   padding-left: 0px; " // [수정] 패딩 제거
        "}"

        "QLabel#fieldValueLabel.id {" // ID 라벨에만 추가적인 스타일을 줄 수 있도록 클래스 선택자 활용
        "   qproperty-alignment: 'AlignLeft | AlignVCenter'; "
        "}"


        // 입력 필드 (Enter new ID)
        "QLineEdit#idLineEdit {"
        "   font-family: '%1';"
        "   background-color: rgba(0, 255, 255, 30);"
        "   border: 1px solid #00FFFF;"
        "   border-radius: 5px;"
        "   color: white;"
        "   padding: 5px;"
        "   font-size: 16px;"
        "}"
        "QLineEdit#idLineEdit::placeholder {"
        "   color: #C0F0F0;"
        "}"

        // '변경' 버튼
        "QPushButton#changeIdButton {"
        "   font-family: '%1';"
        "   background-color: #00FFFF;"
        "   color: black;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 5px 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton#changeIdButton:hover {"
        "   background-color: #33FFFF;"
        "}"

        // '계정 삭제' 버튼
        "QPushButton#deleteButton {"
        "   font-family: '%1';"
        "   background-color: #FF0000;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 10px 20px;"
        "   font-size: 18px;"
        "}"
        "QPushButton#deleteButton:hover {"
        "   background-color: #CC0000;"
        "}"

        // [추가] '뒤로가기' 버튼
        "QPushButton#backButton {"
        "   font-family: '%1';"
        "   background-color: transparent;"
        "   color: white;"
        "   border: 1px solid white;" // 흰색 테두리
        "   border-radius: 5px;"
        "   padding: 8px 15px;"
        "   font-size: 16px;"
        "}"
        "QPushButton#backButton:hover {"
        "   background-color: rgba(255, 255, 255, 30);" // 살짝 밝아지는 효과
        "   border: 1px solid #00FFFF;"
        "   color: #00FFFF;"
        "}"
        "QPushButton#menuButton:focus {"
        "    outline: none; /* 포커스 테두리(하얀 점선) 제거 */"
        "}"
        );

    QString qss = qssTemplate.arg(fontFamily);
    this->setStyleSheet(qss);
}

void MyPageWidget::setCurrentUsername(const QString& username)
{
    currentLoadedUsername = username;
    currentUsernameLabel->setText(QString("ID : %1").arg(username.toUpper())); // [수정] "ID : " 접두사 추가
    newUsernameEdit->clear();
}

void MyPageWidget::setHighScore(int score)
{
    highScoreLabel->setText(QString::number(score));
}

void MyPageWidget::onChangeButtonClicked()
{
    QString newUsername = newUsernameEdit->text().trimmed();
    if (newUsername.isEmpty()) {
        QMessageBox::warning(this, "오류", "새 아이디를 입력하세요.");
        return;
    }
    if (newUsername == currentLoadedUsername) {
        QMessageBox::warning(this, "오류", "현재 아이디와 동일합니다.");
        return;
    }
    emit changeUsernameRequested(newUsername);
}

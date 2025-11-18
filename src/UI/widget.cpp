#include "UI/widget.h"
#include "UI/loginwidget.h"  // 로그인 페이지 헤더
#include "UI/signupwidget.h" // 회원가입 페이지 헤더
#include "UI/mainmenuwidget.h" // 메인메뉴 페이지 헤더
#include "UI/rankingwidget.h" // 랭킹 페이지
#include "UI/mypagewidget.h" // 마이 페이지
#include "game/gamewidget.h"

// 필요한 Qt 클래스들
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPalette>
#include <QPixmap>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("MISSILE COMMAND");

    initDatabase();

    // 1. 배경 스위치를 켭니다. (가장 중요)
    this->setAutoFillBackground(true);

    // 2. 초기 크기를 설정합니다. (FixedSize가 아님)
    resize(1024, 800);

    // 3. 페이지 스택 위젯 생성
    stackedWidget = new QStackedWidget(this);

    // 4. 페이지들 생성
    loginPage = new LoginWidget(this);
    signupPage = new SignupWidget(this);
    mainMenuPage = new MainMenuWidget(this);
    myPage = new MyPageWidget(this);

    // game!
    game = new GameWidget(this);

    // 5. 스택에 페이지 추가 (0번 = 로그인, 1번 = 회원가입)
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(signupPage);
    stackedWidget->addWidget(mainMenuPage);
    stackedWidget->addWidget(myPage);

    stackedWidget->addWidget(game);

    rankingWidget = new RankingWidget(this); // 랭킹 위젯 생성
    connect(rankingWidget, &RankingWidget::backClicked,
            this, [this](){
                // '뒤로가기' 클릭 시 메인 메뉴로 돌아가기
                stackedWidget->setCurrentWidget(mainMenuPage);
            });

    // 랭킹 위젯을 스택에 추가
    stackedWidget->addWidget(rankingWidget);

    // 6. 메인 창의 레이아웃 설정 (스택 위젯을 꽉 채움)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 여백 없음
    mainLayout->addWidget(stackedWidget);

    // 7. 시그널 연결: 페이지 전환 로직

    // ... (mainMenuWidget의 랭킹 버튼 연결)
    connect(mainMenuPage, &MainMenuWidget::rankingClicked, this, [this](){
        // 랭킹 버튼 클릭 시 랭킹 페이지로 이동하고 랭킹 데이터 로드
        stackedWidget->setCurrentWidget(rankingWidget);
        rankingWidget->loadRanks(this->currentUsername); // 랭킹 화면 표시 시 데이터 로드 함수 호출
    });

    // 로그인 페이지의 signupClicked 시그널이 울리면...
    connect(loginPage, &LoginWidget::signupClicked, this, [=]() {
        // 스택 위젯의 1번(회원가입) 페이지를 보여준다
        stackedWidget->setCurrentIndex(1);
    });

    // 회원가입 페이지의 backClicked 시그널이 울리면...
    connect(signupPage, &SignupWidget::backClicked, this, [=]() {
        // 스택 위젯의 0번(로그인) 페이지를 보여준다
        stackedWidget->setCurrentIndex(0);
    });

    // loginPage가 "로그인 요청" 시그널을 보내면,
    // Widget의 "login" 함수를 실행합니다.
    connect(loginPage, &LoginWidget::loginRequested, this, &Widget::login);

    // signupPage가 "회원가입 요청" 시그널을 보내면,
    // Widget의 "signUp" 함수를 실행합니다.
    connect(signupPage, &SignupWidget::signupRequested, this, &Widget::signUp);

    // ************************************************
    // ** [추가] 로그아웃 시그널 연결
    // ************************************************
    connect(mainMenuPage, &MainMenuWidget::logoutRequested, this, [=]() {
        // 로그 아웃시 필드 초기화
        loginPage->clearFields();

        // 2. 0번(로그인) 페이지를 보여준다
        stackedWidget->setCurrentIndex(0);
    });
    // ************************************************
    // ** [추가] 마이페이지 시그널 연결
    // ************************************************

    // MainMenu -> MyPage
    connect(mainMenuPage, &MainMenuWidget::myPageClicked, this, [=](){
        // 마이페이지로 이동하기 전, 현재 사용자 이름을 전달
        myPage->setCurrentUsername(this->currentUsername);
        stackedWidget->setCurrentWidget(myPage);
    });

    // MainMenu -> game
    connect(mainMenuPage, &MainMenuWidget::startGameClicked, this, [=](){
        // 마이페이지로 이동하기 전, 현재 사용자 이름을 전달
        stackedWidget->setCurrentWidget(game);
        game->gameStart();
    });

    // MyPage -> MainMenu (뒤로가기)
    connect(myPage, &MyPageWidget::backClicked, this, [=](){
        stackedWidget->setCurrentWidget(mainMenuPage);
    });

    // MyPage -> 아이디 변경 로직 호출
    connect(myPage, &MyPageWidget::changeUsernameRequested, this, &Widget::handleChangeUsername);

    // MyPage -> 계정 삭제 로직 호출
    connect(myPage, &MyPageWidget::deleteAccountRequested, this, &Widget::handleDeleteAccount);

    // 8. 처음 실행 시 0번(로그인) 페이지를 보여줌
    stackedWidget->setCurrentIndex(0);
}

Widget::~Widget()
{
}

/**
 * @brief 배경 업데이트 함수 (기존 코드)
 */
void Widget::updateScaledBackground()
{
    QPalette palette = this->palette();
    QPixmap bg(":/UI/background_UI.jpg");

    if (bg.isNull()) {
        qWarning("배경 이미지를 찾을 수 없습니다! (:/UI/background_UI.jpg)");
        return;
    }

    bg = bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    palette.setBrush(QPalette::Window, QBrush(bg));
    this->setPalette(palette);
}

/**
 * @brief 리사이즈 이벤트 (기존 코드)
 */
void Widget::resizeEvent(QResizeEvent *event)
{
    updateScaledBackground(); // 창 크기가 바뀔 때마다 배경 다시 그림
    QWidget::resizeEvent(event);
}

/**
 * @brief 프로그램 시작 시 DB를 초기화하고 'users' 테이블을 생성합니다.
 * 이 함수는 Widget 생성자 같은 곳에서 딱 한 번만 호출하면 됩니다.
 */
void Widget::initDatabase()
{
    // 1. DB 연결 (드라이버: QSQLITE)
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db"); // 'users.db' 라는 파일명으로 DB 생성

    // 2. DB 열기
    if (!db.open()) {
        qWarning() << "Database Error: " << db.lastError().text();
        QMessageBox::critical(this, "DB 연결 실패", "데이터베이스를 열 수 없습니다.");
        return;
    }

    qDebug() << "Database connected!";

    // 3. 'users' 테이블 생성 (없을 경우에만)
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username TEXT UNIQUE NOT NULL,"     // 사용자 아이디 (중복 불가)
        "    password_hash TEXT NOT NULL"       // 해시된 비밀번호
        " );"
        );

    if (!success) {
        qWarning() << "Table Create Error: " << query.lastError().text();
    }
}


/**
 * @brief 회원가입 함수
 * @param username 사용자 ID
 * @param password 원본 비밀번호
 * @return 성공 시 true, 실패(중복 ID 등) 시 false
 */
bool Widget::signUp(const QString& username, const QString& password)
{
    // 1. 비밀번호 해싱 (보안!)
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString passwordHash = hash.toHex();

    // 2. SQL 쿼리 준비 (SQL Injection 방지)
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(passwordHash);

    // 3. 쿼리 실행
    if (!query.exec()) {
        qWarning() << "Sign-up Error: " << query.lastError().text();
        // 아이디 중복(UNIQUE) 오류일 가능성이 높음
        QMessageBox::warning(this, "회원가입 실패", "아이디가 중복되거나 오류가 발생했습니다.");
        return false;
    }

    QMessageBox::information(this, "회원가입 성공", "회원가입이 완료되었습니다.");
    return true;
}


/**
 * @brief 로그인 함수
 * @param username 사용자 ID
 * @param password 원본 비밀번호
 * @return 성공 시 true, 실패 시 false
 */
bool Widget::login(const QString& username, const QString& password)
{
    // 1. DB에서 해당 username의 해시값 가져오기
    QSqlQuery query;
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        qWarning() << "Login SQL Error: " << query.lastError().text();
        return false;
    }

    // 2. 사용자가 존재하는지 확인
    if (query.next()) {
        QString storedHash = query.value(0).toString(); // DB에 저장된 해시

        // 3. 입력된 비밀번호를 동일한 방식으로 해싱
        QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        QString providedHash = hash.toHex();

        // 4. 두 해시 값이 일치하는지 비교
        if (storedHash == providedHash) {
            qDebug() << "Login success for user:" << username;
            // --- [핵심 수정] ---
            // 1. 로그인한 유저 이름을 멤버 변수에 저장
            this->currentUsername = username;

            // 2. MainMenuWidget에 유저 이름 전달 (setUsername 슬롯이 있다고 가정)
            mainMenuPage->setUsername(username);

            // 3. 스택 위젯을 2번(메인메뉴) 페이지로 전환
            stackedWidget->setCurrentIndex(2);

            return true;
            // --- [수정 끝] ---
        }
    }

    // 사용자가 없거나 비밀번호가 틀린 경우
    QMessageBox::warning(this, "로그인 실패", "아이디 또는 비밀번호가 올바르지 않습니다.");
    return false;
}
bool Widget::handleChangeUsername(const QString& newUsername)
{
    if (newUsername == this->currentUsername) {
        QMessageBox::warning(this, "오류", "현재 아이디와 동일합니다.");
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE users SET username = :newUsername WHERE username = :oldUsername");
    query.bindValue(":newUsername", newUsername);
    query.bindValue(":oldUsername", this->currentUsername);

    if (!query.exec()) {
        qWarning() << "Change Username Error: " << query.lastError().text();
        QMessageBox::warning(this, "변경 실패", "아이디가 중복되거나 DB 오류가 발생했습니다.");
        return false;
    }

    QMessageBox::information(this, "변경 성공", "아이디가 성공적으로 변경되었습니다.");

    // [중요] 현재 사용자 정보 갱신
    this->currentUsername = newUsername;

    // [중M] 다른 위젯들의 정보도 갱신
    mainMenuPage->setUsername(newUsername);
    myPage->setCurrentUsername(newUsername);

    return true;
}

// ************************************************
// ** [추가] 계정 삭제 DB 로직
// ************************************************
bool Widget::handleDeleteAccount()
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE username = :username");
    query.bindValue(":username", this->currentUsername);

    if (!query.exec()) {
        qWarning() << "Delete Account Error: " << query.lastError().text();
        QMessageBox::warning(this, "삭제 실패", "계정 삭제 중 오류가 발생했습니다.");
        return false;
    }

    QMessageBox::information(this, "계정 삭제", "계정이 성공적으로 삭제되었습니다.");

    // [중요] 계정 삭제 시 로그아웃 처리
    this->currentUsername = "";
    mainMenuPage->setUsername("GUEST");
    loginPage->clearFields();
    stackedWidget->setCurrentIndex(0); // 0번(로그인) 페이지로 이동

    return true;
}

#ifndef WIDGET_H
#define WIDGET_H
#include "UI/mypagewidget.h"
#include <QWidget>
#include <QResizeEvent> // 1. 리사이즈 이벤트 include

// SQL
#include <QSqlDatabase>     // DB 연결
#include <QSqlQuery>        // SQL 쿼리 실행
#include <QSqlError>        // 에러 확인
#include <QDebug>           // 디버깅용
#include <QCryptographicHash> // 비밀번호 해싱
#include <QMessageBox>      // 사용자 알림

// 전방 선언
class QStackedWidget;
class LoginWidget;    // 2. 로그인 페이지 (곧 만듦)
class SignupWidget; // 3. 회원가입 페이지 (곧 만듦)
class MainMenuWidget;
class RankingWidget;
class GameWidget;

/**
 * @brief 메인 윈도우. 배경화면과 페이지 전환(스택)을 관리합니다.
 */
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    // 배경 리사이즈를 위해 이 함수는 메인 창이 가져갑니다.
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateScaledBackground(); // 배경 업데이트 함수
    void initDatabase();
    bool signUp(const QString& username, const QString& password);
    bool login(const QString& username, const QString& password);
    bool handleChangeUsername(const QString& newUsername);
    bool handleDeleteAccount();

    QStackedWidget *stackedWidget; // 페이지를 담을 스택
    LoginWidget    *loginPage;     // 로그인 페이지
    SignupWidget   *signupPage;    // 회원가입 페이지
    MainMenuWidget   *mainMenuPage;    // [수정] 메인메뉴 페이지
    RankingWidget *rankingWidget;
    MyPageWidget   *myPage;

    GameWidget  *game;


    QString currentUsername;
};
#endif // WIDGET_H

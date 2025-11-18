#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

// 전방 선언
class QLineEdit;
class QLabel;
class QPushButton;

/**
 * @brief 로그인 전용 페이지 (기존 Widget 클래스의 UI)
 */
class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

public slots:
    void clearFields(); // [추가] 필드 초기화 슬롯

signals:
    // '회원가입' 버튼을 누르면 메인 창에 알림
    void signupClicked();
    // "로그인" 버튼 클릭 시 (DB 로직 실행용)
    void loginRequested(const QString& username, const QString& password);

private slots:
    // '회원가입' 버튼 클릭 시 내부적으로 호출될 함수
    void onSignupButtonClicked();

private:
    void initUI();
    void loadStyles();

    // UI 멤버 변수
    QLabel *titleLabel;
    QLineEdit *idLineEdit;
    QLineEdit *passwordLineEdit;
    QLabel *subtitleLabel;
    QPushButton *loginButton;
    QPushButton *signupButton;
    QPushButton *quitButton;
};

#endif // LOGINWIDGET_H

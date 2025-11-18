#ifndef SIGNUPWIDGET_H
#define SIGNUPWIDGET_H

#include <QWidget> // QDialog가 아닌 QWidget

// 전방 선언
class QLabel;
class QLineEdit;
class QPushButton;
class QFrame;

/**
 * @brief 회원가입 전용 페이지 (기존 SignupDialog 클래스의 UI)
 */
class SignupWidget : public QWidget // QDialog가 아닌 QWidget
{
    Q_OBJECT

public:
    explicit SignupWidget(QWidget *parent = nullptr);
    ~SignupWidget();

signals:
    // 'Back' 버튼을 누르면 메인 창에 알림
    void backClicked();
    void signupRequested(const QString& username, const QString& password);

private slots:
    // 'Back' 버튼 클릭 시 내부적으로 호출될 함수
    void onBackButtonClicked();
    void onSignupButtonClicked();

private:
    void initUI();
    void loadStyles();

    // UI 멤버 변수
    QLabel *titleLabel;
    QFrame *mainPanel;
    QLineEdit *idEdit;
    QLineEdit *passEdit;
    QLineEdit *passConfirmEdit;
    QPushButton *activateButton;
    QPushButton *backButton;
};

#endif // SIGNUPWIDGET_H

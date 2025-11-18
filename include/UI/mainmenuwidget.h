#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>

// 전방 선언 (Forward Declarations)
class QLabel;
class QPushButton;
class QFrame;
class QWidget; // menuPanel용

/**
 * @brief 메인 메뉴 페이지 (LoginWidget 레이아웃 기반)
 */
class MainMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
    ~MainMenuWidget();

public slots:
    // 부모(Widget)가 로그인 성공 시 호출할 슬롯
    void setUsername(const QString& username);

signals:
    // "게임시작", "랭킹" 버튼용 시그널
    void startGameClicked();
    void rankingClicked();
    void logoutRequested();
    void myPageClicked();

private:
    void initUI();
    void loadStyles();

    // --- UI 멤버 변수 (LoginWidget과 거의 동일한 구조) ---

    // 상단 타이틀
    QLabel *titleLabel;
    QLabel *subtitleLabel;

    // 중간 왼쪽 패널 (로그인 UI 대신 환영 메시지)
    QFrame *welcomePanel;
    QLabel *welcomeLabel;

    // 중간 오른쪽 패널 (로그인 버튼 대신 새 메뉴)
    QWidget *menuPanel;
    QPushButton *startButton;
    QPushButton *rankingButton;
    QPushButton *exitButton;
    QPushButton *logoutButton;
    QPushButton *myPageButton;

};

#endif // MAINMENUWIDGET_H

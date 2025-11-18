#ifndef MYPAGEWIDGET_H
#define MYPAGEWIDGET_H

#include <QWidget>

// 전방 선언
class QLabel;
class QLineEdit;
class QPushButton;
class QFrame; // 패널 프레임

class MyPageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyPageWidget(QWidget *parent = nullptr);

public slots:
    void setCurrentUsername(const QString& username);
    void setHighScore(int score);

signals:
    void backClicked(); // [다시 추가] 뒤로가기 시그널
    void changeUsernameRequested(const QString& newUsername);
    void deleteAccountRequested();

private slots:
    void onChangeButtonClicked();

private:
    void initUI();
    void loadStyles(); // 폰트 및 스타일 적용

    QLabel *titleLabel; // "MY PAGE"

    QFrame *infoPanel; // 오른쪽에 보이는 반투명 패널
    QLabel *currentUsernameLabel; // [수정] "ID : VEDA"를 표시할 라벨
    QLabel *highScoreLabel; // "HIGH SCORE : 9999"
    QLineEdit *newUsernameEdit; // "Enter new ID"
    QPushButton *changeUsernameButton; // "변경" 버튼
    QPushButton *deleteAccountButton; // "계정 삭제" 버튼
    QPushButton *backButton; // [다시 추가] 뒤로가기 버튼

    QString currentLoadedUsername;
};

#endif // MYPAGEWIDGET_H

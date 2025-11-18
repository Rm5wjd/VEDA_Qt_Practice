#ifndef RANKINGWIDGET_H
#define RANKINGWIDGET_H

#include <QWidget>
#include <QVector>

// 전방 선언
class QLabel;
class QPushButton;
class QFrame;

/**
 * @brief 랭킹 화면 페이지
 */
class RankingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RankingWidget(QWidget *parent = nullptr);
    ~RankingWidget();

public slots:
    // 부모(Widget)가 호출하여 랭킹을 불러오고 화면을 업데이트하는 함수
    void loadRanks(const QString& currentUsername);

signals:
    // "뒤로가기" 버튼용 시그널
    void backClicked();

private:
    void initUI();
    void loadStyles();

    // UI 멤버 변수
    QLabel *rankingTitle;
    QLabel *topCommanderLabel; // "TOP COMMANDER" 텍스트
    QLabel *topCommanderName;  // 1위 이름 라벨
    QLabel *crownIcon;
    QFrame *rankPanel;         // 오른쪽 랭킹 목록 패널
    QFrame *topCommanderFrame;

    // 랭킹 목록 (2위부터 10위까지의 이름을 표시할 라벨)
    QVector<QLabel*> rankNameLabels;

    // 뒤로가기 버튼 (화면 전환용)
    QPushButton *backButton;
};

#endif // RANKINGWIDGET_H

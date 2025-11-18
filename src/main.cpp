#include <QApplication>
#include "UI/widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 메인 UI
    Widget mainW;
    mainW.setWindowTitle("Missile Command");
    mainW.show();

    // 메인 게임 위젯 생성 및 표시
    //GameWidget w;
    //w.setWindowTitle("Missile Command");
    //w.show(); // 위젯을 화면에 보임

    return a.exec();
}

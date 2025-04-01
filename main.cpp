#include <QApplication>
#include "Window.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Window window = Window();
    window.show();
    return app.exec();
}

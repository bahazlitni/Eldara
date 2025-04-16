#include <QApplication>
#include "Window.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication scene(argc, argv);

    Window window = Window();
    window.show();
    return scene.exec();
}

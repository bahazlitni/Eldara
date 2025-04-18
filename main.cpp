#include <QApplication>
#include "Window.h"

int main(int argc, char *argv[]) {
    QApplication scene(argc, argv);

    Window window = Window();
    window.show();
    return scene.exec();
}

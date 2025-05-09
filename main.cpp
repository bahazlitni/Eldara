#include <QApplication>
#include "Window.h" // Include your main window header
#include <QDebug> // For debugging

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    Window mainWindow;
    mainWindow.show();
    int result = a.exec();
    return result;
}

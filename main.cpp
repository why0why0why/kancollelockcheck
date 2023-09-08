#include "mainwindow.h"

#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory shared("kancollelockcheck");
    if(shared.attach())
    {
        QMessageBox::information(NULL, QStringLiteral("Warning"), QStringLiteral("软件已打开!"), QMessageBox::Yes, QMessageBox::Yes);
        return -1;
    }
    shared.create(1);
    MainWindow w;
    w.show();
    return a.exec();
}

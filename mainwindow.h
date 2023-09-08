#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include "xlsxdocument.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initView();
    void connections();

    void on_btnxlsxPath_clicked();
    void on_btnCheck_clicked();

    QLabel *labelxlsx;
    QLineEdit *editPathxlsx;
    QPushButton *btnxlsxPath, *btnCheck;
};
#endif // MAINWINDOW_H

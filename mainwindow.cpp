#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QVector>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    initView();
    connections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initView()
{
    auto mainWidget = new QWidget;
    setCentralWidget(mainWidget);

    labelxlsx = new QLabel(tr("xlsx路径："), this);
    editPathxlsx = new QLineEdit(this);
    editPathxlsx->setMinimumWidth(300);
    btnxlsxPath = new QPushButton(tr("..."), this);
    btnxlsxPath->setMaximumWidth(30);
    btnCheck = new QPushButton(tr("检查"), this);

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(labelxlsx);
    auto hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(editPathxlsx);
    hLayout1->addWidget(btnxlsxPath);
    vLayout->addLayout(hLayout1);
    vLayout->addStretch();
    auto hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(btnCheck);
    hLayout2->addStretch();
    vLayout->addLayout(hLayout2);
    mainWidget->setLayout(vLayout);
}

void MainWindow::connections()
{
    connect(btnxlsxPath, &QPushButton::clicked, this, &MainWindow::on_btnxlsxPath_clicked);
    connect(btnCheck, &QPushButton::clicked, this, &MainWindow::on_btnCheck_clicked);
}

void MainWindow::on_btnxlsxPath_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("选择xlsx"), ".",tr("Excel文件 (*.xlsx)"));
    if (!filepath.isEmpty())
    {
        editPathxlsx->setText(filepath);
    }
}

void MainWindow::on_btnCheck_clicked()
{
    QFileInfo xlsxPath(editPathxlsx->text());
    if (!xlsxPath.exists())
    {
        QMessageBox::information(NULL, QStringLiteral("Warning"), QStringLiteral("路径不存在或为空!"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    QXlsx::Document xlsx(xlsxPath.absoluteFilePath());

    int rowCount = xlsx.dimension().lastRow();
    int colCount = xlsx.dimension().lastColumn();

    auto tmpStr = xlsx.cellAt(3, 11)->value().toString();
    auto tmpColor = xlsx.cellAt(3, 11)->format().patternBackgroundColor();
}

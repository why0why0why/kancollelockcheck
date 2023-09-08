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
#include <QRegularExpression>
#include <QMap>
#include <QList>

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
    QString filepath = QFileDialog::getOpenFileName(this, tr("选择xlsx"), ".", tr("Excel文件 (*.xlsx)"));
    if (!filepath.isEmpty())
    {
        editPathxlsx->setText(filepath);
    }
}

static QString colToLetters(int col)
{
    QString letters;
    while (col > 0)
    {
        int rem = (col - 1) % 26;
        letters.prepend(QChar('A' + rem));
        col = (col - 1) / 26;
    }
    return letters;
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
    auto dim = xlsx.dimension();
    if (!dim.isValid())
    {
        QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Excel 文件无有效单元格。"));
        return;
    }

    int rowCount = dim.lastRow();
    int colCount = dim.lastColumn();

    struct CellInfo
    {
        int row;
        int col;
        QColor color;
    };
    QMap<QString, QList<CellInfo>> mapTextCells;

    QRegularExpression skipRe("^[EP]\\d+$", QRegularExpression::CaseInsensitiveOption);

    for (int r = 1; r <= rowCount; ++r)
    {
        for (int c = 1; c <= colCount; ++c)
        {
            auto cell = xlsx.cellAt(r, c);
            if (!cell)
                continue;
            QString text = cell->value().toString();
            if (text.isEmpty())
                continue;
            if (skipRe.match(text).hasMatch())
                continue;
            QColor bg = cell->format().patternBackgroundColor();
            mapTextCells[text].append({r, c, bg});
        }
    }

    QStringList problems;
    for (auto it = mapTextCells.constBegin(); it != mapTextCells.constEnd(); ++it)
    {
        const QString &text = it.key();
        const QList<CellInfo> &cells = it.value();
        if (cells.size() <= 1)
            continue;

        QMap<QString, QList<CellInfo>> colorGroups;
        for (const CellInfo &ci : cells)
        {
            QString colorKey = ci.color.isValid() ? ci.color.name(QColor::HexRgb) : QStringLiteral("none");
            colorGroups[colorKey].append(ci);
        }
        if (colorGroups.size() > 1)
        {
            QStringList parts;
            for (auto cit = colorGroups.constBegin(); cit != colorGroups.constEnd(); ++cit)
            {
                QString colorKey = cit.key();
                const QList<CellInfo> &lst = cit.value();
                QStringList posList;
                for (const CellInfo &ci : lst)
                {
                    QString cellRef = colToLetters(ci.col) + QString::number(ci.row);
                    posList.append(QString("%1").arg(cellRef));
                }
                parts.append(QString("%1: %2").arg(colorKey, posList.join(", ")));
            }
            problems.append(QStringLiteral("\"%1\" 在不同位置冲突：\n%2").arg(text, parts.join("\n")));
        }
    }

    if (!problems.isEmpty())
    {
        QString msg = problems.join("\n\n");
        QMessageBox::critical(this, QStringLiteral("存在冲突"), msg);
    }
    else
    {
        QMessageBox::information(this, QStringLiteral("检查通过"), QStringLiteral("未发现冲突。"));
    }
}

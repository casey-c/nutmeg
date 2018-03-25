#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = new Canvas();
    ui->mainGrid->addWidget(canvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}

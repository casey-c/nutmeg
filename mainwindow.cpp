#include "canvas.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(new Canvas(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

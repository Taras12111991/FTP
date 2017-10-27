#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpserver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FtpServer *server = new FtpServer(this);
    setCentralWidget(server);

    setWindowTitle("FTP Server");

    resize(700, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

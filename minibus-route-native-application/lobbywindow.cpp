#include "lobbywindow.h"
#include "ui_lobbywindow.h"

LobbyWindow::LobbyWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LobbyWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget( &login_form_ );
    ui->stackedWidget->addWidget( &verify_member_form_ );
}

LobbyWindow::~LobbyWindow()
{
    delete ui;
}


#include "LoginView.h"


LoginView::LoginView(QMainWindow* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.loginPushButton, SIGNAL(released()), this, SLOT(handleLoginButtonClicked()));
	connect(ui.cancelPushButton, SIGNAL(released()), this, SLOT(handleCancelButtonClicked()));

}

LoginView::~LoginView()
{
}

void LoginView::handleLoginButtonClicked()
{
	emit NotifyLoginButtonClicked(ui.usernameLineEdit->text(), ui.passwordLineEdit->text());
}

void LoginView::handleCancelButtonClicked()
{
	ui.usernameLineEdit->clear();
	ui.passwordLineEdit->clear();
}
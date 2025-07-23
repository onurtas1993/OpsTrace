#pragma once

#include <QtWidgets/QMainWindow> 
#include "ui_LoginView.h"

class LoginView : public QMainWindow
{
	Q_OBJECT

public:
	LoginView(QMainWindow* parent = Q_NULLPTR);
	~LoginView();

private slots:
	void handleLoginButtonClicked();
	void handleCancelButtonClicked();

signals:
	void NotifyLoginButtonClicked(QString username, QString password);

private:
	Ui::LoginView ui;
};

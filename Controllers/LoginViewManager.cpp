#include "LoginViewManager.h" 

#include "MainViewManager.h"  
#include "../Views/LoginView.h" 
#include "../Views/MainView.h" 
#include "../Threads/NetworkThread.h" 

LoginViewManager::LoginViewManager(QObject* parent,
	LoginView& view) :
	QObject(parent),
	view(view)
{
	networkThread = new QThread;
	networkThreadWorker = new NetworkThread();
	networkThreadWorker->moveToThread(networkThread);
	networkThread->start();

	connect(&view, &LoginView::NotifyLoginButtonClicked, networkThreadWorker, &NetworkThread::startLoggingInProcess);
	connect(networkThreadWorker, &NetworkThread::SessionCreated, this, &LoginViewManager::onLoginSuccessful);
	connect(networkThreadWorker, &NetworkThread::LoginFailed, this, &LoginViewManager::onLoginFailed);

}

LoginViewManager::~LoginViewManager()
{

}

void LoginViewManager::onLoginSuccessful(QJsonObject data)
{
	MainView* mainView = new MainView(data, NULL);
	MainViewManager* mainViewManager = new MainViewManager(this, *mainView, data);
	mainView->show();
	view.close();
}

void LoginViewManager::onLoginFailed(QString msg)
{
	//"could not authorize"
	QMessageBox msgBox;
	msgBox.setWindowTitle("Error occured while logging in.");
	msgBox.setText(msg);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.exec();
}

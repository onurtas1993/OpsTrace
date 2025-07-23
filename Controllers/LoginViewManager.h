#pragma once

#include <QObject> 
#include <QJsonObject>

class LoginView;
class NetworkThread;

class LoginViewManager : public QObject
{
	Q_OBJECT
public:
	explicit LoginViewManager(QObject* parent, LoginView& view);
	~LoginViewManager();

public slots:
	void onLoginSuccessful(QJsonObject);
	void onLoginFailed(QString);

private:
	LoginView& view;

	QThread* networkThread;
	NetworkThread* networkThreadWorker;

	explicit LoginViewManager(const LoginViewManager& rhs);
};

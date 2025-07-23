#pragma once
#include <QObject>

class LoginView;
class LoginViewManager;

class Startup : public QObject
{
	Q_OBJECT
public:
	explicit Startup();
	void show() const;
	~Startup();

private:
	LoginView& loginView;
	LoginViewManager* loginViewManager;

};




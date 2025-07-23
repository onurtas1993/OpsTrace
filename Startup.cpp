#include "Startup.h"
#include "Views/LoginView.h"
#include "Controllers/LoginViewManager.h" 

Startup::Startup() :
	QObject(NULL),
	loginViewManager(new LoginViewManager(this, loginView)),
	loginView(*new LoginView(NULL))
{

}

Startup::~Startup()
{
	delete& loginView;
}

void Startup::show() const
{
	loginView.show();
}

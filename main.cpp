#include <QtWidgets/QApplication>
#include "Startup.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	Startup start_up;

	start_up.show();
	return a.exec();
}

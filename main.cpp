#include "copyfiles.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CopyFilesWindow w;
	w.show();
	return a.exec();
}

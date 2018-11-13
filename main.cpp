#include "copyfiles.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CopyFilesWindow w;
	if (argc == 2)
	{
		w.importFromXml(QString::fromLocal8Bit(argv[1]));
	}
	w.show();
	return a.exec();
}

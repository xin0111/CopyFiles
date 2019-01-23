#include "copyfiles.h"
#include <QtWidgets/QApplication>
#include <QCommandLineParser>

#if defined(COPYFILES_STACTIC)
#include <QtCore/QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCommandLineParser parser;
	const QCommandLineOption fileOption(QStringLiteral("f"), 
		"", QStringLiteral("filepath"));
	parser.addOption(fileOption);
	parser.process(a);
	CopyFilesWindow w;	
	if (parser.isSet(fileOption))
	{
		const QString filePath = parser.value(fileOption);
		w.importFromXml(filePath);
	}
	w.show();
	return a.exec();
}

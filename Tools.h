#pragma once
#include <QStringList>
#include <QListWidget>
class QDomDocument;
class CTools
{
public:
	enum emCopyError
	{
		NON_EXISTENT,
		UNABLE_CREATE,
		COPY_FAILED,
		EMPTY_RULE,
		ERROR_REGEX
	};
	CTools();
	~CTools();
	static int CalcNextIndex(int nCount, const QStringList& showLists);
	static 	bool copyFileToPath(QString sourceDir, QString toDir,
		QString& errorMsg=QString(), bool coverFileIfExist = true);
	static bool openXml(QDomDocument& doc, const QString& filePath);
    static bool saveXml(QDomDocument& doc, const QString& filePath);
	static QString copyErrorMsg(emCopyError errorType, QString filePath);
};


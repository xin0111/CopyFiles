#ifndef COPYTHREAD_H
#define COPYTHREAD_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QVector>
#include <QString>
#include "Tools.h"

class CopyThread : public QThread
{
	Q_OBJECT
public:
	static CopyThread* getInstance();
private:
	CopyThread();
	~CopyThread();
public:
	void AddFileHash(QString strFrom, QStringList listTo);
	void AddFileRules(QString strFrom, QStringList listTo);
	void ResetFileRules();	
protected:
	virtual void run();
signals:	
	void sig_copyRuleCount(int nCount);
	void sig_copyFromItem(QString filePath);
	void sig_copyFinished(bool bSuccessed, QString strMsg);
	void sig_copyError(QString filePath);
private:
	bool copyDirectoryRules(const QString &fromDir, const QString &toDir, 
		bool addRoot = false, QString toRootDirName = QString(), bool findChildDir = true);
	void setErrorString(CTools::emCopyError errorType, QString filePath);
	bool hasCopyError();
public:
	QHash<QString, QStringList> m_fileHash;
	QVector<QStringList> m_fileRules; //From#To
	QString m_strError;
	QRegExp m_regFile;
	QString m_ruleFilePath;
};


#endif // COPYTHREAD_H

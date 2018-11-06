#ifndef COPYTHREAD_H
#define COPYTHREAD_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QVector>
#include <QString>

struct CopyRuleInfo
{
	QString strFrom_;
	QString strTo_;
	CopyRuleInfo()
	{

	}
	CopyRuleInfo(QString strFrom, QString strTo)
	{
		strFrom_ = strFrom;
		strTo_ = strTo;
	}
};
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
	enum CopyError
	{
		NON_EXISTENT,
		UNABLE_CREATE,
		COPY_FAILED,
		EMPTY_RULE,
		ERROR_REGEX
	};
	bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist=true);		
	bool copyDirectoryRules(const QString &fromDir, const QString &toDir, 
		bool addRoot = false, QString toRootDirName = QString(), bool findChildDir = true);
	void setErrorString(CopyError errorType, QString filePath);
public:
	QHash<QString, QStringList> m_fileHash;
	QVector<CopyRuleInfo> m_fileRules;
	bool   m_hasError;
	QString m_strError;
	QRegExp m_regFile;
};


#endif // COPYTHREAD_H

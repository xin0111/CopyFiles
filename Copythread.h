#ifndef COPYTHREAD_H
#define COPYTHREAD_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QString>

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
	void ResetFileHash();
protected:
	virtual void run();
signals:	
	void sig_copyFromItem(QString filePath);
	void sig_copyFinished(bool bSuccessed);
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
	bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, 
		bool coverFileIfExist = true, bool addRoot = false);
	void setErrorString(CopyError errorType, QString filePath);
public:
	QHash<QString, QStringList> m_fileHash;
	bool   m_hasError;
	QString m_strError;
	QRegExp m_regFile;
};


#endif // COPYTHREAD_H

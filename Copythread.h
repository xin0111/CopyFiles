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
	void sig_copySuccessed();
	void sig_errorfilePath(QString filePath);
private:
	bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist=true);	
	bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, 
		bool coverFileIfExist = true, bool isRoot=false);
public:
	QHash<QString, QStringList> m_fileHash;
};


#endif // COPYTHREAD_H

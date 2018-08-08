#include "Copythread.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

CopyThread* CopyThread::getInstance()
{
	static CopyThread cthread;
	return &cthread;
}

CopyThread::CopyThread()
{

}

CopyThread::~CopyThread()
{

}

void CopyThread::run()
{
	QHashIterator<QString, QStringList> i(m_fileHash);
	QString strCopyFrom;
	
	QStringList copyToList;
	while (i.hasNext()) {
		i.next();
		strCopyFrom = i.key();
		copyToList = i.value();
		QFileInfo fromFileInfo(strCopyFrom);
		for (size_t i = 0; i < copyToList.size(); i++)
		{
			if (fromFileInfo.isFile())
				copyFileToPath(strCopyFrom, copyToList.at(i));
			else if (fromFileInfo.isDir())
				copyDirectoryFiles(strCopyFrom, copyToList.at(i), true,true);
			else
				sig_errorfilePath(strCopyFrom);
		}		
	}
	sig_copySuccessed();
}
//拷贝文件：
bool CopyThread::copyFileToPath(QString sourceDir,
		QString toDir, bool coverFileIfExist)
{
	toDir.replace("\\", "/");
	if (sourceDir == toDir){
		return true;
	}
	if (!QFile::exists(sourceDir)){
		return false;
	}
	QFileInfo sourceInfo(sourceDir);	
	QString toDirFile = toDir;
	toDirFile.append("/");
	toDirFile.append(sourceInfo.fileName());
	QDir createfile;
	bool exist = createfile.exists(toDirFile);
	if (exist){
		if (coverFileIfExist){
			createfile.remove(toDirFile);
		}
	}
	else
	{			
		if (!createfile.mkpath(toDir))
			return false;
	}

	if (!QFile::copy(sourceDir, toDirFile))
	{
		return false;
	}
	return true;
}

//拷贝文件夹：
bool CopyThread::copyDirectoryFiles(const QString &fromDir,
	const QString &toDir, bool coverFileIfExist, bool isRoot)
{
	QDir sourceDir(fromDir);	
	if (!sourceDir.exists()){
		return false;
	}
	QString newToDir = toDir;	
	if (isRoot)
		newToDir.append("/").append(sourceDir.dirName());	
	QDir targetDir(newToDir);
	if (!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
		if (!targetDir.mkpath(targetDir.absolutePath()))
			return false;
	}

	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
			if (!copyDirectoryFiles(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()),
				coverFileIfExist))
				return false;
		}
		else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())){
				targetDir.remove(fileInfo.fileName());
			}
			/// 进行文件copy
			if (!QFile::copy(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()))){
				return false;
			}
		}
	}
	return true;
}

void CopyThread::AddFileHash(QString strFrom, QStringList listTo)
{
	m_fileHash[strFrom] = listTo;
}

void CopyThread::ResetFileHash()
{
	m_fileHash.clear();
}

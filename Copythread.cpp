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
//�����ļ���
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

//�����ļ��У�
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
	if (!targetDir.exists()){    /**< ���Ŀ��Ŀ¼�����ڣ�����д��� */
		if (!targetDir.mkpath(targetDir.absolutePath()))
			return false;
	}

	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()){    /**< ��ΪĿ¼ʱ���ݹ�Ľ���copy */
			if (!copyDirectoryFiles(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()),
				coverFileIfExist))
				return false;
		}
		else{            /**< �������ǲ���ʱ�������ļ�����ɾ������ */
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())){
				targetDir.remove(fileInfo.fileName());
			}
			/// �����ļ�copy
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

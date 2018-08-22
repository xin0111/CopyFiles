#include "Copythread.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

CopyThread* CopyThread::getInstance()
{
	static CopyThread cthread;
	return &cthread;
}

CopyThread::CopyThread()
{
	m_hasError = false;
}

CopyThread::~CopyThread()
{

}

void CopyThread::run()
{
	if (m_fileHash.isEmpty())	{
		setErrorString(EMPTY_RULE, "");
	}
	else
	{
		QHashIterator<QString, QStringList> i(m_fileHash);
		QString strCopyFrom;
		QStringList copyToList;
		while (i.hasNext()) {
			i.next();
			strCopyFrom = i.key();
			copyToList = i.value();
			sig_copyFromItem(strCopyFrom);
			QFileInfo fromFileInfo(strCopyFrom);
			m_regFile = QRegExp();
			if(!fromFileInfo.isFile() && !fromFileInfo.suffix().isEmpty())
			{//��������
				QString strReg = fromFileInfo.fileName();
				qDebug() << strReg;
				m_regFile = QRegExp(".*(\\|/)"+strReg);		
				if(!m_regFile.isValid())
					setErrorString(ERROR_REGEX, strCopyFrom);
				strCopyFrom = fromFileInfo.dir().path();	
				fromFileInfo.setFile(strCopyFrom);
			}
			for (size_t i = 0; i < copyToList.size(); i++)
			{
				if (fromFileInfo.isFile())
					copyFileToPath(strCopyFrom, copyToList.at(i));
				else if (fromFileInfo.isDir())
					copyDirectoryFiles(strCopyFrom, copyToList.at(i), true, false);
				else
				{
					setErrorString(NON_EXISTENT, strCopyFrom);
				}

				if (m_hasError)	{
					break;
				}
			}
		}
	}	
	emit sig_copyFinished(!m_hasError);
	if (m_hasError) {
		sig_copyError(m_strError);
	}
	m_hasError = false;
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
		setErrorString(NON_EXISTENT, sourceDir);
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
		{
			setErrorString(UNABLE_CREATE, toDir);
			return false;
		}			
	}

	if (!QFile::copy(sourceDir, toDirFile))
	{
		setErrorString(COPY_FAILED, sourceDir);
		return false;
	}
	return true;
}

//�����ļ��У�
bool CopyThread::copyDirectoryFiles(const QString &fromDir,
	const QString &toDir, bool coverFileIfExist, bool addRoot)
{
	QDir sourceDir(fromDir);	
	if (!sourceDir.exists()){
		setErrorString(NON_EXISTENT, fromDir);
		return false;
	}
	QString newToDir = toDir;	
	if (addRoot)
		newToDir.append("/").append(sourceDir.dirName());
	QDir targetDir(newToDir);
	if (!targetDir.exists()){    /**< ���Ŀ��Ŀ¼�����ڣ�����д��� */
		if (!targetDir.mkpath(targetDir.absolutePath()))
		{
			setErrorString(UNABLE_CREATE, targetDir.absolutePath());
			return false;
		}
	}
	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;
		qDebug()<<fileInfo.filePath();
		//����ƥ��
		if(!m_regFile.isEmpty() && (
			fileInfo.isDir() ||
			!m_regFile.exactMatch(fileInfo.filePath())))
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
				setErrorString(COPY_FAILED, fileInfo.filePath());
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

void CopyThread::setErrorString(CopyError errorType, QString filePath)
{
	m_strError.clear();
	switch (errorType)
	{
	case CopyThread::NON_EXISTENT:
		m_strError = QString::fromLocal8Bit("�������ļ�(��)\n");
		break;
	case CopyThread::UNABLE_CREATE:
		m_strError = QString::fromLocal8Bit("�޷������ļ���\n");
		break;
	case CopyThread::COPY_FAILED:
		m_strError = QString::fromLocal8Bit("�����ļ�ʧ��\n");
		break;
	case CopyThread::EMPTY_RULE:
		m_strError = QString::fromLocal8Bit("��������ȷ���ƹ���!\n");
		break;
	case CopyThread::ERROR_REGEX:
		m_strError = QString::fromLocal8Bit("�������!\n");
		break;
	default:
		break;
	}	
	m_strError.append(filePath);
	m_hasError = true;
}

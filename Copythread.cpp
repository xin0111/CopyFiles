#include "Copythread.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QDateTime>

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
			//���ҹ���
			AddFileRules(strCopyFrom, copyToList);
			if (m_hasError)	break;
		}
		if (!m_hasError)
		{
			sig_copyRuleCount(m_fileRules.size());
			//�����ļ�
			for each (CopyRuleInfo var in m_fileRules)
			{
				sig_copyFromItem(var.strFrom_);
				copyFileToPath(var.strFrom_, var.strTo_, true);
				if (m_hasError)	break;
			}
		}
	}	
	sig_copyFinished(!m_hasError, m_strError);
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
		QFileInfo toInfo(toDirFile);
		if (toInfo.lastModified() == sourceInfo.lastModified())
		{//δ���£�������
			return true;
		}	
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

bool CopyThread::copyDirectoryRules(const QString &fromDir, const QString &toDir,
	bool addRoot, QString toRootDirName, bool findChildDir)
{
	QDir sourceDir(fromDir);
	if (!sourceDir.exists()){
		setErrorString(NON_EXISTENT, fromDir);
		return false;
	}
	QString newToDir = toDir;
	if (addRoot)		
		newToDir.append("/").append(toRootDirName.isEmpty()?
		sourceDir.dirName() : toRootDirName);

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
		qDebug() << fileInfo.filePath();
		if (fileInfo.isDir()){    /**< ��ΪĿ¼ʱ���ݹ�Ľ���copy */		
			if (!findChildDir)
				continue;
			if (!copyDirectoryRules(fileInfo.filePath(),
				!m_regFile.isEmpty() ? newToDir : targetDir.filePath(fileInfo.fileName())))
				return false;
		}
		else{
			//����ƥ��
			if (!m_regFile.isEmpty() && (
				!m_regFile.exactMatch(fileInfo.filePath())))
				continue;

			//�洢����
			m_fileRules.push_back(CopyRuleInfo(fileInfo.filePath(), newToDir));
		}
	}
	return true;
}

void CopyThread::AddFileHash(QString strFrom, QStringList listTo)
{
	m_fileHash[strFrom] = listTo;
}

void CopyThread::AddFileRules(QString strFrom, QStringList listTo)
{
	if (QDir::isRelativePath(strFrom))
	{//���·�� ���� 
		strFrom = m_ruleFilePath + strFrom;
	}	
	QFileInfo fromFileInfo(strFrom);
	
	for (size_t i = 0; i < listTo.size(); i++)
	{
		m_regFile = QRegExp();
		bool bAddRoot = false;
		QString toRootDirName;
		bool bFindChirdDir = true;

		QString strReg = fromFileInfo.fileName();
		qDebug() << strReg;
		if (strReg.indexOf("*") != -1||
			strReg.indexOf(">") != -1)
		{//��������				
			if (strReg.split("-").size() >= 2)
			{//�����Ƿ�ƥ����Ŀ¼
				bFindChirdDir = false;
				strReg = strReg.split("-")[0];
			}
			else if (strReg.split("+").size() >= 2)
			{//�Ƿ񴴽�From��Ŀ¼
				bAddRoot = true;
			}
			else if (strReg.split(">").size() >= 2)
			{//���� �¸�Ŀ¼
				bAddRoot = true;
				QStringList findList = QString(strReg).split(">");
				if (findList.size() >= 2)
				{
					toRootDirName = findList.at(1);
					strReg = findList.at(0);
				}
			}
			if (!strReg.isEmpty() && strReg != ("*.+"))
			{
				m_regFile = QRegExp(".*(\|/)" + strReg);
			}			
			if (!m_regFile.isValid())
				setErrorString(ERROR_REGEX, strFrom);

			strFrom = fromFileInfo.dir().path();
			fromFileInfo.setFile(strFrom);
		}
		
		if (m_hasError)	break;
		
		if (fromFileInfo.isFile())
		{//�洢����
			m_fileRules.push_back(CopyRuleInfo(strFrom, listTo.at(i)));
		}
		else if (fromFileInfo.isDir())
		{
			copyDirectoryRules(strFrom, listTo.at(i), bAddRoot, toRootDirName, bFindChirdDir);
		}
		else
		{
			setErrorString(NON_EXISTENT, strFrom);
		}
		if (m_hasError)	break;
	}
}

void CopyThread::ResetFileRules()
{
	m_fileHash.clear();
	m_fileRules.clear();
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

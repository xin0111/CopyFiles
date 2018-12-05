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
	m_strError.clear();
}

CopyThread::~CopyThread()
{

}

void CopyThread::run()
{
	m_strError.clear();
	if (m_fileHash.isEmpty())	{
		setErrorString(CTools::EMPTY_RULE, "");
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
			if (hasCopyError())	break;
		}
		if (!hasCopyError())
		{
			sig_copyRuleCount(m_fileRules.size());
			//�����ļ�
			for each (QStringList var in m_fileRules)
			{
				sig_copyFromItem(var.at(0));
				CTools::copyFileToPath(var.at(0), var.at(1),m_strError, true);				
				if (hasCopyError())	break;
			}
		}
	}	
	sig_copyFinished(!hasCopyError(), m_strError);
}

bool CopyThread::copyDirectoryRules(const QString &fromDir, const QString &toDir,
	bool addRoot, QString toRootDirName, bool findChildDir)
{
	QDir sourceDir(fromDir);
	if (!sourceDir.exists()){
		setErrorString(CTools::NON_EXISTENT, fromDir);
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
			setErrorString(CTools::UNABLE_CREATE, targetDir.absolutePath());
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
			m_fileRules.push_back(QStringList()<<fileInfo.filePath()<<newToDir);
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
				setErrorString(CTools::ERROR_REGEX, strFrom);

			strFrom = fromFileInfo.dir().path();
			fromFileInfo.setFile(strFrom);
		}
		
		if (hasCopyError())	break;
		
		if (fromFileInfo.isFile())
		{//�洢����
			m_fileRules.push_back(QStringList()<<strFrom <<listTo.at(i));
		}
		else if (fromFileInfo.isDir())
		{
			copyDirectoryRules(strFrom, listTo.at(i), bAddRoot, toRootDirName, bFindChirdDir);
		}
		else
		{
			setErrorString(CTools::NON_EXISTENT, strFrom);
		}
		if (hasCopyError())	break;
	}
}

void CopyThread::ResetFileRules()
{
	m_fileHash.clear();
	m_fileRules.clear();
	m_strError.clear();
}

void CopyThread::setErrorString(CTools::emCopyError errorType, QString filePath)
{
	m_strError = CTools::copyErrorMsg(errorType, filePath);
}

bool CopyThread::hasCopyError()
{
	return !m_strError.isEmpty();
}

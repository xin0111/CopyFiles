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

			strCopyFrom.replace("\\", "/");
			//查找规则
			AddFileRules(strCopyFrom, copyToList);
			if (hasCopyError())	break;
		}
		if (!hasCopyError())
		{
			sig_copyRuleCount(m_fileRules.size());
			//拷贝文件
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
	 bool findChildDir)
{
	QDir sourceDir(fromDir);
	if (!sourceDir.exists()){
		setErrorString(CTools::NON_EXISTENT, fromDir);
		return false;
	}

	QDir targetDir(toDir);

	if (!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
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
		if (fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */		
			if (!findChildDir)
				continue;
			if (!copyDirectoryRules(fileInfo.filePath(),
				!m_regFile.isEmpty() ? toDir : targetDir.filePath(fileInfo.fileName())))
				return false;
		}
		else{
			//正则匹配
			if (!m_regFile.isEmpty() && (
				!m_regFile.exactMatch(fileInfo.filePath())))
				continue;

			//存储规则
			m_fileRules.push_back(QStringList()<<fileInfo.filePath()<<toDir);
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
	{//相对路径 处理 
		strFrom = m_ruleFilePath + strFrom;
	}	
	QFileInfo fromFileInfo(strFrom);

	for (int i = 0; i < listTo.size(); i++)
	{
		QString strTempForm = strFrom;

		m_regFile = QRegExp();
		bool bAddRoot = false;
		QString toRootDirName;
		bool bFindChirdDir = true;
		int nIndex = strTempForm.indexOf(QRegExp("[*>]"));

		QString suffixReg = nIndex!=-1?
			strTempForm.mid(nIndex, strTempForm.length()):
			"";
		qDebug() << suffixReg;
	
		// 非文件名特殊字符
		if (suffixReg.indexOf("*") != -1||
			suffixReg.indexOf(">") != -1||
			suffixReg.indexOf("?") != -1||
			suffixReg.indexOf(":") != -1 ||
			suffixReg.indexOf("\\") != -1 ||
			suffixReg.indexOf("/") != -1 ||
			suffixReg.indexOf("|") != -1 ||
			suffixReg.indexOf("\"") != -1 
			)
		{//存在正则				
			if (suffixReg.split("-").size() >= 2)
			{//正则是否匹配子目录
				bFindChirdDir = false;
				suffixReg = suffixReg.split("-")[0];
			}
			else if (suffixReg.split("+").size() >= 2)
			{//是否创建From根目录
				bAddRoot = true;				
				auto strDirs = strTempForm.split("/");
				toRootDirName = strDirs[strDirs.length()-2];
			}
			if (suffixReg.split(">").size() >= 2)
			{//创建 新根目录
				bAddRoot = true;
				QStringList findList = QString(suffixReg).split(">");
				if (findList.size() >= 2)
				{
					toRootDirName = findList.at(1);
					suffixReg = findList.at(0);					
				}
			}
			if (!suffixReg.isEmpty() && suffixReg != ("*.+"))
			{
				m_regFile = QRegExp(".*([\\|/])" + suffixReg);
			}			
			if (!m_regFile.isValid())
				setErrorString(CTools::ERROR_REGEX, strTempForm);

			strTempForm = strTempForm.mid(0, nIndex);
			fromFileInfo.setFile(strTempForm);
		}
		
		if (hasCopyError())	break;

		if (fromFileInfo.isFile())
		{//存储规则
			m_fileRules.push_back(QStringList() << strTempForm <<
				copyToDirectory(listTo.at(i), bAddRoot, toRootDirName ));
		}
		else if (fromFileInfo.isDir())
		{
			copyDirectoryRules(strTempForm,
				copyToDirectory(listTo.at(i), bAddRoot, toRootDirName),
				bFindChirdDir);
		}
		else
		{
			setErrorString(CTools::NON_EXISTENT, strTempForm);
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

QString CopyThread::copyToDirectory(const QString &toDir,
	bool addRoot, QString toRootDirName)
{
	QString newToDir = toDir;
	if (addRoot)
			newToDir.append("/").append(toRootDirName);

	return newToDir;
}

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
			//查找规则
			AddFileRules(strCopyFrom, copyToList);
			if (m_hasError)	break;
		}
		if (!m_hasError)
		{
			sig_copyRuleCount(m_fileRules.size());
			//拷贝文件
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

//拷贝文件：
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

bool CopyThread::copyDirectoryRules(const QString &fromDir, const QString &toDir, bool addRoot)
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

	if (!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
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
		if (fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
			if (!copyDirectoryRules(fileInfo.filePath(),
				!m_regFile.isEmpty() ? newToDir : targetDir.filePath(fileInfo.fileName())))
				return false;
		}
		else{
			//正则匹配
			if (!m_regFile.isEmpty() && (
				!m_regFile.exactMatch(fileInfo.filePath())))
				continue;

			//存储规则
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
	QFileInfo fromFileInfo(strFrom);

	for (size_t i = 0; i < listTo.size(); i++)
	{
		m_regFile = QRegExp();
		if (!fromFileInfo.exists() && !fromFileInfo.suffix().isEmpty())
		{
			QString strReg = fromFileInfo.fileName();
			qDebug() << strReg;
			if (strReg.indexOf("*") != -1)
			{//存在正则
				m_regFile = QRegExp(".*(\\|/)" + strReg);
				if (!m_regFile.isValid())
					setErrorString(ERROR_REGEX, strFrom);
				strFrom = fromFileInfo.dir().path();
				fromFileInfo.setFile(strFrom);
			}
		}
		if (m_hasError)	break;
		
		if (fromFileInfo.isFile())
		{//存储规则
			m_fileRules.push_back(CopyRuleInfo(strFrom, listTo.at(i)));
		}
		else if (fromFileInfo.isDir())
		{
			copyDirectoryRules(strFrom, listTo.at(i), false);
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
		m_strError = QString::fromLocal8Bit("不存在文件(夹)\n");
		break;
	case CopyThread::UNABLE_CREATE:
		m_strError = QString::fromLocal8Bit("无法创建文件夹\n");
		break;
	case CopyThread::COPY_FAILED:
		m_strError = QString::fromLocal8Bit("复制文件失败\n");
		break;
	case CopyThread::EMPTY_RULE:
		m_strError = QString::fromLocal8Bit("不存在正确复制规则!\n");
		break;
	case CopyThread::ERROR_REGEX:
		m_strError = QString::fromLocal8Bit("错误规则!\n");
		break;
	default:
		break;
	}	
	m_strError.append(filePath);
	m_hasError = true;
}

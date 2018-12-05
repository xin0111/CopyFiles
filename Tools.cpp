#include "Tools.h"
#include <QRegExp>
#include <QList>
#include <QVariant>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QDomDocument>
#include <QTextStream>

CTools::CTools()
{
}


CTools::~CTools()
{
}

int CTools::CalcNextIndex(int nCount, const QStringList& showLists)
{
	QRegExp rx("\\d+");
	QList<int> nNums;
	for (int i = 0; i < nCount; ++i)
	{
		QString str = showLists.at(i);
		int pos = rx.indexIn(str);
		QString strValue = str.mid(pos);
		nNums << QVariant(strValue).toInt();
	}
	int nValue = -1;
	int nIndex = 0;
	qSort(nNums);
	for (int i = 0; i < nNums.size(); ++i)
	{
		if ((i + 1) != nNums.at(i))
		{
			nValue = i;
			break;
		}
	}
	nIndex = (nValue != -1) ? nValue : nCount;
	return nIndex + 1;
}

bool CTools::copyFileToPath(QString sourceDir, QString toDir, 
	QString& errorMsg/*=QString()*/, bool coverFileIfExist /*= true*/)
{
	toDir.replace("\\", "/");
	if (sourceDir == toDir){
		return true;
	}
	if (!QFile::exists(sourceDir)){
		errorMsg = copyErrorMsg(NON_EXISTENT, sourceDir);
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
		{//未更新，不拷贝
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
			errorMsg = copyErrorMsg(UNABLE_CREATE, toDir);
			return false;
		}
	}
	if (!QFile::copy(sourceDir, toDirFile))
	{
		errorMsg = copyErrorMsg(COPY_FAILED, sourceDir);
		return false;
	}
	return true;
}

bool CTools::openXml(QDomDocument& doc,const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	if (!doc.setContent(&file)) {
		file.close();
		return false;
	}
	file.close();
	return true;
}
bool CTools::saveXml(QDomDocument& doc,const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QFile::Truncate))
		return false;
	QTextStream ts(&file);
	ts.reset();
	doc.save(ts, 4, QDomNode::EncodingFromTextStream);
	file.close();
	return true;
}
QString CTools::copyErrorMsg(emCopyError errorType, QString filePath)
{
	QString errorMsg;
	switch (errorType)
	{
	case NON_EXISTENT:
		errorMsg = QString::fromLocal8Bit("不存在文件(夹)\n");
		break;
	case UNABLE_CREATE:
		errorMsg = QString::fromLocal8Bit("无法创建文件夹\n");
		break;
	case COPY_FAILED:
		errorMsg = QString::fromLocal8Bit("复制文件失败\n");
		break;
	case EMPTY_RULE:
		errorMsg = QString::fromLocal8Bit("不存在正确复制规则!\n");
		break;
	case ERROR_REGEX:
		errorMsg = QString::fromLocal8Bit("错误规则!\n");
		break;
	default:
		break;
	}
	return  errorMsg.append(filePath);
}


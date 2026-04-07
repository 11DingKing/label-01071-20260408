/**
 * @file Logger.cpp
 * @brief 日志管理类实现
 */

#include "Logger.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <iostream>

// 初始化静态成员
Logger* Logger::s_instance = nullptr;

Logger* Logger::instance()
{
    if (!s_instance) {
        s_instance = new Logger();
    }
    return s_instance;
}

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_level(Level::Debug)
    , m_colorOutput(true)
    , m_fileLogEnabled(false)
    , m_maxFileSize(10 * 1024 * 1024) // 10MB
    , m_logFile(nullptr)
    , m_logStream(nullptr)
    , m_initialized(false)
{
}

Logger::~Logger()
{
    shutdown();
}

void Logger::init(bool enableFileLog, const QString &logDir)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_initialized) {
        return;
    }
    
    m_fileLogEnabled = enableFileLog;
    
    // 设置日志目录
    if (logDir.isEmpty()) {
        m_logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (m_logDir.isEmpty()) {
            m_logDir = QDir::homePath() + "/.screenshot-tool";
        }
        m_logDir += "/logs";
    } else {
        m_logDir = logDir;
    }
    
    // 创建日志目录
    QDir dir(m_logDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // 打开日志文件
    if (m_fileLogEnabled) {
        openLogFile();
    }
    
    m_initialized = true;
    
    // 记录初始化信息
    info("Logger", QString("日志系统初始化完成，日志目录: %1").arg(m_logDir));
    info("Logger", QString("应用版本: %1").arg(QCoreApplication::applicationVersion()));
}

void Logger::shutdown()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_initialized) {
        return;
    }
    
    if (m_logStream) {
        m_logStream->flush();
        delete m_logStream;
        m_logStream = nullptr;
    }
    
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
    
    m_initialized = false;
}

void Logger::setLogLevel(Level level)
{
    QMutexLocker locker(&m_mutex);
    m_level = level;
}

Logger::Level Logger::logLevel() const
{
    return m_level;
}

void Logger::setColorOutput(bool enable)
{
    QMutexLocker locker(&m_mutex);
    m_colorOutput = enable;
}

void Logger::setFileLogEnabled(bool enable)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_fileLogEnabled == enable) {
        return;
    }
    
    m_fileLogEnabled = enable;
    
    if (enable && m_initialized && !m_logFile) {
        openLogFile();
    } else if (!enable && m_logFile) {
        if (m_logStream) {
            m_logStream->flush();
            delete m_logStream;
            m_logStream = nullptr;
        }
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
}

void Logger::setMaxFileSize(qint64 maxSize)
{
    QMutexLocker locker(&m_mutex);
    m_maxFileSize = maxSize;
}

void Logger::log(Level level, 
                 const QString &category,
                 const QString &message,
                 const char *file,
                 int line,
                 const char *function)
{
    // 检查日志级别
    if (static_cast<int>(level) < static_cast<int>(m_level)) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    // 格式化消息
    QString coloredMsg = formatMessage(level, category, message, file, line, function, m_colorOutput);
    QString plainMsg = formatMessage(level, category, message, file, line, function, false);
    
    // 输出到控制台
    if (m_colorOutput) {
        std::cout << coloredMsg.toStdString() << std::endl;
    } else {
        std::cout << plainMsg.toStdString() << std::endl;
    }
    
    // 输出到文件
    if (m_fileLogEnabled && m_logStream) {
        rotateLogFileIfNeeded();
        *m_logStream << plainMsg << "\n";
        m_logStream->flush();
    }
    
    // 发送信号
    emit logMessage(level, plainMsg);
}

void Logger::debug(const QString &category, const QString &message)
{
    log(Level::Debug, category, message);
}

void Logger::info(const QString &category, const QString &message)
{
    log(Level::Info, category, message);
}

void Logger::warning(const QString &category, const QString &message)
{
    log(Level::Warning, category, message);
}

void Logger::error(const QString &category, const QString &message)
{
    log(Level::Error, category, message);
}

void Logger::critical(const QString &category, const QString &message)
{
    log(Level::Critical, category, message);
}

QString Logger::logDirectory() const
{
    return m_logDir;
}

QString Logger::levelToString(Level level) const
{
    switch (level) {
    case Level::Debug:    return "DEBUG";
    case Level::Info:     return "INFO ";
    case Level::Warning:  return "WARN ";
    case Level::Error:    return "ERROR";
    case Level::Critical: return "CRIT ";
    default:              return "?????";
    }
}

QString Logger::levelToColor(Level level) const
{
    // ANSI 颜色代码
    switch (level) {
    case Level::Debug:    return "\033[36m";   // 青色
    case Level::Info:     return "\033[32m";   // 绿色
    case Level::Warning:  return "\033[33m";   // 黄色
    case Level::Error:    return "\033[31m";   // 红色
    case Level::Critical: return "\033[35;1m"; // 亮紫色
    default:              return "\033[0m";    // 重置
    }
}

QString Logger::formatMessage(Level level,
                             const QString &category,
                             const QString &message,
                             const char *file,
                             int line,
                             const char *function,
                             bool colored) const
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    
    QString formattedMsg;
    
    if (colored) {
        QString colorCode = levelToColor(level);
        QString resetCode = "\033[0m";
        QString dimCode = "\033[2m";
        
        formattedMsg = QString("%1[%2]%3 %4[%5]%3 %6[%7]%3 %8")
            .arg(dimCode)
            .arg(timestamp)
            .arg(resetCode)
            .arg(colorCode)
            .arg(levelStr)
            .arg(dimCode)
            .arg(category)
            .arg(message);
    } else {
        formattedMsg = QString("[%1] [%2] [%3] %4")
            .arg(timestamp)
            .arg(levelStr)
            .arg(category)
            .arg(message);
    }
    
    // 添加源码位置信息（仅 Debug 级别）
    if (level == Level::Debug && file && function) {
        QString fileName = QFileInfo(file).fileName();
        if (colored) {
            formattedMsg += QString(" \033[2m(%1:%2 %3)\033[0m")
                .arg(fileName)
                .arg(line)
                .arg(function);
        } else {
            formattedMsg += QString(" (%1:%2 %3)")
                .arg(fileName)
                .arg(line)
                .arg(function);
        }
    }
    
    return formattedMsg;
}

void Logger::rotateLogFileIfNeeded()
{
    if (!m_logFile || !m_logFile->isOpen()) {
        return;
    }
    
    if (m_logFile->size() < m_maxFileSize) {
        return;
    }
    
    // 关闭当前文件
    if (m_logStream) {
        m_logStream->flush();
        delete m_logStream;
        m_logStream = nullptr;
    }
    m_logFile->close();
    
    // 重命名旧文件
    QString baseName = m_logFile->fileName();
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString archiveName = baseName;
    archiveName.replace(".log", QString("_%1.log").arg(timestamp));
    
    QFile::rename(baseName, archiveName);
    
    // 打开新文件
    openLogFile();
    
    // 清理旧日志文件（保留最近5个）
    QDir logDir(m_logDir);
    QStringList filters;
    filters << "screenshot_*.log";
    QFileInfoList fileList = logDir.entryInfoList(filters, QDir::Files, QDir::Time);
    
    while (fileList.size() > 5) {
        QFile::remove(fileList.takeLast().absoluteFilePath());
    }
}

bool Logger::openLogFile()
{
    QString fileName = QString("%1/screenshot_%2.log")
        .arg(m_logDir)
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
    
    m_logFile = new QFile(fileName);
    
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete m_logFile;
        m_logFile = nullptr;
        return false;
    }
    
    m_logStream = new QTextStream(m_logFile);
    m_logStream->setEncoding(QStringConverter::Utf8);
    
    return true;
}

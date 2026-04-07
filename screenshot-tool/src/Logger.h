/**
 * @file Logger.h
 * @brief 日志管理类 - 提供统一的日志记录功能
 * 
 * 特性：
 * - 支持多日志级别（Debug, Info, Warning, Error, Critical）
 * - 支持文件输出和控制台输出
 * - 支持日志文件轮转
 * - 线程安全
 * - 彩色控制台输出
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

/**
 * @class Logger
 * @brief 单例日志管理器
 */
class Logger : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 日志级别枚举
     */
    enum class Level {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };
    Q_ENUM(Level)

    /**
     * @brief 获取单例实例
     * @return Logger 单例指针
     */
    static Logger* instance();
    
    /**
     * @brief 初始化日志系统
     * @param enableFileLog 是否启用文件日志
     * @param logDir 日志目录路径（空则使用默认路径）
     */
    void init(bool enableFileLog = true, const QString &logDir = QString());
    
    /**
     * @brief 关闭日志系统
     */
    void shutdown();
    
    /**
     * @brief 设置最低日志级别
     * @param level 最低级别，低于此级别的日志将被忽略
     */
    void setLogLevel(Level level);
    
    /**
     * @brief 获取当前日志级别
     * @return 当前最低日志级别
     */
    Level logLevel() const;
    
    /**
     * @brief 启用/禁用控制台彩色输出
     * @param enable 是否启用
     */
    void setColorOutput(bool enable);
    
    /**
     * @brief 启用/禁用文件日志
     * @param enable 是否启用
     */
    void setFileLogEnabled(bool enable);
    
    /**
     * @brief 设置最大日志文件大小（字节）
     * @param maxSize 最大大小，超过后自动轮转
     */
    void setMaxFileSize(qint64 maxSize);
    
    /**
     * @brief 写入日志
     * @param level 日志级别
     * @param category 日志分类
     * @param message 日志消息
     * @param file 源文件名
     * @param line 行号
     * @param function 函数名
     */
    void log(Level level, 
             const QString &category,
             const QString &message,
             const char *file = nullptr,
             int line = 0,
             const char *function = nullptr);
    
    /**
     * @brief 便捷日志方法 - Debug
     */
    void debug(const QString &category, const QString &message);
    
    /**
     * @brief 便捷日志方法 - Info
     */
    void info(const QString &category, const QString &message);
    
    /**
     * @brief 便捷日志方法 - Warning
     */
    void warning(const QString &category, const QString &message);
    
    /**
     * @brief 便捷日志方法 - Error
     */
    void error(const QString &category, const QString &message);
    
    /**
     * @brief 便捷日志方法 - Critical
     */
    void critical(const QString &category, const QString &message);
    
    /**
     * @brief 获取日志目录路径
     * @return 日志目录路径
     */
    QString logDirectory() const;

signals:
    /**
     * @brief 新日志消息信号
     * @param level 日志级别
     * @param message 格式化后的日志消息
     */
    void logMessage(Logger::Level level, const QString &message);

private:
    /**
     * @brief 私有构造函数（单例模式）
     */
    explicit Logger(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Logger() override;
    
    // 禁用拷贝
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief 获取级别名称
     * @param level 日志级别
     * @return 级别名称字符串
     */
    QString levelToString(Level level) const;
    
    /**
     * @brief 获取级别对应的控制台颜色代码
     * @param level 日志级别
     * @return ANSI 颜色代码
     */
    QString levelToColor(Level level) const;
    
    /**
     * @brief 格式化日志消息
     * @param level 日志级别
     * @param category 分类
     * @param message 消息
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param colored 是否包含颜色代码
     * @return 格式化后的字符串
     */
    QString formatMessage(Level level,
                         const QString &category,
                         const QString &message,
                         const char *file,
                         int line,
                         const char *function,
                         bool colored) const;
    
    /**
     * @brief 检查并执行日志文件轮转
     */
    void rotateLogFileIfNeeded();
    
    /**
     * @brief 打开日志文件
     * @return 是否成功
     */
    bool openLogFile();

private:
    static Logger *s_instance;      ///< 单例实例
    
    Level m_level;                  ///< 当前日志级别
    bool m_colorOutput;             ///< 是否彩色输出
    bool m_fileLogEnabled;          ///< 是否启用文件日志
    qint64 m_maxFileSize;           ///< 最大文件大小
    
    QString m_logDir;               ///< 日志目录
    QFile *m_logFile;               ///< 日志文件
    QTextStream *m_logStream;       ///< 日志文件流
    
    mutable QMutex m_mutex;         ///< 线程安全互斥锁
    
    bool m_initialized;             ///< 是否已初始化
};

// ===== 便捷宏定义 =====

#define LOG_DEBUG(category, message) \
    Logger::instance()->log(Logger::Level::Debug, category, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_INFO(category, message) \
    Logger::instance()->log(Logger::Level::Info, category, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_WARNING(category, message) \
    Logger::instance()->log(Logger::Level::Warning, category, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_ERROR(category, message) \
    Logger::instance()->log(Logger::Level::Error, category, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_CRITICAL(category, message) \
    Logger::instance()->log(Logger::Level::Critical, category, message, __FILE__, __LINE__, __FUNCTION__)

#endif // LOGGER_H

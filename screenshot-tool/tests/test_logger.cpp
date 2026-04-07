/**
 * @file test_logger.cpp
 * @brief Logger 类单元测试
 */

#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSignalSpy>

#include "Logger.h"

class TestLogger : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir *m_tempDir;
    
private slots:
    /**
     * @brief 初始化测试环境
     */
    void initTestCase()
    {
        qDebug() << "初始化 Logger 测试";
        m_tempDir = new QTemporaryDir();
        QVERIFY(m_tempDir->isValid());
    }
    
    /**
     * @brief 清理测试环境
     */
    void cleanupTestCase()
    {
        Logger::instance()->shutdown();
        delete m_tempDir;
        qDebug() << "清理 Logger 测试";
    }
    
    /**
     * @brief 每个测试前的清理
     */
    void cleanup()
    {
        Logger::instance()->shutdown();
    }
    
    /**
     * @brief 测试单例模式
     */
    void testSingleton()
    {
        Logger *logger1 = Logger::instance();
        Logger *logger2 = Logger::instance();
        
        QVERIFY(logger1 != nullptr);
        QVERIFY(logger2 != nullptr);
        QCOMPARE(logger1, logger2);
    }
    
    /**
     * @brief 测试日志初始化
     */
    void testInit()
    {
        QString logDir = m_tempDir->path() + "/logs_init";
        
        Logger::instance()->init(true, logDir);
        
        // 验证日志目录已创建
        QVERIFY(QDir(logDir).exists());
        QCOMPARE(Logger::instance()->logDirectory(), logDir);
    }
    
    /**
     * @brief 测试日志级别设置
     */
    void testLogLevel()
    {
        Logger *logger = Logger::instance();
        
        // 默认级别应该是 Debug
        QCOMPARE(logger->logLevel(), Logger::Level::Debug);
        
        // 设置为 Warning
        logger->setLogLevel(Logger::Level::Warning);
        QCOMPARE(logger->logLevel(), Logger::Level::Warning);
        
        // 设置为 Error
        logger->setLogLevel(Logger::Level::Error);
        QCOMPARE(logger->logLevel(), Logger::Level::Error);
        
        // 重置为 Debug
        logger->setLogLevel(Logger::Level::Debug);
    }
    
    /**
     * @brief 测试日志输出
     */
    void testLogOutput()
    {
        QString logDir = m_tempDir->path() + "/logs_output";
        
        Logger::instance()->init(true, logDir);
        Logger::instance()->setLogLevel(Logger::Level::Debug);
        
        // 写入各级别日志
        Logger::instance()->debug("Test", "Debug message");
        Logger::instance()->info("Test", "Info message");
        Logger::instance()->warning("Test", "Warning message");
        Logger::instance()->error("Test", "Error message");
        Logger::instance()->critical("Test", "Critical message");
        
        // 关闭以确保写入完成
        Logger::instance()->shutdown();
        
        // 检查日志文件是否存在
        QDir dir(logDir);
        QStringList logFiles = dir.entryList(QStringList() << "*.log", QDir::Files);
        QVERIFY(!logFiles.isEmpty());
        
        // 读取日志内容
        QString logFilePath = logDir + "/" + logFiles.first();
        QFile logFile(logFilePath);
        QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
        
        QString content = logFile.readAll();
        logFile.close();
        
        // 验证日志内容包含所有级别的消息
        QVERIFY(content.contains("Debug message"));
        QVERIFY(content.contains("Info message"));
        QVERIFY(content.contains("Warning message"));
        QVERIFY(content.contains("Error message"));
        QVERIFY(content.contains("Critical message"));
        
        qDebug() << "日志文件内容:\n" << content;
    }
    
    /**
     * @brief 测试日志级别过滤
     */
    void testLogLevelFiltering()
    {
        QString logDir = m_tempDir->path() + "/logs_filter";
        
        Logger::instance()->init(true, logDir);
        Logger::instance()->setLogLevel(Logger::Level::Warning);
        
        // 写入日志（Debug 和 Info 应该被过滤）
        Logger::instance()->debug("Test", "Filtered debug");
        Logger::instance()->info("Test", "Filtered info");
        Logger::instance()->warning("Test", "Visible warning");
        Logger::instance()->error("Test", "Visible error");
        
        Logger::instance()->shutdown();
        
        // 读取日志内容
        QDir dir(logDir);
        QStringList logFiles = dir.entryList(QStringList() << "*.log", QDir::Files);
        QVERIFY(!logFiles.isEmpty());
        
        QString logFilePath = logDir + "/" + logFiles.first();
        QFile logFile(logFilePath);
        QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
        
        QString content = logFile.readAll();
        logFile.close();
        
        // Debug 和 Info 不应该出现
        QVERIFY(!content.contains("Filtered debug"));
        QVERIFY(!content.contains("Filtered info"));
        
        // Warning 和 Error 应该出现
        QVERIFY(content.contains("Visible warning"));
        QVERIFY(content.contains("Visible error"));
    }
    
    /**
     * @brief 测试信号发送
     */
    void testLogSignal()
    {
        Logger::instance()->init(false);
        Logger::instance()->setLogLevel(Logger::Level::Debug);
        
        QSignalSpy spy(Logger::instance(), 
                      &Logger::logMessage);
        
        Logger::instance()->info("Test", "Signal test");
        
        QCOMPARE(spy.count(), 1);
        
        QList<QVariant> arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).value<Logger::Level>(), Logger::Level::Info);
        QVERIFY(arguments.at(1).toString().contains("Signal test"));
    }
    
    /**
     * @brief 测试宏定义
     */
    void testMacros()
    {
        QString logDir = m_tempDir->path() + "/logs_macro";
        
        Logger::instance()->init(true, logDir);
        Logger::instance()->setLogLevel(Logger::Level::Debug);
        
        LOG_DEBUG("Macro", "Debug via macro");
        LOG_INFO("Macro", "Info via macro");
        LOG_WARNING("Macro", "Warning via macro");
        LOG_ERROR("Macro", "Error via macro");
        LOG_CRITICAL("Macro", "Critical via macro");
        
        Logger::instance()->shutdown();
        
        // 读取并验证
        QDir dir(logDir);
        QStringList logFiles = dir.entryList(QStringList() << "*.log", QDir::Files);
        QString logFilePath = logDir + "/" + logFiles.first();
        QFile logFile(logFilePath);
        logFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QString content = logFile.readAll();
        logFile.close();
        
        QVERIFY(content.contains("Debug via macro"));
        QVERIFY(content.contains("Info via macro"));
    }
    
    /**
     * @brief 测试禁用文件日志
     */
    void testDisableFileLog()
    {
        QString logDir = m_tempDir->path() + "/logs_disabled";
        
        Logger::instance()->init(false, logDir);
        Logger::instance()->info("Test", "No file log");
        Logger::instance()->shutdown();
        
        // 验证没有创建日志文件
        QDir dir(logDir);
        if (dir.exists()) {
            QStringList logFiles = dir.entryList(QStringList() << "*.log", QDir::Files);
            QVERIFY(logFiles.isEmpty());
        }
    }
    
    /**
     * @brief 测试颜色输出设置
     */
    void testColorOutput()
    {
        Logger::instance()->init(false);
        
        Logger::instance()->setColorOutput(true);
        Logger::instance()->info("Test", "Colored output");
        
        Logger::instance()->setColorOutput(false);
        Logger::instance()->info("Test", "Plain output");
        
        // 这主要是视觉测试，确保不会崩溃
        QVERIFY(true);
    }
    
    /**
     * @brief 基准测试：日志写入性能
     */
    void benchmarkLogWriting()
    {
        Logger::instance()->init(false);
        Logger::instance()->setLogLevel(Logger::Level::Debug);
        
        QBENCHMARK {
            Logger::instance()->info("Benchmark", "Performance test message");
        }
    }
};

QTEST_MAIN(TestLogger)
#include "test_logger.moc"

/**
 * @file test_screen_capture.cpp
 * @brief ScreenCapture 类单元测试
 */

#include <QtTest/QtTest>
#include <QGuiApplication>
#include <QScreen>

#include "ScreenCapture.h"

class TestScreenCapture : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief 初始化测试环境
     */
    void initTestCase()
    {
        qDebug() << "初始化 ScreenCapture 测试";
    }
    
    /**
     * @brief 清理测试环境
     */
    void cleanupTestCase()
    {
        qDebug() << "清理 ScreenCapture 测试";
    }
    
    /**
     * @brief 测试获取屏幕数量
     */
    void testGetScreenCount()
    {
        ScreenCapture capture;
        int count = capture.getScreenCount();
        
        QVERIFY2(count > 0, "至少应该有一个屏幕");
        QCOMPARE(count, QGuiApplication::screens().size());
        
        qDebug() << "屏幕数量:" << count;
    }
    
    /**
     * @brief 测试获取所有屏幕列表
     */
    void testGetAllScreens()
    {
        ScreenCapture capture;
        QList<QScreen*> screens = capture.getAllScreens();
        
        QVERIFY(!screens.isEmpty());
        QCOMPARE(screens.size(), capture.getScreenCount());
        
        for (QScreen *screen : screens) {
            QVERIFY(screen != nullptr);
            qDebug() << "屏幕:" << screen->name() 
                     << "几何区域:" << screen->geometry()
                     << "DPR:" << screen->devicePixelRatio();
        }
    }
    
    /**
     * @brief 测试获取主屏幕几何区域
     */
    void testGetPrimaryScreenGeometry()
    {
        ScreenCapture capture;
        QRect geometry = capture.getPrimaryScreenGeometry();
        
        QVERIFY(!geometry.isEmpty());
        QVERIFY(geometry.width() > 0);
        QVERIFY(geometry.height() > 0);
        
        qDebug() << "主屏幕几何区域:" << geometry;
    }
    
    /**
     * @brief 测试获取虚拟桌面几何区域
     */
    void testGetVirtualDesktopGeometry()
    {
        ScreenCapture capture;
        QRect virtualGeometry = capture.getVirtualDesktopGeometry();
        QRect primaryGeometry = capture.getPrimaryScreenGeometry();
        
        QVERIFY(!virtualGeometry.isEmpty());
        
        // 虚拟桌面应该包含主屏幕
        QVERIFY(virtualGeometry.contains(primaryGeometry) ||
                virtualGeometry == primaryGeometry);
        
        qDebug() << "虚拟桌面几何区域:" << virtualGeometry;
    }
    
    /**
     * @brief 测试捕获单个屏幕
     */
    void testCaptureScreen()
    {
        ScreenCapture capture;
        
        // 捕获第一个屏幕
        QPixmap pixmap = capture.captureScreen(0);
        
        QVERIFY2(!pixmap.isNull(), "截图不应为空");
        QVERIFY(pixmap.width() > 0);
        QVERIFY(pixmap.height() > 0);
        
        qDebug() << "单屏幕截图尺寸:" << pixmap.size()
                 << "DPR:" << pixmap.devicePixelRatio();
    }
    
    /**
     * @brief 测试捕获无效屏幕索引
     */
    void testCaptureInvalidScreen()
    {
        ScreenCapture capture;
        
        // 使用无效索引
        QPixmap pixmap = capture.captureScreen(-1);
        QVERIFY(pixmap.isNull());
        
        pixmap = capture.captureScreen(9999);
        QVERIFY(pixmap.isNull());
    }
    
    /**
     * @brief 测试捕获所有屏幕
     */
    void testCaptureAllScreens()
    {
        ScreenCapture capture;
        QPixmap pixmap = capture.captureAllScreens();
        
        QVERIFY2(!pixmap.isNull(), "全屏截图不应为空");
        
        QRect virtualGeometry = capture.getVirtualDesktopGeometry();
        
        // 截图尺寸应该与虚拟桌面大小匹配（考虑 DPR）
        qreal dpr = pixmap.devicePixelRatio();
        int expectedWidth = virtualGeometry.width() * dpr;
        int expectedHeight = virtualGeometry.height() * dpr;
        
        QCOMPARE(pixmap.width(), expectedWidth);
        QCOMPARE(pixmap.height(), expectedHeight);
        
        qDebug() << "全屏截图尺寸:" << pixmap.size()
                 << "预期尺寸:" << expectedWidth << "x" << expectedHeight;
    }
    
    /**
     * @brief 测试使用 QScreen 指针捕获屏幕
     */
    void testCaptureScreenByPointer()
    {
        ScreenCapture capture;
        QScreen *primaryScreen = QGuiApplication::primaryScreen();
        
        QVERIFY(primaryScreen != nullptr);
        
        QPixmap pixmap = capture.captureScreen(primaryScreen);
        
        QVERIFY(!pixmap.isNull());
        QVERIFY(pixmap.width() > 0);
        QVERIFY(pixmap.height() > 0);
    }
    
    /**
     * @brief 测试捕获空屏幕指针
     */
    void testCaptureNullScreen()
    {
        ScreenCapture capture;
        QPixmap pixmap = capture.captureScreen(nullptr);
        
        QVERIFY(pixmap.isNull());
    }
    
    /**
     * @brief 基准测试：截图性能
     */
    void benchmarkCaptureAllScreens()
    {
        ScreenCapture capture;
        
        QBENCHMARK {
            QPixmap pixmap = capture.captureAllScreens();
            Q_UNUSED(pixmap);
        }
    }
};

QTEST_MAIN(TestScreenCapture)
#include "test_screen_capture.moc"

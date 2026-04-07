/**
 * @file test_selection_overlay.cpp
 * @brief SelectionOverlay 类单元测试
 */

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>

#include "SelectionOverlay.h"

class TestSelectionOverlay : public QObject
{
    Q_OBJECT

private:
    SelectionOverlay *m_overlay;
    QPixmap m_testScreenshot;
    
private slots:
    /**
     * @brief 初始化测试环境
     */
    void initTestCase()
    {
        qDebug() << "初始化 SelectionOverlay 测试";
        
        // 创建测试用截图（纯色图像）
        m_testScreenshot = QPixmap(1920, 1080);
        m_testScreenshot.fill(Qt::blue);
    }
    
    /**
     * @brief 清理测试环境
     */
    void cleanupTestCase()
    {
        qDebug() << "清理 SelectionOverlay 测试";
    }
    
    /**
     * @brief 每个测试前初始化
     */
    void init()
    {
        m_overlay = new SelectionOverlay();
        m_overlay->setScreenshot(m_testScreenshot);
    }
    
    /**
     * @brief 每个测试后清理
     */
    void cleanup()
    {
        delete m_overlay;
        m_overlay = nullptr;
    }
    
    /**
     * @brief 测试初始状态
     */
    void testInitialState()
    {
        QVERIFY(m_overlay != nullptr);
        
        // 初始选区应该为空
        QRect selection = m_overlay->getSelectedRegion();
        QVERIFY(selection.isEmpty());
        
        // 初始截图应该为空
        QPixmap selectedPixmap = m_overlay->getSelectedPixmap();
        QVERIFY(selectedPixmap.isNull());
    }
    
    /**
     * @brief 测试设置截图
     */
    void testSetScreenshot()
    {
        QPixmap newScreenshot(800, 600);
        newScreenshot.fill(Qt::red);
        
        m_overlay->setScreenshot(newScreenshot);
        
        // 设置新截图后，选区应该被重置
        QRect selection = m_overlay->getSelectedRegion();
        QVERIFY(selection.isEmpty());
    }
    
    /**
     * @brief 测试取消信号
     */
    void testCancelSignal()
    {
        QSignalSpy spy(m_overlay, &SelectionOverlay::captureCancelled);
        
        // 模拟 ESC 键按下
        QKeyEvent escEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &escEvent);
        
        QCOMPARE(spy.count(), 1);
    }
    
    /**
     * @brief 测试鼠标框选（模拟）
     */
    void testMouseSelection()
    {
        m_overlay->resize(1920, 1080);
        
        // 模拟鼠标按下
        QPoint startPos(100, 100);
        QMouseEvent pressEvent(QEvent::MouseButtonPress, 
                              startPos, 
                              Qt::LeftButton, 
                              Qt::LeftButton, 
                              Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &pressEvent);
        
        // 模拟鼠标移动
        QPoint movePos(300, 250);
        QMouseEvent moveEvent(QEvent::MouseMove, 
                             movePos, 
                             Qt::LeftButton, 
                             Qt::LeftButton, 
                             Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &moveEvent);
        
        // 模拟鼠标释放
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, 
                                movePos, 
                                Qt::LeftButton, 
                                Qt::NoButton, 
                                Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &releaseEvent);
        
        // 验证选区
        QRect selection = m_overlay->getSelectedRegion();
        QVERIFY(!selection.isEmpty());
        QCOMPARE(selection.topLeft(), startPos);
        QCOMPARE(selection.bottomRight(), movePos);
    }
    
    /**
     * @brief 测试选区太小被忽略
     */
    void testMinimumSelectionSize()
    {
        m_overlay->resize(1920, 1080);
        
        // 模拟很小的框选
        QPoint startPos(100, 100);
        QPoint endPos(105, 105); // 只有 5x5
        
        QMouseEvent pressEvent(QEvent::MouseButtonPress, 
                              startPos, 
                              Qt::LeftButton, 
                              Qt::LeftButton, 
                              Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &pressEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, 
                                endPos, 
                                Qt::LeftButton, 
                                Qt::NoButton, 
                                Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &releaseEvent);
        
        // 选区太小应该被忽略
        QRect selection = m_overlay->getSelectedRegion();
        QVERIFY(selection.isEmpty());
    }
    
    /**
     * @brief 测试确认截图信号
     */
    void testConfirmSignal()
    {
        m_overlay->resize(1920, 1080);
        
        QSignalSpy spy(m_overlay, &SelectionOverlay::captureConfirmed);
        
        // 先创建一个有效选区
        QPoint startPos(100, 100);
        QPoint endPos(300, 250);
        
        QMouseEvent pressEvent(QEvent::MouseButtonPress, 
                              startPos, 
                              Qt::LeftButton, 
                              Qt::LeftButton, 
                              Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &pressEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, 
                                endPos, 
                                Qt::LeftButton, 
                                Qt::NoButton, 
                                Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &releaseEvent);
        
        // 模拟 Enter 键确认
        QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &enterEvent);
        
        QCOMPARE(spy.count(), 1);
        
        // 验证信号携带的 pixmap
        QList<QVariant> arguments = spy.takeFirst();
        QPixmap capturedPixmap = arguments.at(0).value<QPixmap>();
        QVERIFY(!capturedPixmap.isNull());
    }
    
    /**
     * @brief 测试保存请求信号
     */
    void testSaveSignal()
    {
        m_overlay->resize(1920, 1080);
        
        QSignalSpy spy(m_overlay, &SelectionOverlay::saveRequested);
        
        // 先创建一个有效选区
        QPoint startPos(100, 100);
        QPoint endPos(300, 250);
        
        QMouseEvent pressEvent(QEvent::MouseButtonPress, 
                              startPos, 
                              Qt::LeftButton, 
                              Qt::LeftButton, 
                              Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &pressEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, 
                                endPos, 
                                Qt::LeftButton, 
                                Qt::NoButton, 
                                Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &releaseEvent);
        
        // 模拟 Ctrl+S 保存
        QKeyEvent saveEvent(QEvent::KeyPress, Qt::Key_S, Qt::ControlModifier);
        QApplication::sendEvent(m_overlay, &saveEvent);
        
        QCOMPARE(spy.count(), 1);
    }
    
    /**
     * @brief 测试 Ctrl+A 全选
     */
    void testSelectAll()
    {
        m_overlay->resize(1920, 1080);
        
        // 模拟 Ctrl+A
        QKeyEvent selectAllEvent(QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier);
        QApplication::sendEvent(m_overlay, &selectAllEvent);
        
        QRect selection = m_overlay->getSelectedRegion();
        
        // 应该选中整个区域
        QCOMPARE(selection.topLeft(), QPoint(0, 0));
        QCOMPARE(selection.width(), 1920);
        QCOMPARE(selection.height(), 1080);
    }
    
    /**
     * @brief 测试右键取消
     */
    void testRightClickCancel()
    {
        QSignalSpy spy(m_overlay, &SelectionOverlay::captureCancelled);
        
        // 模拟右键点击
        QMouseEvent rightClickEvent(QEvent::MouseButtonPress, 
                                   QPoint(100, 100), 
                                   Qt::RightButton, 
                                   Qt::RightButton, 
                                   Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &rightClickEvent);
        
        QCOMPARE(spy.count(), 1);
    }
    
    /**
     * @brief 测试获取选区像素图
     */
    void testGetSelectedPixmap()
    {
        m_overlay->resize(1920, 1080);
        
        // 创建选区
        QPoint startPos(100, 100);
        QPoint endPos(300, 250);
        
        QMouseEvent pressEvent(QEvent::MouseButtonPress, 
                              startPos, 
                              Qt::LeftButton, 
                              Qt::LeftButton, 
                              Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &pressEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, 
                                endPos, 
                                Qt::LeftButton, 
                                Qt::NoButton, 
                                Qt::NoModifier);
        QApplication::sendEvent(m_overlay, &releaseEvent);
        
        // 获取选区图像
        QPixmap selectedPixmap = m_overlay->getSelectedPixmap();
        
        QVERIFY(!selectedPixmap.isNull());
        
        // 验证尺寸（考虑 DPR）
        QRect selection = m_overlay->getSelectedRegion();
        qreal dpr = m_testScreenshot.devicePixelRatio();
        
        QCOMPARE(selectedPixmap.width(), (int)(selection.width() * dpr));
        QCOMPARE(selectedPixmap.height(), (int)(selection.height() * dpr));
    }
    
    /**
     * @brief 测试窗口属性
     */
    void testWindowFlags()
    {
        Qt::WindowFlags flags = m_overlay->windowFlags();
        
        // 应该是无边框窗口
        QVERIFY(flags & Qt::FramelessWindowHint);
        
        // 应该置顶
        QVERIFY(flags & Qt::WindowStaysOnTopHint);
    }
};

QTEST_MAIN(TestSelectionOverlay)
#include "test_selection_overlay.moc"

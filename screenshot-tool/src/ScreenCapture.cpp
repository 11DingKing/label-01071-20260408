/**
 * @file ScreenCapture.cpp
 * @brief 屏幕捕获类实现
 */

#include "ScreenCapture.h"

#include <QGuiApplication>
#include <QPainter>
#include <QWindow>

// 定义日志分类
Q_LOGGING_CATEGORY(logScreenCapture, "screenshot.capture")

ScreenCapture::ScreenCapture(QObject *parent)
    : QObject(parent)
{
    qCDebug(logScreenCapture) << "ScreenCapture 初始化";
    qCDebug(logScreenCapture) << "检测到屏幕数量:" << getScreenCount();
}

ScreenCapture::~ScreenCapture()
{
    qCDebug(logScreenCapture) << "ScreenCapture 销毁";
}

QPixmap ScreenCapture::captureAllScreens()
{
    qCInfo(logScreenCapture) << "开始捕获所有屏幕";
    
    QList<QScreen*> screens = getAllScreens();
    
    if (screens.isEmpty()) {
        qCWarning(logScreenCapture) << "没有检测到任何屏幕";
        return QPixmap();
    }
    
    // 如果只有一个屏幕，直接捕获
    if (screens.size() == 1) {
        qCDebug(logScreenCapture) << "单屏幕模式";
        return captureScreen(screens.first());
    }
    
    // 多屏幕情况：计算虚拟桌面大小并合并
    qCDebug(logScreenCapture) << "多屏幕模式，屏幕数量:" << screens.size();
    
    QRect virtualGeometry = calculateVirtualGeometry();
    qCDebug(logScreenCapture) << "虚拟桌面几何区域:" << virtualGeometry;
    
    // 获取最大设备像素比
    qreal maxDpr = 1.0;
    for (QScreen *screen : screens) {
        maxDpr = qMax(maxDpr, screen->devicePixelRatio());
    }
    
    // 创建目标图像
    QPixmap result(virtualGeometry.width() * maxDpr, 
                   virtualGeometry.height() * maxDpr);
    result.setDevicePixelRatio(maxDpr);
    result.fill(Qt::black); // 填充黑色背景
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // 捕获每个屏幕并绘制到结果图像中
    for (QScreen *screen : screens) {
        QPixmap screenPixmap = captureScreen(screen);
        
        if (!screenPixmap.isNull()) {
            // 计算在虚拟桌面中的位置（相对于左上角）
            QRect screenGeometry = screen->geometry();
            QPoint offset(screenGeometry.left() - virtualGeometry.left(),
                         screenGeometry.top() - virtualGeometry.top());
            
            qCDebug(logScreenCapture) << "绘制屏幕:" << screen->name()
                                      << "位置:" << offset
                                      << "尺寸:" << screenGeometry.size();
            
            painter.drawPixmap(offset, screenPixmap);
        }
    }
    
    painter.end();
    
    qCInfo(logScreenCapture) << "屏幕捕获完成，结果尺寸:" 
                             << result.width() << "x" << result.height()
                             << "DPR:" << result.devicePixelRatio();
    
    return result;
}

QPixmap ScreenCapture::captureScreen(int screenIndex)
{
    QList<QScreen*> screens = getAllScreens();
    
    if (screenIndex < 0 || screenIndex >= screens.size()) {
        qCWarning(logScreenCapture) << "无效的屏幕索引:" << screenIndex
                                    << "可用范围: 0 -" << screens.size() - 1;
        return QPixmap();
    }
    
    return captureScreen(screens.at(screenIndex));
}

QPixmap ScreenCapture::captureScreen(QScreen *screen)
{
    if (!screen) {
        qCWarning(logScreenCapture) << "屏幕指针为空";
        return QPixmap();
    }
    
    qCDebug(logScreenCapture) << "捕获屏幕:" << screen->name()
                              << "几何区域:" << screen->geometry()
                              << "DPR:" << screen->devicePixelRatio();
    
    // 使用 grabWindow 捕获屏幕
    // WId(0) 表示捕获整个桌面
    QPixmap pixmap = screen->grabWindow(0);
    
    if (pixmap.isNull()) {
        qCWarning(logScreenCapture) << "捕获屏幕失败:" << screen->name();
        return QPixmap();
    }
    
    qCDebug(logScreenCapture) << "屏幕捕获成功:" << screen->name()
                              << "图像尺寸:" << pixmap.width() << "x" << pixmap.height();
    
    return pixmap;
}

QRect ScreenCapture::getVirtualDesktopGeometry() const
{
    return calculateVirtualGeometry();
}

QRect ScreenCapture::getPrimaryScreenGeometry() const
{
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    
    if (primaryScreen) {
        return primaryScreen->geometry();
    }
    
    return QRect();
}

QList<QScreen*> ScreenCapture::getAllScreens() const
{
    return QGuiApplication::screens();
}

int ScreenCapture::getScreenCount() const
{
    return QGuiApplication::screens().size();
}

QRect ScreenCapture::calculateVirtualGeometry() const
{
    QList<QScreen*> screens = getAllScreens();
    
    if (screens.isEmpty()) {
        return QRect();
    }
    
    // 计算包含所有屏幕的边界矩形
    QRect virtualRect;
    
    for (QScreen *screen : screens) {
        if (virtualRect.isNull()) {
            virtualRect = screen->geometry();
        } else {
            virtualRect = virtualRect.united(screen->geometry());
        }
    }
    
    return virtualRect;
}

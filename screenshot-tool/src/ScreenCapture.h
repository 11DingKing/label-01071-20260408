/**
 * @file ScreenCapture.h
 * @brief 屏幕捕获类 - 负责捕获屏幕图像
 */

#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QScreen>
#include <QLoggingCategory>

// 日志分类声明
Q_DECLARE_LOGGING_CATEGORY(logScreenCapture)

/**
 * @class ScreenCapture
 * @brief 屏幕捕获工具类，支持单屏和多屏截图
 */
class ScreenCapture : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit ScreenCapture(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ScreenCapture() override;
    
    /**
     * @brief 捕获所有屏幕组成的完整图像
     * 
     * 该方法会捕获所有连接的显示器，并将它们合并为一张完整的图像
     * 支持多显示器和不同 DPI 的屏幕配置
     * 
     * @return 包含所有屏幕内容的图像，失败返回空 QPixmap
     */
    QPixmap captureAllScreens();
    
    /**
     * @brief 捕获指定屏幕的图像
     * @param screenIndex 屏幕索引（从0开始）
     * @return 指定屏幕的图像，失败返回空 QPixmap
     */
    QPixmap captureScreen(int screenIndex);
    
    /**
     * @brief 捕获指定屏幕的图像
     * @param screen 屏幕指针
     * @return 指定屏幕的图像，失败返回空 QPixmap
     */
    QPixmap captureScreen(QScreen *screen);
    
    /**
     * @brief 获取虚拟桌面的总几何区域
     * 
     * 包含所有屏幕的边界矩形
     * 
     * @return 虚拟桌面的边界矩形
     */
    QRect getVirtualDesktopGeometry() const;
    
    /**
     * @brief 获取主屏幕的几何区域
     * @return 主屏幕的边界矩形
     */
    QRect getPrimaryScreenGeometry() const;
    
    /**
     * @brief 获取所有屏幕的列表
     * @return 屏幕指针列表
     */
    QList<QScreen*> getAllScreens() const;
    
    /**
     * @brief 获取屏幕数量
     * @return 连接的屏幕数量
     */
    int getScreenCount() const;

private:
    /**
     * @brief 计算虚拟桌面的总几何区域
     * @return 包含所有屏幕的边界矩形
     */
    QRect calculateVirtualGeometry() const;
};

#endif // SCREENCAPTURE_H

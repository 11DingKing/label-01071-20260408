/**
 * @file SelectionOverlay.h
 * @brief 选区覆盖层类 - 显示半透明遮罩并处理框选交互
 */

#ifndef SELECTIONOVERLAY_H
#define SELECTIONOVERLAY_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QLoggingCategory>

// 前向声明
class ToolBar;

// 日志分类声明
Q_DECLARE_LOGGING_CATEGORY(logSelectionOverlay)

/**
 * @class SelectionOverlay
 * @brief 全屏透明覆盖层，用于显示截图遮罩和处理框选交互
 * 
 * 该类创建一个全屏无边框窗口，显示捕获的屏幕图像，
 * 并在其上绘制半透明遮罩，用户可以通过拖拽选择截图区域
 */
class SelectionOverlay : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 选区状态枚举
     */
    enum class SelectionState {
        None,       ///< 未开始选择
        Selecting,  ///< 正在框选
        Selected,   ///< 已选择完成
        Moving,     ///< 正在移动选区
        Resizing    ///< 正在调整选区大小
    };
    
    /**
     * @brief 调整句柄位置枚举
     */
    enum class ResizeHandle {
        None,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Top,
        Bottom,
        Left,
        Right
    };

    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit SelectionOverlay(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~SelectionOverlay() override;
    
    /**
     * @brief 设置要显示的屏幕截图
     * @param screenshot 屏幕截图
     */
    void setScreenshot(const QPixmap &screenshot);
    
    /**
     * @brief 获取当前选中的区域
     * @return 选中的矩形区域（逻辑坐标）
     */
    QRect getSelectedRegion() const;
    
    /**
     * @brief 获取选中区域的图像
     * @return 选中区域的 QPixmap
     */
    QPixmap getSelectedPixmap() const;

signals:
    /**
     * @brief 截图确认信号
     * @param pixmap 选中区域的图像
     */
    void captureConfirmed(const QPixmap &pixmap);
    
    /**
     * @brief 保存请求信号
     * @param pixmap 选中区域的图像
     */
    void saveRequested(const QPixmap &pixmap);
    
    /**
     * @brief 截图取消信号
     */
    void captureCancelled();

protected:
    /**
     * @brief 绘制事件处理
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent *event) override;
    
    /**
     * @brief 鼠标按下事件处理
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标移动事件处理
     * @param event 鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标释放事件处理
     * @param event 鼠标事件
     */
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标双击事件处理
     * @param event 鼠标事件
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    
    /**
     * @brief 键盘按下事件处理
     * @param event 键盘事件
     */
    void keyPressEvent(QKeyEvent *event) override;
    
    /**
     * @brief 显示事件处理
     * @param event 显示事件
     */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief 确认截图槽函数
     */
    void confirmCapture();
    
    /**
     * @brief 保存到文件槽函数
     */
    void saveToFile();
    
    /**
     * @brief 取消截图槽函数
     */
    void cancel();

private:
    /**
     * @brief 绘制半透明遮罩
     * @param painter 绘图器
     */
    void drawMask(QPainter &painter);
    
    /**
     * @brief 绘制选区边框
     * @param painter 绘图器
     */
    void drawSelectionBorder(QPainter &painter);
    
    /**
     * @brief 绘制选区尺寸提示
     * @param painter 绘图器
     */
    void drawSizeHint(QPainter &painter);
    
    /**
     * @brief 绘制调整句柄
     * @param painter 绘图器
     */
    void drawResizeHandles(QPainter &painter);
    
    /**
     * @brief 更新工具栏位置
     */
    void updateToolBarPosition();
    
    /**
     * @brief 规范化选区矩形（确保宽高为正）
     * @return 规范化后的选区矩形
     */
    QRect normalizedSelectionRect() const;
    
    /**
     * @brief 检测鼠标位置所在的调整句柄
     * @param pos 鼠标位置
     * @return 句柄类型
     */
    ResizeHandle hitTestHandle(const QPoint &pos) const;
    
    /**
     * @brief 根据句柄类型获取对应的光标
     * @param handle 句柄类型
     * @return 光标形状
     */
    Qt::CursorShape getCursorForHandle(ResizeHandle handle) const;
    
    /**
     * @brief 检测鼠标是否在选区内
     * @param pos 鼠标位置
     * @return 是否在选区内
     */
    bool isInsideSelection(const QPoint &pos) const;

private:
    // 截图数据
    QPixmap m_screenshot;           ///< 捕获的屏幕截图
    
    // 选区状态
    SelectionState m_state;         ///< 当前选区状态
    QPoint m_startPoint;            ///< 选区起始点
    QPoint m_endPoint;              ///< 选区结束点
    
    // 移动/调整相关
    QPoint m_lastMousePos;          ///< 上次鼠标位置（用于移动/调整）
    ResizeHandle m_activeHandle;    ///< 当前激活的调整句柄
    
    // 工具栏
    ToolBar *m_toolBar;             ///< 操作工具栏
    
    // UI 常量
    static constexpr int HANDLE_SIZE = 8;           ///< 调整句柄大小
    static constexpr int BORDER_WIDTH = 2;          ///< 边框宽度
    static constexpr int MIN_SELECTION_SIZE = 10;   ///< 最小选区尺寸
};

#endif // SELECTIONOVERLAY_H

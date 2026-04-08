/**
 * @file SelectionOverlay.cpp
 * @brief 选区覆盖层类实现
 */

#include <cmath>

#include "SelectionOverlay.h"
#include "ToolBar.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QScreen>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 定义日志分类
Q_LOGGING_CATEGORY(logSelectionOverlay, "screenshot.overlay")

SelectionOverlay::SelectionOverlay(QWidget *parent)
    : QWidget(parent)
    , m_state(SelectionState::None)
    , m_activeHandle(ResizeHandle::None)
    , m_toolBar(new ToolBar(this))
    , m_currentTool(AnnotationTool::Select)
{
    qCDebug(logSelectionOverlay) << "初始化选区覆盖层";
    
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | 
                   Qt::WindowStaysOnTopHint | 
                   Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    
    // 启用鼠标追踪（用于更新光标）
    setMouseTracking(true);
    
    // 设置焦点策略
    setFocusPolicy(Qt::StrongFocus);
    
    // 初始隐藏工具栏
    m_toolBar->hide();
    
    // 连接工具栏信号
    connect(m_toolBar, &ToolBar::confirmClicked, this, &SelectionOverlay::confirmCapture);
    connect(m_toolBar, &ToolBar::saveClicked, this, &SelectionOverlay::saveToFile);
    connect(m_toolBar, &ToolBar::cancelClicked, this, &SelectionOverlay::cancel);
    connect(m_toolBar, &ToolBar::selectToolClicked, this, &SelectionOverlay::onSelectTool);
    connect(m_toolBar, &ToolBar::rectangleToolClicked, this, &SelectionOverlay::onRectangleTool);
    connect(m_toolBar, &ToolBar::arrowToolClicked, this, &SelectionOverlay::onArrowTool);
    
    qCDebug(logSelectionOverlay) << "选区覆盖层初始化完成";
}

SelectionOverlay::~SelectionOverlay()
{
    qCDebug(logSelectionOverlay) << "销毁选区覆盖层";
}

void SelectionOverlay::setScreenshot(const QPixmap &screenshot)
{
    m_screenshot = screenshot;
    
    qCDebug(logSelectionOverlay) << "设置截图，尺寸:" 
                                 << screenshot.width() << "x" << screenshot.height();
    
    // 重置选区状态
    m_state = SelectionState::None;
    m_startPoint = QPoint();
    m_endPoint = QPoint();
    
    // 重置标注状态
    m_currentTool = AnnotationTool::Select;
    m_annotations.clear();
    
    // 隐藏工具栏
    m_toolBar->hide();
    
    update();
}

QRect SelectionOverlay::getSelectedRegion() const
{
    return normalizedSelectionRect();
}

QPixmap SelectionOverlay::getSelectedPixmap() const
{
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty() || m_screenshot.isNull()) {
        return QPixmap();
    }
    
    // 考虑设备像素比
    qreal dpr = m_screenshot.devicePixelRatio();
    QRect scaledRect(
        selection.x() * dpr,
        selection.y() * dpr,
        selection.width() * dpr,
        selection.height() * dpr
    );
    
    QPixmap result = m_screenshot.copy(scaledRect);
    
    // 如果有标注，绘制到结果上
    if (!m_annotations.isEmpty()) {
        QPainter painter(&result);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // 设置画笔（红色，固定粗细）
        QPen pen(QColor(255, 0, 0), ANNOTATION_LINE_WIDTH * dpr);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        
        // 绘制每个标注
        for (const Annotation &annotation : m_annotations) {
            // 计算相对于选区的坐标
            QPoint start = annotation.startPoint - selection.topLeft();
            QPoint end = annotation.endPoint - selection.topLeft();
            
            // 缩放坐标
            start = QPoint(start.x() * dpr, start.y() * dpr);
            end = QPoint(end.x() * dpr, end.y() * dpr);
            
            if (annotation.type == AnnotationTool::Rectangle) {
                painter.drawRect(QRect(start, end).normalized());
            } else if (annotation.type == AnnotationTool::Arrow) {
                // 绘制箭头线
                painter.drawLine(start, end);
                
                // 计算箭头头部
                double angle = std::atan2(end.y() - start.y(), end.x() - start.x());
                int arrowSize = ARROW_HEAD_SIZE * dpr;
                
                QPointF p1 = end - QPointF(
                    arrowSize * std::cos(angle - M_PI / 6),
                    arrowSize * std::sin(angle - M_PI / 6)
                );
                QPointF p2 = end - QPointF(
                    arrowSize * std::cos(angle + M_PI / 6),
                    arrowSize * std::sin(angle + M_PI / 6)
                );
                
                // 绘制箭头头部（填充）
                painter.setBrush(QColor(255, 0, 0));
                QPainterPath arrowPath;
                arrowPath.moveTo(end);
                arrowPath.lineTo(p1);
                arrowPath.lineTo(p2);
                arrowPath.closeSubpath();
                painter.drawPath(arrowPath);
                painter.setBrush(Qt::NoBrush);
            }
        }
    }
    
    return result;
}

void SelectionOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 先填充完全不透明的背景（确保窗口可见）
    painter.fillRect(rect(), Qt::black);
    
    // 绘制原始截图
    if (!m_screenshot.isNull()) {
        painter.drawPixmap(rect(), m_screenshot);
    }
    
    // 绘制半透明遮罩
    drawMask(painter);
    
    // 如果有选区，绘制边框和句柄
    if (m_state != SelectionState::None && m_state != SelectionState::Drawing) {
        drawSelectionBorder(painter);
        drawSizeHint(painter);
        
        if (m_state == SelectionState::Selected) {
            drawResizeHandles(painter);
        }
    }
    
    // 绘制所有标注
    drawAnnotations(painter);
    
    // 绘制正在绘制的标注预览
    if (m_state == SelectionState::Drawing) {
        Annotation preview;
        preview.type = m_currentTool;
        preview.startPoint = m_annotationStart;
        preview.endPoint = m_annotationEnd;
        drawAnnotation(painter, preview);
    }
}

void SelectionOverlay::drawMask(QPainter &painter)
{
    QRect selection = normalizedSelectionRect();
    
    // 如果没有选区，整个屏幕都显示遮罩
    if (selection.isEmpty()) {
        painter.fillRect(rect(), QColor(0, 0, 0, 120));
        return;
    }
    
    // 创建遮罩路径（整个窗口减去选区）
    QPainterPath maskPath;
    maskPath.addRect(rect());
    
    QPainterPath selectionPath;
    selectionPath.addRect(selection);
    maskPath = maskPath.subtracted(selectionPath);
    
    // 绘制半透明遮罩
    painter.fillPath(maskPath, QColor(0, 0, 0, 120));
}

void SelectionOverlay::drawSelectionBorder(QPainter &painter)
{
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty()) {
        return;
    }
    
    // 绘制外发光效果
    for (int i = 4; i > 0; i--) {
        QRect glowRect = selection.adjusted(-i, -i, i, i);
        QPen glowPen(QColor(64, 158, 255, 30 * (5 - i)), 1);
        painter.setPen(glowPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(glowRect);
    }
    
    // 绘制主边框（渐变效果）
    QLinearGradient borderGradient(selection.topLeft(), selection.bottomRight());
    borderGradient.setColorAt(0, QColor(64, 158, 255));
    borderGradient.setColorAt(0.5, QColor(102, 177, 255));
    borderGradient.setColorAt(1, QColor(64, 158, 255));
    
    QPen borderPen(QBrush(borderGradient), BORDER_WIDTH);
    borderPen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(selection);
    
    // 绘制虚线辅助线（九宫格）- 更美观的样式
    QPen dashPen(QColor(255, 255, 255, 80), 1, Qt::DotLine);
    painter.setPen(dashPen);
    
    int thirdWidth = selection.width() / 3;
    int thirdHeight = selection.height() / 3;
    
    // 垂直线
    painter.drawLine(selection.left() + thirdWidth, selection.top() + 1,
                    selection.left() + thirdWidth, selection.bottom() - 1);
    painter.drawLine(selection.left() + 2 * thirdWidth, selection.top() + 1,
                    selection.left() + 2 * thirdWidth, selection.bottom() - 1);
    
    // 水平线
    painter.drawLine(selection.left() + 1, selection.top() + thirdHeight,
                    selection.right() - 1, selection.top() + thirdHeight);
    painter.drawLine(selection.left() + 1, selection.top() + 2 * thirdHeight,
                    selection.right() - 1, selection.top() + 2 * thirdHeight);
}

void SelectionOverlay::drawSizeHint(QPainter &painter)
{
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty()) {
        return;
    }
    
    // 准备尺寸文本
    QString sizeText = QString("%1 × %2 px")
        .arg(selection.width())
        .arg(selection.height());
    
    // 设置字体
    QFont font = painter.font();
    font.setPixelSize(12);
    font.setWeight(QFont::Medium);
    painter.setFont(font);
    
    // 计算文本尺寸
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(sizeText) + 20;
    int textHeight = fm.height() + 12;
    
    // 计算提示框位置（在选区上方）
    int hintX = selection.left();
    int hintY = selection.top() - textHeight - 8;
    
    // 如果上方空间不够，显示在选区内部上方
    if (hintY < 0) {
        hintY = selection.top() + 8;
    }
    
    // 绘制背景（玻璃拟态效果）
    QRectF hintRect(hintX, hintY, textWidth, textHeight);
    
    // 背景阴影
    for (int i = 3; i > 0; i--) {
        QRectF shadowRect = hintRect.adjusted(-i, -i + 1, i, i + 1);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 15 * (4 - i)));
        painter.drawRoundedRect(shadowRect, 6 + i, 6 + i);
    }
    
    // 渐变背景
    QLinearGradient bgGradient(hintRect.topLeft(), hintRect.bottomLeft());
    bgGradient.setColorAt(0, QColor(64, 158, 255, 230));
    bgGradient.setColorAt(1, QColor(58, 142, 230, 230));
    
    painter.setBrush(bgGradient);
    painter.drawRoundedRect(hintRect, 6, 6);
    
    // 高光
    QPen highlightPen(QColor(255, 255, 255, 60), 1);
    painter.setPen(highlightPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(hintRect.left() + 6, hintRect.top() + 1,
                    hintRect.right() - 6, hintRect.top() + 1);
    
    // 绘制文本
    painter.setPen(Qt::white);
    painter.drawText(hintRect, Qt::AlignCenter, sizeText);
}

void SelectionOverlay::drawResizeHandles(QPainter &painter)
{
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty()) {
        return;
    }
    
    int halfSize = HANDLE_SIZE / 2;
    
    // 定义8个句柄位置
    QVector<QPoint> handlePositions = {
        selection.topLeft(),
        selection.topRight(),
        selection.bottomLeft(),
        selection.bottomRight(),
        QPoint(selection.center().x(), selection.top()),
        QPoint(selection.center().x(), selection.bottom()),
        QPoint(selection.left(), selection.center().y()),
        QPoint(selection.right(), selection.center().y())
    };
    
    // 绘制句柄
    for (const QPoint &pos : handlePositions) {
        QRectF handleRect(pos.x() - halfSize, pos.y() - halfSize, 
                         HANDLE_SIZE, HANDLE_SIZE);
        
        // 外部阴影
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 40));
        painter.drawEllipse(handleRect.adjusted(-1, 0, 1, 2));
        
        // 白色填充
        painter.setBrush(Qt::white);
        painter.drawEllipse(handleRect);
        
        // 蓝色边框
        QPen borderPen(QColor(64, 158, 255), 2);
        painter.setPen(borderPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(handleRect);
        
        // 内部高光
        QRectF innerRect = handleRect.adjusted(2, 2, -2, -2);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(64, 158, 255, 60));
        painter.drawEllipse(innerRect);
    }
}

void SelectionOverlay::updateToolBarPosition()
{
    if (!m_toolBar) {
        return;
    }
    
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty()) {
        m_toolBar->hide();
        return;
    }
    
    // 工具栏尺寸
    QSize toolBarSize = m_toolBar->sizeHint();
    
    // 计算位置（在选区右下角外侧）
    int toolBarX = selection.right() - toolBarSize.width();
    int toolBarY = selection.bottom() + 8;
    
    // 如果下方空间不够，显示在选区上方
    if (toolBarY + toolBarSize.height() > height()) {
        toolBarY = selection.top() - toolBarSize.height() - 8;
    }
    
    // 如果上方也不够，显示在选区内部
    if (toolBarY < 0) {
        toolBarY = selection.bottom() - toolBarSize.height() - 8;
    }
    
    // 确保不超出左边界
    if (toolBarX < 8) {
        toolBarX = 8;
    }
    
    // 确保不超出右边界
    if (toolBarX + toolBarSize.width() > width() - 8) {
        toolBarX = width() - toolBarSize.width() - 8;
    }
    
    m_toolBar->move(toolBarX, toolBarY);
    m_toolBar->show();
    m_toolBar->raise();
}

QRect SelectionOverlay::normalizedSelectionRect() const
{
    return QRect(m_startPoint, m_endPoint).normalized();
}

SelectionOverlay::ResizeHandle SelectionOverlay::hitTestHandle(const QPoint &pos) const
{
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty()) {
        return ResizeHandle::None;
    }
    
    int halfSize = HANDLE_SIZE / 2 + 2; // 稍微扩大点击区域
    
    // 检测四个角
    if (QRect(selection.topLeft() - QPoint(halfSize, halfSize), 
              QSize(HANDLE_SIZE + 4, HANDLE_SIZE + 4)).contains(pos)) {
        return ResizeHandle::TopLeft;
    }
    if (QRect(selection.topRight() - QPoint(halfSize, halfSize), 
              QSize(HANDLE_SIZE + 4, HANDLE_SIZE + 4)).contains(pos)) {
        return ResizeHandle::TopRight;
    }
    if (QRect(selection.bottomLeft() - QPoint(halfSize, halfSize), 
              QSize(HANDLE_SIZE + 4, HANDLE_SIZE + 4)).contains(pos)) {
        return ResizeHandle::BottomLeft;
    }
    if (QRect(selection.bottomRight() - QPoint(halfSize, halfSize), 
              QSize(HANDLE_SIZE + 4, HANDLE_SIZE + 4)).contains(pos)) {
        return ResizeHandle::BottomRight;
    }
    
    // 检测四条边
    if (QRect(selection.center().x() - halfSize, selection.top() - halfSize, 
              HANDLE_SIZE + 4, HANDLE_SIZE + 4).contains(pos)) {
        return ResizeHandle::Top;
    }
    if (QRect(selection.center().x() - halfSize, selection.bottom() - halfSize, 
              HANDLE_SIZE + 4, HANDLE_SIZE + 4).contains(pos)) {
        return ResizeHandle::Bottom;
    }
    if (QRect(selection.left() - halfSize, selection.center().y() - halfSize, 
              HANDLE_SIZE + 4, HANDLE_SIZE + 4).contains(pos)) {
        return ResizeHandle::Left;
    }
    if (QRect(selection.right() - halfSize, selection.center().y() - halfSize, 
              HANDLE_SIZE + 4, HANDLE_SIZE + 4).contains(pos)) {
        return ResizeHandle::Right;
    }
    
    return ResizeHandle::None;
}

Qt::CursorShape SelectionOverlay::getCursorForHandle(ResizeHandle handle) const
{
    switch (handle) {
    case ResizeHandle::TopLeft:
    case ResizeHandle::BottomRight:
        return Qt::SizeFDiagCursor;
    case ResizeHandle::TopRight:
    case ResizeHandle::BottomLeft:
        return Qt::SizeBDiagCursor;
    case ResizeHandle::Top:
    case ResizeHandle::Bottom:
        return Qt::SizeVerCursor;
    case ResizeHandle::Left:
    case ResizeHandle::Right:
        return Qt::SizeHorCursor;
    default:
        return Qt::CrossCursor;
    }
}

bool SelectionOverlay::isInsideSelection(const QPoint &pos) const
{
    QRect selection = normalizedSelectionRect();
    
    if (selection.isEmpty()) {
        return false;
    }
    
    // 缩小判定区域，排除边框和句柄区域
    QRect innerRect = selection.adjusted(HANDLE_SIZE, HANDLE_SIZE, 
                                          -HANDLE_SIZE, -HANDLE_SIZE);
    return innerRect.contains(pos);
}

void SelectionOverlay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        if (event->button() == Qt::RightButton) {
            cancel();
        }
        return;
    }
    
    QPoint pos = event->pos();
    m_lastMousePos = pos;
    
    // 如果当前是标注工具，开始绘制标注
    if (m_currentTool != AnnotationTool::Select) {
        m_state = SelectionState::Drawing;
        m_annotationStart = pos;
        m_annotationEnd = pos;
        qCDebug(logSelectionOverlay) << "开始绘制标注，起始点:" << pos;
        update();
        return;
    }
    
    // 选择工具模式下的原有逻辑
    if (m_state == SelectionState::Selected) {
        // 检测是否点击了调整句柄
        ResizeHandle handle = hitTestHandle(pos);
        
        if (handle != ResizeHandle::None) {
            m_state = SelectionState::Resizing;
            m_activeHandle = handle;
            qCDebug(logSelectionOverlay) << "开始调整选区大小";
            return;
        }
        
        // 检测是否点击了选区内部
        if (isInsideSelection(pos)) {
            m_state = SelectionState::Moving;
            qCDebug(logSelectionOverlay) << "开始移动选区";
            return;
        }
    }
    
    // 开始新的框选
    m_state = SelectionState::Selecting;
    m_startPoint = pos;
    m_endPoint = pos;
    m_toolBar->hide();
    
    qCDebug(logSelectionOverlay) << "开始框选，起始点:" << pos;
    
    update();
}

void SelectionOverlay::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    
    // 更新光标
    if (m_state == SelectionState::Selected) {
        ResizeHandle handle = hitTestHandle(pos);
        
        if (handle != ResizeHandle::None) {
            setCursor(getCursorForHandle(handle));
        } else if (isInsideSelection(pos)) {
            setCursor(Qt::SizeAllCursor);
        } else {
            setCursor(Qt::CrossCursor);
        }
    } else if (m_state == SelectionState::None) {
        setCursor(Qt::CrossCursor);
    }
    
    // 处理拖拽
    if (event->buttons() & Qt::LeftButton) {
        switch (m_state) {
        case SelectionState::Drawing:
            m_annotationEnd = pos;
            break;
            
        case SelectionState::Selecting:
            m_endPoint = pos;
            break;
            
        case SelectionState::Moving: {
            QPoint delta = pos - m_lastMousePos;
            m_startPoint += delta;
            m_endPoint += delta;
            m_lastMousePos = pos;
            updateToolBarPosition();
            break;
        }
            
        case SelectionState::Resizing: {
            QRect selection = normalizedSelectionRect();
            QPoint delta = pos - m_lastMousePos;
            
            switch (m_activeHandle) {
            case ResizeHandle::TopLeft:
                m_startPoint = selection.topLeft() + delta;
                m_endPoint = selection.bottomRight();
                break;
            case ResizeHandle::TopRight:
                m_startPoint = QPoint(selection.left(), selection.top() + delta.y());
                m_endPoint = QPoint(selection.right() + delta.x(), selection.bottom());
                break;
            case ResizeHandle::BottomLeft:
                m_startPoint = QPoint(selection.left() + delta.x(), selection.top());
                m_endPoint = QPoint(selection.right(), selection.bottom() + delta.y());
                break;
            case ResizeHandle::BottomRight:
                m_startPoint = selection.topLeft();
                m_endPoint = selection.bottomRight() + delta;
                break;
            case ResizeHandle::Top:
                m_startPoint = QPoint(selection.left(), selection.top() + delta.y());
                m_endPoint = selection.bottomRight();
                break;
            case ResizeHandle::Bottom:
                m_startPoint = selection.topLeft();
                m_endPoint = QPoint(selection.right(), selection.bottom() + delta.y());
                break;
            case ResizeHandle::Left:
                m_startPoint = QPoint(selection.left() + delta.x(), selection.top());
                m_endPoint = selection.bottomRight();
                break;
            case ResizeHandle::Right:
                m_startPoint = selection.topLeft();
                m_endPoint = QPoint(selection.right() + delta.x(), selection.bottom());
                break;
            default:
                break;
            }
            
            m_lastMousePos = pos;
            updateToolBarPosition();
            break;
        }
            
        default:
            break;
        }
        
        update();
    }
}

void SelectionOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    
    // 处理标注绘制完成
    if (m_state == SelectionState::Drawing) {
        // 检查标注是否有效（有一定大小）
        int dx = m_annotationEnd.x() - m_annotationStart.x();
        int dy = m_annotationEnd.y() - m_annotationStart.y();
        int distance = qSqrt(dx * dx + dy * dy);
        
        if (distance >= MIN_SELECTION_SIZE) {
            Annotation annotation;
            annotation.type = m_currentTool;
            annotation.startPoint = m_annotationStart;
            annotation.endPoint = m_annotationEnd;
            m_annotations.append(annotation);
            
            qCInfo(logSelectionOverlay) << "标注完成，类型:" 
                << (m_currentTool == AnnotationTool::Rectangle ? "矩形" : "箭头")
                << "起始点:" << m_annotationStart 
                << "结束点:" << m_annotationEnd;
        } else {
            qCDebug(logSelectionOverlay) << "标注太小，已忽略";
        }
        
        // 保持当前工具，继续等待下一次绘制
        m_state = SelectionState::Selected;
        update();
        return;
    }
    
    if (m_state == SelectionState::Selecting ||
        m_state == SelectionState::Moving ||
        m_state == SelectionState::Resizing) {
        
        QRect selection = normalizedSelectionRect();
        
        // 检查选区是否有效
        if (selection.width() >= MIN_SELECTION_SIZE && 
            selection.height() >= MIN_SELECTION_SIZE) {
            m_state = SelectionState::Selected;
            updateToolBarPosition();
            
            qCInfo(logSelectionOverlay) << "框选完成，区域:" << selection;
        } else {
            // 选区太小，重置
            m_state = SelectionState::None;
            m_startPoint = QPoint();
            m_endPoint = QPoint();
            m_toolBar->hide();
            
            qCDebug(logSelectionOverlay) << "选区太小，已重置";
        }
        
        m_activeHandle = ResizeHandle::None;
        update();
    }
}

void SelectionOverlay::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_state == SelectionState::Selected && 
            isInsideSelection(event->pos())) {
            confirmCapture();
        }
    }
}

void SelectionOverlay::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        cancel();
        break;
        
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_state == SelectionState::Selected) {
            confirmCapture();
        }
        break;
        
    case Qt::Key_S:
        if (event->modifiers() & Qt::ControlModifier) {
            if (m_state == SelectionState::Selected) {
                saveToFile();
            }
        }
        break;
        
    case Qt::Key_C:
        if (event->modifiers() & Qt::ControlModifier) {
            if (m_state == SelectionState::Selected) {
                confirmCapture();
            }
        }
        break;
        
    case Qt::Key_A:
        // Ctrl+A 全选
        if (event->modifiers() & Qt::ControlModifier) {
            m_startPoint = QPoint(0, 0);
            m_endPoint = QPoint(width(), height());
            m_state = SelectionState::Selected;
            updateToolBarPosition();
            update();
        }
        break;
        
    default:
        QWidget::keyPressEvent(event);
    }
}

void SelectionOverlay::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // 获取焦点
    activateWindow();
    setFocus();
    
    // 设置窗口大小为虚拟桌面大小
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        // 获取所有屏幕的虚拟几何区域
        QRect virtualGeometry;
        for (QScreen *s : QGuiApplication::screens()) {
            if (virtualGeometry.isNull()) {
                virtualGeometry = s->geometry();
            } else {
                virtualGeometry = virtualGeometry.united(s->geometry());
            }
        }
        
        setGeometry(virtualGeometry);
    }
    
    qCDebug(logSelectionOverlay) << "覆盖层显示，尺寸:" << size();
}

void SelectionOverlay::confirmCapture()
{
    qCInfo(logSelectionOverlay) << "确认截图";
    
    QPixmap selectedPixmap = getSelectedPixmap();
    
    if (!selectedPixmap.isNull()) {
        emit captureConfirmed(selectedPixmap);
    } else {
        qCWarning(logSelectionOverlay) << "无法获取选区图像";
    }
}

void SelectionOverlay::saveToFile()
{
    qCInfo(logSelectionOverlay) << "请求保存到文件";
    
    QPixmap selectedPixmap = getSelectedPixmap();
    
    if (!selectedPixmap.isNull()) {
        emit saveRequested(selectedPixmap);
    } else {
        qCWarning(logSelectionOverlay) << "无法获取选区图像";
    }
}

void SelectionOverlay::cancel()
{
    qCInfo(logSelectionOverlay) << "取消截图";
    emit captureCancelled();
}

void SelectionOverlay::onSelectTool()
{
    m_currentTool = AnnotationTool::Select;
    qCDebug(logSelectionOverlay) << "切换到选择工具";
}

void SelectionOverlay::onRectangleTool()
{
    m_currentTool = AnnotationTool::Rectangle;
    qCDebug(logSelectionOverlay) << "切换到矩形标注工具";
}

void SelectionOverlay::onArrowTool()
{
    m_currentTool = AnnotationTool::Arrow;
    qCDebug(logSelectionOverlay) << "切换到箭头标注工具";
}

void SelectionOverlay::drawAnnotations(QPainter &painter)
{
    for (const Annotation &annotation : m_annotations) {
        drawAnnotation(painter, annotation);
    }
}

void SelectionOverlay::drawAnnotation(QPainter &painter, const Annotation &annotation)
{
    // 设置画笔（红色，固定粗细）
    QPen pen(QColor(255, 0, 0), ANNOTATION_LINE_WIDTH);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    
    if (annotation.type == AnnotationTool::Rectangle) {
        drawRectangleAnnotation(painter, annotation.startPoint, annotation.endPoint);
    } else if (annotation.type == AnnotationTool::Arrow) {
        drawArrowAnnotation(painter, annotation.startPoint, annotation.endPoint);
    }
}

void SelectionOverlay::drawRectangleAnnotation(QPainter &painter, const QPoint &start, const QPoint &end)
{
    QRect rect = QRect(start, end).normalized();
    painter.drawRect(rect);
}

void SelectionOverlay::drawArrowAnnotation(QPainter &painter, const QPoint &start, const QPoint &end)
{
    // 绘制箭头线
    painter.drawLine(start, end);
    
    // 计算箭头头部
    double angle = std::atan2(end.y() - start.y(), end.x() - start.x());
    
    QPointF p1 = end - QPointF(
        ARROW_HEAD_SIZE * std::cos(angle - M_PI / 6),
        ARROW_HEAD_SIZE * std::sin(angle - M_PI / 6)
    );
    QPointF p2 = end - QPointF(
        ARROW_HEAD_SIZE * std::cos(angle + M_PI / 6),
        ARROW_HEAD_SIZE * std::sin(angle + M_PI / 6)
    );
    
    // 绘制箭头头部（填充）
    painter.setBrush(QColor(255, 0, 0));
    QPainterPath arrowPath;
    arrowPath.moveTo(end);
    arrowPath.lineTo(p1);
    arrowPath.lineTo(p2);
    arrowPath.closeSubpath();
    painter.drawPath(arrowPath);
    painter.setBrush(Qt::NoBrush);
}

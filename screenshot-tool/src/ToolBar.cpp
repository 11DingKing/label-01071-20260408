/**
 * @file ToolBar.cpp
 * @brief 工具栏类实现 - 现代简洁风格
 */

#include "ToolBar.h"
#include "StyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

// 定义日志分类
Q_LOGGING_CATEGORY(logToolBar, "screenshot.toolbar")

ToolBar::ToolBar(QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_confirmBtn(nullptr)
    , m_saveBtn(nullptr)
    , m_cancelBtn(nullptr)
{
    qCDebug(logToolBar) << "初始化工具栏";
    
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedHeight(48);
    
    setupUI();
    
    qCDebug(logToolBar) << "工具栏初始化完成";
}

ToolBar::~ToolBar()
{
    qCDebug(logToolBar) << "销毁工具栏";
}

QSize ToolBar::sizeHint() const
{
    return QSize(220, 48);
}

void ToolBar::setupShadow()
{
    // 不使用，阴影在 paintEvent 中绘制
}

void ToolBar::setupUI()
{
    // 主布局
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(16, 8, 16, 8);  // 左右16，上下8
    m_layout->setSpacing(10);  // 按钮间距10px
    
    // 创建三个按钮 - 纯文字，无图标
    m_cancelBtn = createActionButton("取消", ButtonType::Danger);
    m_cancelBtn->setToolTip("取消截图 (ESC)");
    
    m_saveBtn = createActionButton("保存", ButtonType::Primary);
    m_saveBtn->setToolTip("保存到文件 (Ctrl+S)");
    
    m_confirmBtn = createActionButton("复制", ButtonType::Success);
    m_confirmBtn->setToolTip("复制到剪贴板 (Enter)");
    
    // 连接信号
    connect(m_cancelBtn, &QPushButton::clicked, this, &ToolBar::cancelClicked);
    connect(m_saveBtn, &QPushButton::clicked, this, &ToolBar::saveClicked);
    connect(m_confirmBtn, &QPushButton::clicked, this, &ToolBar::confirmClicked);
    
    // 添加到布局
    m_layout->addWidget(m_cancelBtn);
    m_layout->addWidget(m_saveBtn);
    m_layout->addWidget(m_confirmBtn);
    
    adjustSize();
}

QPushButton* ToolBar::createButton(const QString &text, 
                                   const QString &tooltip,
                                   ButtonType type)
{
    QPushButton* btn = createActionButton(text, type);
    btn->setToolTip(tooltip);
    return btn;
}

QPushButton* ToolBar::createActionButton(const QString &text, ButtonType type)
{
    QPushButton *button = new QPushButton(text, this);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(60, 32);  // 固定尺寸 60x32
    
    QString style;
    
    switch (type) {
    case ButtonType::Danger:
        style = R"(
            QPushButton {
                background-color: #EF4444;
                color: #FFFFFF;
                border: none;
                border-radius: 6px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: #F87171;
            }
            QPushButton:pressed {
                background-color: #DC2626;
            }
        )";
        break;
        
    case ButtonType::Primary:
        style = R"(
            QPushButton {
                background-color: #3B82F6;
                color: #FFFFFF;
                border: none;
                border-radius: 6px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: #60A5FA;
            }
            QPushButton:pressed {
                background-color: #2563EB;
            }
        )";
        break;
        
    case ButtonType::Success:
        style = R"(
            QPushButton {
                background-color: #22C55E;
                color: #FFFFFF;
                border: none;
                border-radius: 6px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: #4ADE80;
            }
            QPushButton:pressed {
                background-color: #16A34A;
            }
        )";
        break;
        
    case ButtonType::Secondary:
    default:
        style = R"(
            QPushButton {
                background-color: rgba(255, 255, 255, 0.1);
                color: #FFFFFF;
                border: 1px solid rgba(255, 255, 255, 0.2);
                border-radius: 6px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.2);
            }
            QPushButton:pressed {
                background-color: rgba(255, 255, 255, 0.05);
            }
        )";
        break;
    }
    
    button->setStyleSheet(style);
    return button;
}

void ToolBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // 背景区域
    QRectF bgRect = rect().adjusted(6, 4, -6, -6);
    const int radius = 12;
    
    // 柔和阴影（多层渐变）
    painter.setPen(Qt::NoPen);
    for (int i = 6; i > 0; --i) {
        int alpha = 8 * (7 - i);
        painter.setBrush(QColor(0, 0, 0, alpha));
        painter.drawRoundedRect(bgRect.adjusted(-i, -i + 2, i, i), radius + i, radius + i);
    }
    
    // 毛玻璃背景 - 深色半透明
    QLinearGradient bgGradient(bgRect.topLeft(), bgRect.bottomLeft());
    bgGradient.setColorAt(0, QColor(45, 45, 50, 250));
    bgGradient.setColorAt(1, QColor(30, 30, 35, 250));
    painter.setBrush(bgGradient);
    painter.drawRoundedRect(bgRect, radius, radius);
    
    // 内发光效果（顶部亮边）
    QLinearGradient innerGlow(bgRect.topLeft(), QPointF(bgRect.left(), bgRect.top() + 20));
    innerGlow.setColorAt(0, QColor(255, 255, 255, 25));
    innerGlow.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(innerGlow);
    painter.drawRoundedRect(bgRect, radius, radius);
    
    // 精细边框
    painter.setPen(QPen(QColor(255, 255, 255, 20), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(bgRect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

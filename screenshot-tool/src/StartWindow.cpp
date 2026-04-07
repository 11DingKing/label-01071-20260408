/**
 * @file StartWindow.cpp
 * @brief 启动窗口类实现
 */

#include "StartWindow.h"
#include "StyleManager.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

StartWindow::StartWindow(QWidget *parent)
    : QWidget(parent)
    , m_captureBtn(nullptr)
    , m_closeBtn(nullptr)
    , m_quitBtn(nullptr)
{
    setWindowTitle("截图工具");
    setFixedSize(280, 200);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUI();
}

void StartWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    auto *style = StyleManager::instance();
    const int shadowOffset = style->shadowBlurRadius();
    const int radius = style->borderRadius() + 6;  // 稍大的圆角
    
    QRectF bgRect = rect().adjusted(shadowOffset, shadowOffset, -shadowOffset, -shadowOffset);
    
    // 绘制阴影
    for (int i = shadowOffset; i > 0; i--) {
        QRectF shadowRect = bgRect.adjusted(-i, -i, i, i);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 2 * (shadowOffset - i + 1)));
        painter.drawRoundedRect(shadowRect, radius + i/3, radius + i/3);
    }
    
    // 绘制白色背景
    painter.setBrush(style->cardBackgroundColor());
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(bgRect, radius, radius);
    
    // 绘制细边框
    painter.setPen(QPen(style->borderColor(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(bgRect, radius, radius);
}

void StartWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
}

void StartWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPos);
    }
}

void StartWindow::setupUI()
{
    auto *style = StyleManager::instance();
    const int margin = style->spacingLarge() + 4;
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(margin, margin, margin, margin - 4);
    mainLayout->setSpacing(style->spacingMedium());
    
    // 添加标题区域
    mainLayout->addWidget(createTitleSection());
    mainLayout->addStretch();
    
    // 添加开始按钮
    m_captureBtn = createCaptureButton();
    mainLayout->addWidget(m_captureBtn);
    
    // 添加底部区域
    mainLayout->addWidget(createBottomSection());
    
    // 连接信号
    connect(m_captureBtn, &QPushButton::clicked, this, &StartWindow::startCaptureRequested);
    connect(m_closeBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(m_quitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
}

QWidget* StartWindow::createTitleSection()
{
    auto *style = StyleManager::instance();
    
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(style->spacingSmall() + 2);
    
    // 图标（使用渐变色圆形）
    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(36, 36);
    iconLabel->setStyleSheet(style->startWindowIconStyleSheet());
    
    // 标题文字区域
    QVBoxLayout *titleTextLayout = new QVBoxLayout();
    titleTextLayout->setSpacing(2);
    
    QLabel *titleLabel = new QLabel("截图工具");
    titleLabel->setStyleSheet(style->startWindowTitleStyleSheet());
    
    QLabel *subtitleLabel = new QLabel("框选屏幕任意区域");
    subtitleLabel->setStyleSheet(style->startWindowSubtitleStyleSheet());
    
    titleTextLayout->addWidget(titleLabel);
    titleTextLayout->addWidget(subtitleLabel);
    
    layout->addWidget(iconLabel);
    layout->addLayout(titleTextLayout);
    layout->addStretch();
    
    // 关闭按钮
    m_closeBtn = new QPushButton("×");
    m_closeBtn->setFixedSize(24, 24);
    m_closeBtn->setCursor(Qt::PointingHandCursor);
    m_closeBtn->setStyleSheet(style->closeButtonStyleSheet());
    layout->addWidget(m_closeBtn);
    
    return container;
}

QPushButton* StartWindow::createCaptureButton()
{
    auto *style = StyleManager::instance();
    
    QPushButton *button = new QPushButton("开始截图");
    button->setFixedHeight(42);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(style->startWindowCaptureButtonStyleSheet());
    
    return button;
}

QWidget* StartWindow::createBottomSection()
{
    auto *style = StyleManager::instance();
    
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    
    layout->addStretch();
    
    // 退出按钮
    m_quitBtn = new QPushButton("退出");
    m_quitBtn->setCursor(Qt::PointingHandCursor);
    m_quitBtn->setStyleSheet(style->startWindowQuitButtonStyleSheet());
    layout->addWidget(m_quitBtn);
    
    return container;
}

/**
 * @file StyleManager.cpp
 * @brief 样式管理器实现
 */

#include "StyleManager.h"

// 初始化静态成员
StyleManager* StyleManager::s_instance = nullptr;

StyleManager* StyleManager::instance()
{
    if (!s_instance) {
        s_instance = new StyleManager();
    }
    return s_instance;
}

StyleManager::StyleManager(QObject *parent)
    : QObject(parent)
    , m_theme(Theme::Light)
{
}

void StyleManager::setTheme(Theme theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        emit themeChanged(theme);
    }
}

StyleManager::Theme StyleManager::currentTheme() const
{
    return m_theme;
}

// ===== 颜色定义实现 =====

QColor StyleManager::primaryColor() const
{
    return QColor("#409EFF");
}

QColor StyleManager::primaryHoverColor() const
{
    return QColor("#66B1FF");
}

QColor StyleManager::primaryPressedColor() const
{
    return QColor("#3A8EE6");
}

QColor StyleManager::successColor() const
{
    return QColor("#67C23A");
}

QColor StyleManager::warningColor() const
{
    return QColor("#E6A23C");
}

QColor StyleManager::dangerColor() const
{
    return QColor("#F56C6C");
}

QColor StyleManager::infoColor() const
{
    return QColor("#909399");
}

QColor StyleManager::backgroundColor() const
{
    return m_theme == Theme::Light ? QColor("#F5F7FA") : QColor("#1A1A2E");
}

QColor StyleManager::cardBackgroundColor() const
{
    return m_theme == Theme::Light ? QColor("#FFFFFF") : QColor("#2D2D44");
}

QColor StyleManager::maskColor() const
{
    return QColor(0, 0, 0, 128);
}

QColor StyleManager::borderColor() const
{
    return m_theme == Theme::Light ? QColor("#DCDFE6") : QColor("#4A4A5A");
}

QColor StyleManager::textPrimaryColor() const
{
    return m_theme == Theme::Light ? QColor("#303133") : QColor("#E4E4E7");
}

QColor StyleManager::textSecondaryColor() const
{
    return m_theme == Theme::Light ? QColor("#606266") : QColor("#A0A0A8");
}

QColor StyleManager::textPlaceholderColor() const
{
    return m_theme == Theme::Light ? QColor("#C0C4CC") : QColor("#6B6B78");
}

// ===== 尺寸定义实现 =====

int StyleManager::borderRadius() const
{
    return 8;
}

int StyleManager::borderWidth() const
{
    return 2;
}

int StyleManager::shadowBlurRadius() const
{
    return 12;
}

int StyleManager::spacingSmall() const
{
    return 8;
}

int StyleManager::spacingMedium() const
{
    return 16;
}

int StyleManager::spacingLarge() const
{
    return 24;
}

// ===== 字体定义实现 =====

QFont StyleManager::titleFont() const
{
    QFont font;
    font.setPixelSize(18);
    font.setWeight(QFont::Bold);
    return font;
}

QFont StyleManager::bodyFont() const
{
    QFont font;
    font.setPixelSize(14);
    font.setWeight(QFont::Normal);
    return font;
}

QFont StyleManager::smallFont() const
{
    QFont font;
    font.setPixelSize(12);
    font.setWeight(QFont::Normal);
    return font;
}

// ===== 样式表生成实现 =====

QString StyleManager::globalStyleSheet() const
{
    return QString(R"(
        * {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
        }
        
        QToolTip {
            background-color: rgba(48, 49, 51, 0.95);
            color: #FFFFFF;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 12px;
        }
        
        QMessageBox {
            background-color: %1;
        }
        
        QMessageBox QLabel {
            color: %2;
            font-size: 14px;
            line-height: 1.6;
        }
    )").arg(cardBackgroundColor().name())
       .arg(textPrimaryColor().name());
}

QString StyleManager::toolBarStyleSheet() const
{
    return QString(R"(
        QWidget#ToolBar {
            background: transparent;
        }
    )");
}

QString StyleManager::primaryButtonStyleSheet() const
{
    return QString(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %1, stop:1 %2);
            color: white;
            border: none;
            border-radius: %3px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %4, stop:1 %1);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 %2, stop:1 %5);
        }
        QPushButton:disabled {
            background: %6;
            color: rgba(255, 255, 255, 0.6);
        }
    )").arg(primaryColor().name())
       .arg(primaryPressedColor().name())
       .arg(borderRadius())
       .arg(primaryHoverColor().name())
       .arg(primaryPressedColor().darker(110).name())
       .arg(primaryColor().lighter(130).name());
}

QString StyleManager::secondaryButtonStyleSheet() const
{
    return QString(R"(
        QPushButton {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: %4px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: %5;
            border-color: %6;
            color: %6;
        }
        QPushButton:pressed {
            background: %7;
        }
    )").arg(cardBackgroundColor().name())
       .arg(textPrimaryColor().name())
       .arg(borderColor().name())
       .arg(borderRadius())
       .arg(primaryColor().lighter(190).name())
       .arg(primaryColor().name())
       .arg(primaryColor().lighter(180).name());
}

QString StyleManager::dangerButtonStyleSheet() const
{
    return QString(R"(
        QPushButton {
            background: %1;
            color: white;
            border: none;
            border-radius: %2px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: %3;
        }
        QPushButton:pressed {
            background: %4;
        }
    )").arg(dangerColor().name())
       .arg(borderRadius())
       .arg(dangerColor().lighter(110).name())
       .arg(dangerColor().darker(110).name());
}

QString StyleManager::tooltipStyleSheet() const
{
    return QString(R"(
        QToolTip {
            background-color: rgba(48, 49, 51, 0.95);
            color: #FFFFFF;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 12px;
        }
    )");
}

// ===== StartWindow 样式实现 =====

QString StyleManager::startWindowIconStyleSheet() const
{
    return QString(R"(
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #667eea, stop:1 #764ba2);
        border-radius: 18px;
    )");
}

QString StyleManager::startWindowTitleStyleSheet() const
{
    return QString(R"(
        font-size: 16px;
        font-weight: 600;
        color: %1;
    )").arg(textPrimaryColor().name());
}

QString StyleManager::startWindowSubtitleStyleSheet() const
{
    return QString("font-size: 12px; color: %1;")
        .arg(textSecondaryColor().name());
}

QString StyleManager::startWindowCaptureButtonStyleSheet() const
{
    return QString(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #5a6fd6, stop:1 #6a4190);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4e5fc2, stop:1 #5e3780);
        }
    )");
}

QString StyleManager::startWindowQuitButtonStyleSheet() const
{
    return QString(R"(
        QPushButton {
            background: transparent;
            color: %1;
            border: none;
            font-size: 12px;
            padding: 4px 8px;
        }
        QPushButton:hover {
            color: %2;
        }
    )").arg(textPlaceholderColor().name())
       .arg(textSecondaryColor().name());
}

QString StyleManager::closeButtonStyleSheet() const
{
    return QString(R"(
        QPushButton {
            background: transparent;
            color: %1;
            border: none;
            font-size: 18px;
            font-weight: 300;
        }
        QPushButton:hover {
            color: %2;
        }
    )").arg(textPlaceholderColor().name())
       .arg(dangerColor().name());
}

// ===== ToolBar 样式实现 =====

QString StyleManager::toolBarButtonStyleSheet(const QString &type, int radius) const
{
    if (type == "primary") {
        return QString(R"(
            QPushButton {
                background: %1;
                color: white;
                border: none;
                border-radius: %2px;
                font-size: 20px;
                font-weight: 300;
            }
            QPushButton:hover {
                background: %3;
            }
            QPushButton:pressed {
                background: %4;
            }
        )").arg(primaryColor().name())
           .arg(radius)
           .arg(primaryHoverColor().name())
           .arg(primaryPressedColor().name());
    } 
    else if (type == "success") {
        return QString(R"(
            QPushButton {
                background: %1;
                color: white;
                border: none;
                border-radius: %2px;
                font-size: 24px;
                font-weight: 300;
            }
            QPushButton:hover {
                background: %3;
            }
            QPushButton:pressed {
                background: %4;
            }
        )").arg(successColor().name())
           .arg(radius)
           .arg(successColor().lighter(115).name())
           .arg(successColor().darker(110).name());
    }
    else if (type == "danger") {
        return QString(R"(
            QPushButton {
                background: %1;
                color: white;
                border: none;
                border-radius: %2px;
                font-size: 20px;
                font-weight: 300;
            }
            QPushButton:hover {
                background: %3;
            }
            QPushButton:pressed {
                background: %4;
            }
        )").arg(dangerColor().name())
           .arg(radius)
           .arg(dangerColor().lighter(115).name())
           .arg(dangerColor().darker(110).name());
    }
    else {
        // secondary
        return QString(R"(
            QPushButton {
                background: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: %4px;
                font-size: 18px;
                font-weight: 300;
            }
            QPushButton:hover {
                background: %5;
                color: %6;
                border-color: %6;
            }
            QPushButton:pressed {
                background: %7;
            }
        )").arg(cardBackgroundColor().name())
           .arg(textSecondaryColor().name())
           .arg(borderColor().name())
           .arg(radius)
           .arg(primaryColor().lighter(190).name())
           .arg(primaryColor().name())
           .arg(primaryColor().lighter(180).name());
    }
}

QString StyleManager::toolBarLabelStyleSheet() const
{
    return QString("color: %1; font-size: 11px;")
        .arg(textSecondaryColor().name());
}

// ===== 渐变色实现 =====

QLinearGradient StyleManager::primaryGradient(const QPointF &start, const QPointF &end) const
{
    QLinearGradient gradient(start, end);
    gradient.setColorAt(0, primaryColor());
    gradient.setColorAt(1, primaryPressedColor());
    return gradient;
}

QLinearGradient StyleManager::glassGradient(const QPointF &start, const QPointF &end) const
{
    QLinearGradient gradient(start, end);
    gradient.setColorAt(0, QColor(255, 255, 255, 240));
    gradient.setColorAt(0.5, QColor(255, 255, 255, 220));
    gradient.setColorAt(1, QColor(255, 255, 255, 200));
    return gradient;
}

/**
 * @file test_style_manager.cpp
 * @brief StyleManager 类单元测试
 */

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QApplication>

#include "StyleManager.h"

class TestStyleManager : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief 初始化测试环境
     */
    void initTestCase()
    {
        qDebug() << "初始化 StyleManager 测试";
    }
    
    /**
     * @brief 清理测试环境
     */
    void cleanupTestCase()
    {
        qDebug() << "清理 StyleManager 测试";
    }
    
    /**
     * @brief 测试单例模式
     */
    void testSingleton()
    {
        StyleManager *manager1 = StyleManager::instance();
        StyleManager *manager2 = StyleManager::instance();
        
        QVERIFY(manager1 != nullptr);
        QVERIFY(manager2 != nullptr);
        QCOMPARE(manager1, manager2);
    }
    
    /**
     * @brief 测试主题切换
     */
    void testThemeSwitch()
    {
        StyleManager *manager = StyleManager::instance();
        
        // 设置浅色主题
        manager->setTheme(StyleManager::Theme::Light);
        QCOMPARE(manager->currentTheme(), StyleManager::Theme::Light);
        
        // 设置深色主题
        manager->setTheme(StyleManager::Theme::Dark);
        QCOMPARE(manager->currentTheme(), StyleManager::Theme::Dark);
        
        // 恢复浅色主题
        manager->setTheme(StyleManager::Theme::Light);
    }
    
    /**
     * @brief 测试主题切换信号
     */
    void testThemeChangeSignal()
    {
        StyleManager *manager = StyleManager::instance();
        manager->setTheme(StyleManager::Theme::Light);
        
        QSignalSpy spy(manager, &StyleManager::themeChanged);
        
        manager->setTheme(StyleManager::Theme::Dark);
        
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).value<StyleManager::Theme>(), 
                 StyleManager::Theme::Dark);
        
        // 设置相同主题不应触发信号
        manager->setTheme(StyleManager::Theme::Dark);
        QCOMPARE(spy.count(), 0);
        
        manager->setTheme(StyleManager::Theme::Light);
    }
    
    /**
     * @brief 测试颜色值有效性
     */
    void testColorValidity()
    {
        StyleManager *manager = StyleManager::instance();
        
        // 主题色
        QVERIFY(manager->primaryColor().isValid());
        QVERIFY(manager->primaryHoverColor().isValid());
        QVERIFY(manager->primaryPressedColor().isValid());
        
        // 状态色
        QVERIFY(manager->successColor().isValid());
        QVERIFY(manager->warningColor().isValid());
        QVERIFY(manager->dangerColor().isValid());
        QVERIFY(manager->infoColor().isValid());
        
        // 背景色
        QVERIFY(manager->backgroundColor().isValid());
        QVERIFY(manager->cardBackgroundColor().isValid());
        QVERIFY(manager->maskColor().isValid());
        
        // 文字色
        QVERIFY(manager->textPrimaryColor().isValid());
        QVERIFY(manager->textSecondaryColor().isValid());
        QVERIFY(manager->textPlaceholderColor().isValid());
        
        // 边框色
        QVERIFY(manager->borderColor().isValid());
    }
    
    /**
     * @brief 测试主题相关颜色变化
     */
    void testThemeRelatedColors()
    {
        StyleManager *manager = StyleManager::instance();
        
        // 浅色主题
        manager->setTheme(StyleManager::Theme::Light);
        QColor lightBg = manager->backgroundColor();
        QColor lightCard = manager->cardBackgroundColor();
        QColor lightText = manager->textPrimaryColor();
        
        // 深色主题
        manager->setTheme(StyleManager::Theme::Dark);
        QColor darkBg = manager->backgroundColor();
        QColor darkCard = manager->cardBackgroundColor();
        QColor darkText = manager->textPrimaryColor();
        
        // 颜色应该不同
        QVERIFY(lightBg != darkBg);
        QVERIFY(lightCard != darkCard);
        QVERIFY(lightText != darkText);
        
        // 浅色主题背景应该更亮
        QVERIFY(lightBg.lightness() > darkBg.lightness());
        QVERIFY(lightCard.lightness() > darkCard.lightness());
        
        // 恢复
        manager->setTheme(StyleManager::Theme::Light);
    }
    
    /**
     * @brief 测试尺寸常量
     */
    void testDimensions()
    {
        StyleManager *manager = StyleManager::instance();
        
        QVERIFY(manager->borderRadius() > 0);
        QVERIFY(manager->borderWidth() > 0);
        QVERIFY(manager->shadowBlurRadius() > 0);
        
        QVERIFY(manager->spacingSmall() > 0);
        QVERIFY(manager->spacingMedium() > manager->spacingSmall());
        QVERIFY(manager->spacingLarge() > manager->spacingMedium());
    }
    
    /**
     * @brief 测试字体设置
     */
    void testFonts()
    {
        StyleManager *manager = StyleManager::instance();
        
        QFont titleFont = manager->titleFont();
        QFont bodyFont = manager->bodyFont();
        QFont smallFont = manager->smallFont();
        
        // 字体大小应该递减
        QVERIFY(titleFont.pixelSize() > bodyFont.pixelSize());
        QVERIFY(bodyFont.pixelSize() > smallFont.pixelSize());
        
        // 标题字体应该是粗体
        QVERIFY(titleFont.weight() >= QFont::Bold);
    }
    
    /**
     * @brief 测试样式表生成
     */
    void testStyleSheets()
    {
        StyleManager *manager = StyleManager::instance();
        
        // 全局样式表
        QString globalStyle = manager->globalStyleSheet();
        QVERIFY(!globalStyle.isEmpty());
        QVERIFY(globalStyle.contains("QToolTip"));
        
        // 按钮样式表
        QString primaryBtnStyle = manager->primaryButtonStyleSheet();
        QVERIFY(!primaryBtnStyle.isEmpty());
        QVERIFY(primaryBtnStyle.contains("QPushButton"));
        QVERIFY(primaryBtnStyle.contains(":hover"));
        QVERIFY(primaryBtnStyle.contains(":pressed"));
        
        QString secondaryBtnStyle = manager->secondaryButtonStyleSheet();
        QVERIFY(!secondaryBtnStyle.isEmpty());
        
        QString dangerBtnStyle = manager->dangerButtonStyleSheet();
        QVERIFY(!dangerBtnStyle.isEmpty());
        
        // 工具栏样式表
        QString toolbarStyle = manager->toolBarStyleSheet();
        QVERIFY(!toolbarStyle.isEmpty());
    }
    
    /**
     * @brief 测试渐变色生成
     */
    void testGradients()
    {
        StyleManager *manager = StyleManager::instance();
        
        QPointF start(0, 0);
        QPointF end(0, 100);
        
        QLinearGradient primaryGrad = manager->primaryGradient(start, end);
        QCOMPARE(primaryGrad.start(), start);
        QCOMPARE(primaryGrad.finalStop(), end);
        
        QLinearGradient glassGrad = manager->glassGradient(start, end);
        QCOMPARE(glassGrad.start(), start);
        QCOMPARE(glassGrad.finalStop(), end);
    }
    
    /**
     * @brief 测试主色调值
     */
    void testPrimaryColorValues()
    {
        StyleManager *manager = StyleManager::instance();
        
        QColor primary = manager->primaryColor();
        
        // 验证是蓝色系
        QVERIFY(primary.blue() > primary.red());
        QVERIFY(primary.blue() > primary.green());
        
        // 验证预期的颜色值 (#409EFF)
        QCOMPARE(primary.red(), 64);
        QCOMPARE(primary.green(), 158);
        QCOMPARE(primary.blue(), 255);
    }
    
    /**
     * @brief 测试遮罩色透明度
     */
    void testMaskColorAlpha()
    {
        StyleManager *manager = StyleManager::instance();
        
        QColor mask = manager->maskColor();
        
        // 遮罩应该是半透明的
        QVERIFY(mask.alpha() < 255);
        QVERIFY(mask.alpha() > 0);
        
        // 应该是黑色遮罩
        QCOMPARE(mask.red(), 0);
        QCOMPARE(mask.green(), 0);
        QCOMPARE(mask.blue(), 0);
    }
};

QTEST_MAIN(TestStyleManager)
#include "test_style_manager.moc"

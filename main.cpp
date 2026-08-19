#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>

#include "src/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Force standard dark Fusion style across all platforms
    app.setStyle(QStyleFactory::create("Fusion"));

    // Create Dark Color Palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(40, 40, 40));
    darkPalette.setColor(QPalette::WindowText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
    darkPalette.setColor(QPalette::AlternateBase, QColor(26, 26, 26));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::ToolTipText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Text, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Button, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(13, 71, 161));
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    // Disabled state colors
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));

    app.setPalette(darkPalette);

    MainWindow window;
    window.show();

    return app.exec();
}
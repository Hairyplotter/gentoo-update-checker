#include <QApplication>
#include "systemtraywidget.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("Gentoo Update Checker");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setApplicationDisplayName("Gentoo Update Checker");
    QApplication::setOrganizationName("gentoo");

    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "System tray is not available!";
    }

    // Create and show the main widget
    SystemTrayWidget widget;
    widget.show();

    return app.exec();
}

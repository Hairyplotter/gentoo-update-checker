#include "systemtraywidget.h"
#include "updatechecker.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QEvent>
#include <QCloseEvent>

SystemTrayWidget::SystemTrayWidget(QWidget *parent)
    : QMainWindow(parent),
      m_hasUpdates(false),
      m_updateCount(0),
      m_syncInProgress(false)
{
    setWindowTitle("Gentoo Update Checker");
    setWindowIcon(QIcon(":/icons/gentoo-logo.png"));
    
    // Set minimum size for the window
    setMinimumSize(400, 300);

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    QLabel *infoLabel = new QLabel(
        "<h2>Gentoo Update Checker</h2>"
        "<p>This application monitors your Gentoo Linux system for package updates.</p>"
        "<p>The application minimizes to the system tray. Use the menu options to:</p>"
        "<ul>"
        "<li><b>Sync Database:</b> Update the package database</li>"
        "<li><b>Check Updates:</b> Check for available package updates</li>"
        "<li><b>Open Terminal:</b> Open a terminal for manual package management</li>"
        "</ul>",
        this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    layout->addStretch();
    
    setCentralWidget(centralWidget);

    // Create update checker
    m_updateChecker = std::make_unique<UpdateChecker>(this);

    // Connect signals
    connect(m_updateChecker.get(), &UpdateChecker::syncStarted, this, &SystemTrayWidget::onSyncStarted);
    connect(m_updateChecker.get(), &UpdateChecker::syncFinished, this, &SystemTrayWidget::onSyncFinished);
    connect(m_updateChecker.get(), &UpdateChecker::updatesAvailable, this, &SystemTrayWidget::onUpdatesAvailable);
    connect(m_updateChecker.get(), &UpdateChecker::noUpdatesAvailable, this, &SystemTrayWidget::onNoUpdatesAvailable);
    connect(m_updateChecker.get(), &UpdateChecker::errorOccurred, this, &SystemTrayWidget::onErrorOccurred);

    createTrayIcon();
    createMenu();

    // Start the update checker
    m_updateChecker->start();
}

SystemTrayWidget::~SystemTrayWidget()
{
    if (m_updateChecker) {
        m_updateChecker->stop();
    }
}

void SystemTrayWidget::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    setTrayIcon("idle");
    
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayWidget::onTrayIconActivated);
    m_trayIcon->show();
}

void SystemTrayWidget::createMenu()
{
    m_trayMenu = new QMenu(this);

    m_infoAction = m_trayMenu->addAction("Show Update Info");
    connect(m_infoAction, &QAction::triggered, this, &SystemTrayWidget::showUpdateInfo);

    m_trayMenu->addSeparator();

    m_manualSyncAction = m_trayMenu->addAction("Sync Database");
    connect(m_manualSyncAction, &QAction::triggered, this, &SystemTrayWidget::manualSync);

    m_manualCheckAction = m_trayMenu->addAction("Check Updates");
    connect(m_manualCheckAction, &QAction::triggered, this, &SystemTrayWidget::manualCheck);

    m_trayMenu->addSeparator();

    m_openTerminalAction = m_trayMenu->addAction("Open Terminal");
    connect(m_openTerminalAction, &QAction::triggered, this, &SystemTrayWidget::openTerminal);

    m_trayMenu->addSeparator();

    m_aboutAction = m_trayMenu->addAction("About");
    connect(m_aboutAction, &QAction::triggered, this, &SystemTrayWidget::showAbout);

    m_quitAction = m_trayMenu->addAction("Quit");
    connect(m_quitAction, &QAction::triggered, this, &SystemTrayWidget::quit);

    m_trayIcon->setContextMenu(m_trayMenu);
}

void SystemTrayWidget::setTrayIcon(const QString &status)
{
    QString tooltip = "Gentoo Update Checker";

    if (status == "updates") {
        tooltip = QString("Gentoo Update Checker\n%1 updates available").arg(m_updateCount);
    } else if (status == "syncing") {
        tooltip = "Gentoo Update Checker\nSyncing database...";
    } else if (status == "checking") {
        tooltip = "Gentoo Update Checker\nChecking for updates...";
    } else if (status == "error") {
        tooltip = "Gentoo Update Checker\nError occurred";
    } else {
        tooltip = "Gentoo Update Checker\nNo updates available";
    }

    // Use fallback colors if icons not available
    QPixmap pixmap(32, 32);
    if (status == "updates") {
        pixmap.fill(Qt::red);
    } else if (status == "syncing" || status == "checking") {
        pixmap.fill(Qt::yellow);
    } else if (status == "error") {
        pixmap.fill(Qt::darkRed);
    } else {
        pixmap.fill(Qt::green);
    }

    m_trayIcon->setIcon(QIcon(pixmap));
    m_trayIcon->setToolTip(tooltip);
}

void SystemTrayWidget::updateTrayStatus()
{
    if (m_syncInProgress) {
        setTrayIcon("syncing");
    } else if (m_hasUpdates) {
        setTrayIcon("updates");
    } else {
        setTrayIcon("idle");
    }
}

void SystemTrayWidget::onSyncStarted()
{
    m_syncInProgress = true;
    setTrayIcon("syncing");
    qDebug() << "Sync started";
}

void SystemTrayWidget::onSyncFinished(bool success)
{
    m_syncInProgress = false;
    if (success) {
        qDebug() << "Sync finished successfully";
    } else {
        qDebug() << "Sync failed";
        setTrayIcon("error");
    }
    updateTrayStatus();
}

void SystemTrayWidget::onUpdatesAvailable(int count)
{
    m_hasUpdates = true;
    m_updateCount = count;
    setTrayIcon("updates");
    
    qDebug() << "Updates available:" << count;
    
    // Show notification
    m_trayIcon->showMessage("Gentoo Update Checker",
                           QString("%1 package updates available").arg(count),
                           QSystemTrayIcon::Information,
                           10000);
}

void SystemTrayWidget::onNoUpdatesAvailable()
{
    m_hasUpdates = false;
    m_updateCount = 0;
    setTrayIcon("idle");
    qDebug() << "No updates available";
}

void SystemTrayWidget::onErrorOccurred(const QString &error)
{
    setTrayIcon("error");
    m_trayIcon->showMessage("Gentoo Update Checker - Error",
                           error,
                           QSystemTrayIcon::Critical,
                           15000);
    qDebug() << "Error:" << error;
}

void SystemTrayWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
        toggleVisibility();
    }
}

void SystemTrayWidget::showUpdateInfo()
{
    QString message = "<b>Gentoo Update Checker Status</b><br/>";
    message += QString("Updates available: %1<br/>").arg(m_hasUpdates ? m_updateCount : 0);
    message += QString("Sync in progress: %1<br/>").arg(m_syncInProgress ? "Yes" : "No");
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Update Information");
    msgBox.setText(message);
    msgBox.setIcon(m_hasUpdates ? QMessageBox::Warning : QMessageBox::Information);
    msgBox.exec();
}

void SystemTrayWidget::manualSync()
{
    if (m_syncInProgress) {
        QMessageBox::information(this, "Gentoo Update Checker", "Sync is already in progress.");
        return;
    }
    m_updateChecker->performSync();
}

void SystemTrayWidget::manualCheck()
{
    m_updateChecker->checkForUpdates();
}

void SystemTrayWidget::openTerminal()
{
    QProcess::startDetached("konsole");
}

void SystemTrayWidget::showAbout()
{
    QMessageBox::about(this, "About Gentoo Update Checker",
        "<h2>Gentoo Update Checker</h2>"
        "<p>Version 1.0.0</p>"
        "<p>A KDE Plasma system tray widget for monitoring Gentoo Linux package updates.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Automatically syncs the portage database every 24 hours</li>"
        "<li>Checks for available updates every 4 hours</li>"
        "<li>Displays update notifications in the system tray</li>"
        "<li>Provides quick access to terminal for package management</li>"
        "</ul>"
        "<p><b>Note:</b> Syncing the database and installing updates requires root privileges.</p>"
    );
}

void SystemTrayWidget::quit()
{
    qApp->quit();
}

void SystemTrayWidget::toggleVisibility()
{
    if (isVisible()) {
        hide();
    } else {
        showNormal();
        activateWindow();
    }
}

void SystemTrayWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (windowState() & Qt::WindowMinimized) {
            hide();
            event->ignore();
        }
    }
    QMainWindow::changeEvent(event);
}

void SystemTrayWidget::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

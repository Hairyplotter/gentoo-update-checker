#ifndef SYSTEMTRAYWIDGET_H
#define SYSTEMTRAYWIDGET_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <memory>

class UpdateChecker;

class SystemTrayWidget : public QMainWindow {
    Q_OBJECT

public:
    explicit SystemTrayWidget(QWidget *parent = nullptr);
    ~SystemTrayWidget();

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onSyncStarted();
    void onSyncFinished(bool success);
    void onUpdatesAvailable(int count);
    void onNoUpdatesAvailable();
    void onErrorOccurred(const QString &error);

    void showUpdateInfo();
    void manualSync();
    void manualCheck();
    void openTerminal();
    void showAbout();
    void quit();

private:
    void createTrayIcon();
    void createMenu();
    void setTrayIcon(const QString &status);
    void updateTrayStatus();
    void toggleVisibility();

    std::unique_ptr<UpdateChecker> m_updateChecker;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;

    // Menu actions
    QAction *m_infoAction;
    QAction *m_manualSyncAction;
    QAction *m_manualCheckAction;
    QAction *m_openTerminalAction;
    QAction *m_aboutAction;
    QAction *m_quitAction;

    // State
    bool m_hasUpdates;
    int m_updateCount;
    bool m_syncInProgress;
};

#endif // SYSTEMTRAYWIDGET_H

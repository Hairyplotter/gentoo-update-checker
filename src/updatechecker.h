#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QProcess>

class UpdateChecker : public QObject {
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);
    ~UpdateChecker();

    void start();
    void stop();

    bool hasUpdates() const { return m_hasUpdates; }
    bool isSyncInProgress() const { return m_syncInProgress; }
    int updateCount() const { return m_updateCount; }

signals:
    void syncStarted();
    void syncFinished(bool success);
    void updatesAvailable(int count);
    void noUpdatesAvailable();
    void errorOccurred(const QString &error);

public slots:
    void checkForUpdates();
    void performSync();

private slots:
    void onSyncFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onCheckFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QString parseEmergeOutput(const QString &output);
    void scheduleNextCheck();

    QTimer *m_checkTimer;
    QTimer *m_syncTimer;
    QProcess *m_syncProcess;
    QProcess *m_checkProcess;
    bool m_hasUpdates;
    bool m_syncInProgress;
    int m_updateCount;
    
    // Configuration
    static const int SYNC_INTERVAL_HOURS = 24;
    static const int CHECK_INTERVAL_MINUTES = 4 * 60; // 4 hours
};

#endif // UPDATECHECKER_H

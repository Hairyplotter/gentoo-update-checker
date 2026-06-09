#include "updatechecker.h"
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths>

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent),
      m_syncProcess(nullptr),
      m_checkProcess(nullptr),
      m_hasUpdates(false),
      m_syncInProgress(false),
      m_updateCount(0)
{
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &UpdateChecker::checkForUpdates);

    m_syncTimer = new QTimer(this);
    connect(m_syncTimer, &QTimer::timeout, this, &UpdateChecker::performSync);
}

UpdateChecker::~UpdateChecker()
{
    if (m_syncProcess) {
        m_syncProcess->kill();
        m_syncProcess->waitForFinished();
        delete m_syncProcess;
    }
    if (m_checkProcess) {
        m_checkProcess->kill();
        m_checkProcess->waitForFinished();
        delete m_checkProcess;
    }
}

void UpdateChecker::start()
{
    qDebug() << "Starting update checker...";
    performSync(); // Initial sync
    m_syncTimer->start(SYNC_INTERVAL_HOURS * 60 * 60 * 1000); // 24 hours
    m_checkTimer->start(CHECK_INTERVAL_MINUTES * 60 * 1000); // 4 hours
}

void UpdateChecker::stop()
{
    qDebug() << "Stopping update checker...";
    m_checkTimer->stop();
    m_syncTimer->stop();
}

void UpdateChecker::performSync()
{
    if (m_syncInProgress) {
        qDebug() << "Sync already in progress, skipping...";
        return;
    }

    m_syncInProgress = true;
    emit syncStarted();

    if (!m_syncProcess) {
        m_syncProcess = new QProcess(this);
        connect(m_syncProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &UpdateChecker::onSyncFinished);
        connect(m_syncProcess, &QProcess::errorOccurred,
                this, &UpdateChecker::onProcessError);
    }

    qDebug() << "Starting emerge --sync...";
    m_syncProcess->start("pkexec", QStringList() << "emerge" << "--sync");
}

void UpdateChecker::checkForUpdates()
{
    if (m_checkProcess) {
        if (m_checkProcess->state() == QProcess::Running) {
            qDebug() << "Check already in progress, skipping...";
            return;
        }
    } else {
        m_checkProcess = new QProcess(this);
        connect(m_checkProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &UpdateChecker::onCheckFinished);
        connect(m_checkProcess, &QProcess::errorOccurred,
                this, &UpdateChecker::onProcessError);
    }

    qDebug() << "Checking for available updates...";
    // Using emerge -quDN to get upgrade list without actually installing
    m_checkProcess->start("emerge", QStringList() << "-quDN" << "@world");
}

void UpdateChecker::onSyncFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_syncInProgress = false;

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        qDebug() << "Emerge sync completed successfully";
        emit syncFinished(true);
        // Check for updates immediately after sync
        checkForUpdates();
    } else {
        QString errorOutput = m_syncProcess->readAllStandardError();
        qDebug() << "Emerge sync failed with code" << exitCode << ":" << errorOutput;
        emit syncFinished(false);
        emit errorOccurred(QString("Database sync failed: %1").arg(errorOutput));
    }
}

void UpdateChecker::onCheckFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString output = m_checkProcess->readAllStandardOutput();
    QString errorOutput = m_checkProcess->readAllStandardError();

    if (exitStatus != QProcess::NormalExit) {
        qDebug() << "Check process crashed or was terminated";
        emit errorOccurred("Update check process failed");
        return;
    }

    // emerge -quDN returns exit code 1 if there are updates, 0 if none
    if (exitCode == 1 || !output.isEmpty()) {
        m_updateCount = output.count(QLatin1Char('\n'));
        m_hasUpdates = m_updateCount > 0;
        
        qDebug() << "Updates available:" << m_updateCount;
        qDebug() << "Package list:\n" << output;
        
        emit updatesAvailable(m_updateCount);
    } else {
        m_hasUpdates = false;
        m_updateCount = 0;
        qDebug() << "No updates available";
        emit noUpdatesAvailable();
    }
}

void UpdateChecker::onProcessError(QProcess::ProcessError error)
{
    QString errorMessage;
    switch (error) {
    case QProcess::FailedToStart:
        errorMessage = "Failed to start process. Make sure emerge and pkexec are installed.";
        break;
    case QProcess::Crashed:
        errorMessage = "Process crashed during execution.";
        break;
    case QProcess::Timedout:
        errorMessage = "Process timed out.";
        break;
    case QProcess::WriteError:
        errorMessage = "Error writing to process.";
        break;
    case QProcess::ReadError:
        errorMessage = "Error reading from process.";
        break;
    default:
        errorMessage = "Unknown process error occurred.";
    }
    
    qDebug() << "Process error:" << errorMessage;
    emit errorOccurred(errorMessage);
}

string UpdateChecker::parseEmergeOutput(const QString &output)
{
    // Parse emerge output to extract package information
    QStringList lines = output.split(QLatin1Char('\n'));
    return QString::number(lines.length());
}

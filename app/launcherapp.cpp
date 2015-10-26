#include "launcherapp.h"
#include "views/launcherframe.h"
#include "Logger.h"
#include "dbusinterface/launcheradaptor.h"
#include "app/global.h"
#include "app/xcb_misc.h"
#include "controller/dbusworker.h"
#include "dialogs/confirmuninstalldialog.h"
#include <QDBusConnection>
#include <QThread>

LauncherApp::LauncherApp(QObject *parent) : QObject(parent)
{
    m_launcherFrame = new LauncherFrame;
    new LauncherAdaptor(m_launcherFrame);
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerObject(LauncherPathName, m_launcherFrame);

    m_dbusWorker = new DBusWorker;
    m_dbusThread = new QThread;
    m_dbusWorker->moveToThread(m_dbusThread);

    connect(signalManager, SIGNAL(appUninstalled(QString)), this, SLOT(handleUninstall(QString)));
}

void LauncherApp::show(){
    m_launcherFrame->Show();
    qDebug() << "LauncherApp show";
    m_dbusThread->start();
    emit signalManager->requestData();
}

void LauncherApp::handleUninstall(QString appKey){
    qDebug() << "handleUninstall" << appKey;
    ConfirmUninstallDialog d(m_launcherFrame);
    QString message = tr("Are you sure to uninstall %1").arg(appKey);
    d.setMessage(message);
    connect(&d, SIGNAL(buttonClicked(int)), signalManager, SIGNAL(uninstallActionChanged(int)));
    d.exec();
}


LauncherApp::~LauncherApp()
{

}

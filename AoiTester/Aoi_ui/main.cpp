#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
//Qt
#include <QDir>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QSplashScreen>
#include <QTime>
#include <QTimer>
#include <QTranslator>

//qCC_db
#include <ccColorScalesManager.h>
#include <ccLog.h>
#include <ccNormalVectors.h>

//qCC_io
#include <FileIOFilter.h>
#include <ccGlobalShiftManager.h>

//local
#include "ccApplication.h"
#include "ccPersistentSettings.h"

#include "ccPluginInterface.h"
#include "ccPluginManager.h"




int main(int argc, char *argv[])
{
    bool commandLine = (argc > 1) && (argv[1][0] == '-');
    if ( !commandLine )
    {
        ccApplication::initOpenGL();
    }


    ccApplication app(argc, argv, commandLine);

    //store the log message until a valid logging instance is registered
    ccLog::EnableMessageBackup(true);

    //restore some global parameters
    {
        QSettings settings;
        settings.beginGroup(ccPS::GlobalShift());
        double maxAbsCoord = settings.value(ccPS::MaxAbsCoord(), ccGlobalShiftManager::MaxCoordinateAbsValue()).toDouble();
        double maxAbsDiag = settings.value(ccPS::MaxAbsDiag(), ccGlobalShiftManager::MaxBoundgBoxDiagonal()).toDouble();
        settings.endGroup();

        ccLog::Print(QString("[Global Shift] Max abs. coord = %1 / max abs. diag = %2").arg(maxAbsCoord, 0, 'e', 0).arg(maxAbsDiag, 0, 'e', 0));

        ccGlobalShiftManager::SetMaxCoordinateAbsValue(maxAbsCoord);
        ccGlobalShiftManager::SetMaxBoundgBoxDiagonal(maxAbsDiag);
    }

    FileIOFilter::InitInternalFilters(); //load all known I/O filters (plugins will come later!)
    ccNormalVectors::GetUniqueInstance(); //force pre-computed normals array initialization
    ccColorScalesManager::GetUniqueInstance(); //force pre-computed color tables initialization

    ccPluginManager::get().loadPlugins();

    int result = 0;

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AoiTester_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.setWindowIcon(QIcon(QApplication::applicationDirPath() + "/logo.png"));
    w.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint|Qt::MSWindowsFixedSizeDialogHint);
    w.showMaximized();

    QApplication::processEvents();


//    QDir  workingDir = QCoreApplication::applicationDirPath();
//    QDir::setCurrent(workingDir.absolutePath());

    result = app.exec();

    FileIOFilter::UnregisterAll();

    return result;
}

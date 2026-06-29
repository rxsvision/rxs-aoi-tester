#include "flowchart.h"

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
#include <rxsFixdal.h>




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int result = 0;
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "twodMeasurement_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    rxsSystemReady();

    FlowChart w;
    w.setWindowIcon(QIcon(QApplication::applicationDirPath() + "/logo.png"));
    w.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint|Qt::MSWindowsFixedSizeDialogHint);
    w.showMaximized();
    QApplication::processEvents();
    result = app.exec();

    rxsSystemRelease();
    return result;
}

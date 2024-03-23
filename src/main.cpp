// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "app_environment.h"
#include "import_qml_components_plugins.h"
#include "import_qml_plugins.h"
#include "devicefinder.h"



const char*const applicationName="zilogic_ble";

#ifdef ANDROIDQUIRKS
#include <android/log.h>

void myMessageHandler(
    QtMsgType type,
    const QMessageLogContext& context,
    const QString& msg
    ) {
    QString report=msg;
    if (context.file && !QString(context.file).isEmpty()) {
        report+=" in file ";
        report+=QString(context.file);
        report+=" line ";
        report+=QString::number(context.line);
    }
    if (context.function && !QString(context.function).isEmpty()) {
        report+=+" function ";
        report+=QString(context.function);
    }
    const char*const local=report.toLocal8Bit().constData();
    switch (type) {
    case QtDebugMsg:
        __android_log_write(ANDROID_LOG_DEBUG,applicationName,local);
        break;
    case QtInfoMsg:
        __android_log_write(ANDROID_LOG_INFO,applicationName,local);
        break;
    case QtWarningMsg:
        __android_log_write(ANDROID_LOG_WARN,applicationName,local);
        break;
    case QtCriticalMsg:
        __android_log_write(ANDROID_LOG_ERROR,applicationName,local);
        break;
    case QtFatalMsg:
    default:
        __android_log_write(ANDROID_LOG_FATAL,applicationName,local);
        abort();
    }
}
#endif


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
#ifdef ANDROIDQUIRKS
    qInstallMessageHandler(myMessageHandler);
#endif
    app.setApplicationName(applicationName);

    set_qt_environment();


    QQmlApplicationEngine engine;
    DeviceFinder deviceFinder;
    engine.rootContext()->setContextProperty("device_finder", &deviceFinder);
    const QUrl url(u"qrc:/qt/qml/Main/main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

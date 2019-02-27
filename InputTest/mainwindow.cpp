#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::ClickFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test(){
/*
    QString pluginsDir = "..../插件工程.dll";
    QPluginLoader pluginLoader(pluginsDir);

    QObject *plugin = pluginLoader.instance();
    if (plugin)
    {
        //插件名称
        //对插件初始化

            m_pInterFace = qobject_cast<PlatformInputContextPlugin *>(plugin);
            if (m_pInterFace)
            {
                m_pInterFace->sayHello("i get !!!! ");
            }


    }
    */
}

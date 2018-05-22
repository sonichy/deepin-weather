#include "mainwindow.h"
#include <DBlurEffectWidget>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QAction>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : DBlurEffectWidget(parent)
{
    isMLBD = false;
    city = latitude = longitude = "";
    resize(220,330);
    setWindowIcon(QIcon(":icon/clear.svg"));
    setWindowFlags(Qt::FramelessWindowHint);
    setMaskColor(DarkColor);
    //setBlendMode(InWindowBlend);

    QString x = readSettings(QDir::currentPath() + "/config.ini", "config", "X");
    QString y = readSettings(QDir::currentPath() + "/config.ini", "config", "Y");
    if(x=="" || y=="" || x.toInt()>QApplication::desktop()->width() || y.toInt()>QApplication::desktop()->height()){
        move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
    }else{
        move(x.toInt(),y.toInt());
    }

    layout = new QGridLayout;
    for(int i=0; i<6; i++){
        labelWImg[i] = new QLabel("");
        labelWImg[i]->setPixmap(QPixmap(":icon/clear.svg").scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        labelWImg[i]->setAlignment(Qt::AlignCenter);
        layout->addWidget(labelWImg[i],i,0);
        labelTemp[i] = new QLabel("15~20°C");
        labelTemp[i]->setAlignment(Qt::AlignCenter);
        layout->addWidget(labelTemp[i],i,1);
        if(i==0){
            labelTemp[i]->setStyleSheet("color:white;font-size:20px;");
            labelDate[i] = new QLabel("城市");
            labelDate[i]->setStyleSheet("color:white;font-size:20px;");
        }else{
            labelTemp[i]->setStyleSheet("color:white;font-size:12px;");
            labelDate[i] = new QLabel("1月1日");
            labelDate[i]->setStyleSheet("color:white;font-size:12px;");
        }
        labelDate[i]->setAlignment(Qt::AlignCenter);
        layout->addWidget(labelDate[i],i,2);
    }
    setLayout(layout);

    QTimer *timer = new QTimer();
    timer->setInterval(1800000);
    //timer->setInterval(60000);
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(getWeather()));
    getWeather();

    QAction *action_quit = new QAction("退出", this);
    connect(action_quit,SIGNAL(triggered(bool)),qApp,SLOT(quit()));
    addAction(action_quit);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

MainWindow::~MainWindow()
{

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isMLBD = true;
        m_point = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isMLBD) {
        setCursor(Qt::ClosedHandCursor);
        move(event->pos() - m_point + pos());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    isMLBD = false;
    setCursor(Qt::ArrowCursor);
    writeSettings(QDir::currentPath() + "/config.ini", "config", "X", QString::number(x()));
    writeSettings(QDir::currentPath() + "/config.ini", "config", "Y", QString::number(y()));
}

void MainWindow::getWeather()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    qDebug() << "getWeather()" << currentDateTime;

    // IP转城市名
    QString surl = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json";
    QUrl url(surl);
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply;
    reply = manager.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray BA = reply->readAll();
    qDebug() << surl ;
    qDebug() << BA;
    QJsonParseError JPE;
    QJsonDocument JD = QJsonDocument::fromJson(BA, &JPE);
    if(JPE.error == QJsonParseError::NoError) {
        if(JD.isObject()) {
            QJsonObject JO = JD.object();
            if(JO.contains("city")) {
                QJsonValue JV = JO.take("city");
                if(JV.isString()) {
                    city = JV.toString();
                }
            }
        }
    }

    // 根据城市名取经纬度
    surl = "http://w.api.deepin.com/v1/location/" + city;
    url.setUrl(surl);
    reply = manager.get(QNetworkRequest(url));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    BA = reply->readAll();
    qDebug() << surl ;
    qDebug() << BA;
    JD = QJsonDocument::fromJson(BA, &JPE);
    if(JPE.error == QJsonParseError::NoError) {
        if(JD.isArray()) {
            QJsonArray JA = JD.array();
            latitude = JA[0].toObject().value("latitude").toString();
            longitude = JA[0].toObject().value("longitude").toString();
        }
    }

    // 根据经纬度取天气预报
    surl = "http://w.api.deepin.com/v1/forecast/" + latitude + "/" + longitude;
    url.setUrl(surl);
    reply = manager.get(QNetworkRequest(url));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    BA = reply->readAll();
    qDebug() << surl;
    qDebug() << BA;
    JD = QJsonDocument::fromJson(BA, &JPE);
    if(JPE.error == QJsonParseError::NoError) {
        if(JD.isArray()) {
            QJsonArray JA = JD.array();
            for(int i=0; i<JA.size(); i++){
                QString fileName = ":icon/" + JA[i].toObject().value("name").toString().toLower() + ".svg";
                if(i==0){
                    labelWImg[i]->setPixmap(QPixmap(fileName).scaled(80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                    labelTemp[i]->setText(QString::number(JA[i].toObject().value("temperatureMin").toInt()) + "°C");
                    labelDate[i]->setText(city);
                }else{
                    labelWImg[i]->setPixmap(QPixmap(fileName).scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                    labelTemp[i]->setText(QString::number(JA[i].toObject().value("temperatureMin").toInt()) + "~" + QString::number(JA[i].toObject().value("temperatureMax").toInt()) + "°C");
                    QDateTime date = QDateTime::fromTime_t(JA[i].toObject().value("date").toInt());
                    labelDate[i]->setText(date.toString("M月d日"));
                }
            }
        }
    }
}

QString MainWindow::readSettings(QString path, QString group, QString key)
{
    QSettings settings(path, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup(group);
    QString value = settings.value(key).toString();
    return value;
}

void MainWindow::writeSettings(QString path, QString group, QString key, QString value)
{
    QSettings *config = new QSettings(path, QSettings::IniFormat);
    config->beginGroup(group);
    config->setValue(key, value);
    config->endGroup();
}

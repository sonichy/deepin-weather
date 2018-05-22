#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <DBlurEffectWidget>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>

DWIDGET_USE_NAMESPACE

class MainWindow : public DBlurEffectWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QString city, latitude, longitude;
    QGridLayout *layout;
    QLabel *labelCity, *labelRT, *labelWImg[6], *labelTemp[6], *labelDate[6];
    QPoint m_point;
    bool isMLBD;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QString readSettings(QString path, QString group, QString key);
    void writeSettings(QString path, QString group, QString key, QString value);

private slots:
    void getWeather();

};

#endif // MAINWINDOW_H

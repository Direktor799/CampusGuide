#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <QDir>
#include "./ui_mainwindow.h"
#include "map.h"
#include "player.h"
#include "labels.h"
#include "deswidget.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QFile *logfile;
    QTextStream *log;
    Player *me;
    Map *main_campus, *shahe_campus;
    QPushButton *map_switch_btn, *move_cancel_btn;
    QDateTime *vtime;
    QLabel *time_display;
    RouteLabel *routes_with_strat_display[3];
    DesWidget *deswidget;
    QSlider *slider;
    QLabel *listlabel, *sliderleftlabel, *sliderrightlabel;
    QListWidget *listwidget;
    AnimationLabel *animation;
    bool keep_calcu;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void route_calcu();
    void map_switch();
    void move_switch(multi_routes *routes, strat strategy);
    void move_cancel();
    void timer_update();
    void setFactor();
    void updateListWidget();
    void map_auto_change(Map *from);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

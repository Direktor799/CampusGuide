#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QComboBox>
#include <QSlider>
#include <QListWidget>
#include "./ui_mainwindow.h"
#include "map.h"
#include "player.h"
#include "labels.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void route_calcu();
    void map_switch();
    void move_switch(multi_routes* routes);
    void move_cancel();
    void timer_update();
    void setFactor();
    void updateListWidget();
private:
    Ui::MainWindow *ui;
};

class DesComboBox : public QWidget
{
    Q_OBJECT

public:
    QComboBox* combobox;
    QLabel* label;
    Map* map;
    DesComboBox(Map* m, QWidget *parent = nullptr);
private:

};

class DesWidget : public QWidget
{
    Q_OBJECT

public:
    vector<DesComboBox*> deslist;
    QPushButton* add_btn;
    QPushButton* delete_btn;
    DesWidget(QWidget *parent = nullptr);
    void addComboBox();
    void deleteComboBox();
    void setComboBox(QString text);
    void clear();
private:

};

#endif // MAINWINDOW_H

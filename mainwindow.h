#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QDebug>
#include "./ui_mainwindow.h"
#include "map.h"
#include "player.h"
#include "functions.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void navi_switch();
    void change_map();
    void timer_update();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

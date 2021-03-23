#include "mainwindow.h"
QComboBox* des;
Player* me;
Map* main_campus, *shahe_campus;
QPushButton* navi, *map_switch;
QDateTime* vtime;
QLabel* time_display;

void MainWindow::navi_switch()
{
    if(navi->text() == "开始导航")
    {
        navi->setText("停止导航");
        string s = des->currentText().toStdString();
        for(auto i = me->now_on->vertices.begin(); i < me->now_on->vertices.end(); i++)
            if(i->name == s)
            {
                me->navigation(i - me->now_on->vertices.begin(), 1);
                break;
            }
        navi->setText("开始导航");
    }
    else
    {
        me->now_on->route.second.resize(0);
        me->now_on->passed = me->now_on->route.second.begin();
        me->now_on->update();
        navi->setText("开始导航");
    }
}

void MainWindow::change_map()
{
    if(!main_campus->isHidden())
    {
        main_campus->hide();
        shahe_campus->show();
        des->clear();
        des->addItems(shahe_campus->list);
        map_switch->setText("切换至本部地图");
    }
    else
    {
        shahe_campus->hide();
        main_campus->show();
        des->clear();
        des->addItems(main_campus->list);
        map_switch->setText("切换至沙河地图");
    }
    if(me->now_on->isHidden())
        me->hide();
    else
        me->show();
}

void MainWindow::timer_update()
{
    *vtime = vtime->addSecs(1);
    time_display->setText(vtime->toString("yyyy-MM-dd hh:mm:ss"));
    time_display->update();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("CampusGuide");
    setFixedSize(1400, 672);

    main_campus = new Map("main_campus", this);
    shahe_campus = new Map("shahe_campus", this);
    shahe_campus->hide();

    me = new Player(this);
    me->now_on = main_campus;

    des = new QComboBox(this);
    des->move(1100, 0);
    des->setFixedSize(150, 30);
    des->addItems(main_campus->list);

    navi = new QPushButton("开始导航", this);
    navi->move(1100, 30);
    connect(navi, &QPushButton::clicked, this, &MainWindow::navi_switch);

    map_switch = new QPushButton("切换至沙河地图",this);
    map_switch->move(1200, 0);
    connect(map_switch, &QPushButton::clicked, this, &MainWindow::change_map);

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timer_update);
    timer->start(166);

    vtime = new QDateTime(QDate(2021, 4, 10),QTime(7, 30, 0));

    time_display = new QLabel(this);
    time_display->setFixedWidth(400);
    time_display->move(1100, 650);
    QFont font;
    font.setBold(true);
    time_display->setFont(font);
    time_display->setText("当前时间：" + vtime->toString("yyyy-MM-dd hh:mm:ss"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


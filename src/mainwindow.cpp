#include "mainwindow.h"

void MainWindow::move_cancel()
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 停止导航" << Qt::endl;
    me->now_routes.canceled = true;
}

void MainWindow::move_switch(multi_routes *routes, strat strategy)
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 开始导航" << Qt::endl;
    me->now_routes = *routes;
    me->now_routes.visable = true;
    me->now_using = strategy;
    for (int iter = 0; iter < 3; iter++)
        routes_with_strat_display[iter]->setDisabled(true);
    deswidget->setDisabled(true);
    listwidget->setDisabled(true);
    move_cancel_btn->show();
    keep_calcu = false;
    me->move();
    deswidget->clear();
    deswidget->addComboBox();
    route_calcu();
    for (int iter = 0; iter < 3; iter++)
        routes_with_strat_display[iter]->setEnabled(true);
    deswidget->setEnabled(true);
    listwidget->setEnabled(true);
    move_cancel_btn->hide();
    keep_calcu = true;
}

void MainWindow::route_calcu()
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 正在计算路线" << Qt::endl;
    bool is_valid = false;
    QVector<QPair<Map *, int>> des;
    for (auto i = deswidget->deslist.begin(); i < deswidget->deslist.end(); i++)
        for (auto j = (*i)->map->vertices.begin(); j < (*i)->map->vertices.end(); j++)
            if (j->name == (*i)->combobox->currentText())
            {
                is_valid = true;
                des.push_back(qMakePair((*i)->map, j - (*i)->map->vertices.begin()));
                break;
            }
    me->navigation(des);
    if (is_valid)
        for (int iter = 0; iter < 3; iter++)
            routes_with_strat_display[iter]->display();
    else
        for (int iter = 0; iter < 3; iter++)
            routes_with_strat_display[iter]->hide();
}

void MainWindow::map_switch()
{
    if (main_campus->isVisible())
    {
        main_campus->hide();
        for (auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
            (*i)->hide();
        shahe_campus->show();
        for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
            (*i)->show();
        deswidget->now_show = shahe_campus;
    }
    else
    {
        shahe_campus->hide();
        for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
            (*i)->hide();
        main_campus->show();
        for (auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
            (*i)->show();
        deswidget->now_show = main_campus;
    }
    if (!deswidget->deslist.empty() && deswidget->deslist.back()->combobox->currentText() == "")
    {
        deswidget->deleteComboBox();
        deswidget->addComboBox();
    }
    if (me->now_on->isVisible())
    {
        listlabel->show();
        listwidget->show();
    }
    else
    {
        listlabel->hide();
        listwidget->hide();
    }
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 切换地图至" << deswidget->now_show->name << Qt::endl;
}

void MainWindow::map_auto_change(Map *from)
{
    if (from == deswidget->now_show)
        map_switch();
}

void MainWindow::timer_update()
{
    *vtime = vtime->addMSecs(100 * me->speedfactor);
    time_display->setText(vtime->toString("yyyy-MM-dd hh:mm:ss ddd"));
    time_display->adjustSize();
    time_display->update();
    if (keep_calcu)
        route_calcu();
}

void MainWindow::setFactor()
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 速度倍数从" << me->speedfactor << "x";
    if (slider->value() == 0)
        me->speedfactor = 1;
    else if (slider->value() == 1)
        me->speedfactor = 6;
    else if (slider->value() == 2)
        me->speedfactor = 60;
    me->speedfactor = me->speedfactor;
    sliderrightlabel->setText(QString::number(me->speedfactor) + "x");
    *log << "调整为" << me->speedfactor << "x" << Qt::endl;
}

void MainWindow::updateListWidget()
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 正在更新周边建筑" << Qt::endl;
    listwidget->clear();
    QVector<route_info> surrounding = me->checkSurrounding();
    for (auto i = surrounding.begin(); i < surrounding.end() && i < surrounding.begin() + 10; i++)
        listwidget->addItem(me->now_on->vertices[i->edges.back()->to].name + "(" + QString::number(i->distance) + "m)");
    listwidget->resize(150, listwidget->count() * 18 + 4);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QDir dir = QDir::currentPath();
    dir.cdUp();
    dir.mkdir("logs");
    logfile = new QFile("../logs/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".log");
    logfile->open(QIODevice::WriteOnly | QIODevice::Text);
    log = new QTextStream(logfile);
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 程序启动" << Qt::endl;
    keep_calcu = true;
    ui->setupUi(this);

    setWindowTitle("CampusGuide");
    setFixedSize(1280, 672);

    deswidget = new DesWidget(log, this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timer_update);
    timer->start(100);

    vtime = new QDateTime;
    *vtime = QDateTime::currentDateTime();

    time_display = new QLabel(this);
    time_display->move(1100, 650);
    QFont font;
    font.setBold(true);
    time_display->setFont(font);

    main_campus = new Map("main_campus", "本部", vtime, this);
    for (auto i = main_campus->vertices.begin(); i < main_campus->vertices.end(); i++)
    {
        if (i->name != "Crossing")
        {
            BuildingLabel *bl = new BuildingLabel(i->name, i->pos_x, i->pos_y, this);
            connect(bl, &BuildingLabel::clicked, deswidget, &DesWidget::setComboBox);
            main_campus->bllist.push_back(bl);
        }
    }

    shahe_campus = new Map("shahe_campus", "沙河", vtime, this);
    for (auto i = shahe_campus->vertices.begin(); i < shahe_campus->vertices.end(); i++)
    {
        if (i->name != "Crossing")
        {
            BuildingLabel *bl = new BuildingLabel(i->name, i->pos_x, i->pos_y, this);
            connect(bl, &BuildingLabel::clicked, deswidget, &DesWidget::setComboBox);
            shahe_campus->bllist.push_back(bl);
        }
    }
    shahe_campus->hide();
    for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
        (*i)->hide();

    deswidget->now_show = main_campus;

    me = new Player(log, this);
    me->now_on = main_campus;
    main_campus->stackUnder(me);
    shahe_campus->stackUnder(me);

    deswidget->addComboBox();

    map_switch_btn = new QPushButton(this);
    map_switch_btn->setText("⇄");
    map_switch_btn->setStyleSheet("font: 25px;");
    map_switch_btn->resize(30, 30);
    map_switch_btn->move(1250, 0);
    connect(map_switch_btn, &QPushButton::clicked, this, &MainWindow::map_switch);

    move_cancel_btn = new QPushButton("停止导航", this);
    move_cancel_btn->move(1200, 300);
    move_cancel_btn->resize(80, 30);
    move_cancel_btn->hide();
    connect(move_cancel_btn, &QPushButton::clicked, this, &MainWindow::move_cancel);

    routes_with_strat_display[0] = new RouteLabel(&me->routes_with_strat[0], "最短距离", distance_first, this);
    routes_with_strat_display[0]->move(1100, 250);
    routes_with_strat_display[1] = new RouteLabel(&me->routes_with_strat[1], "最短时间", time_first, this);
    routes_with_strat_display[1]->move(1100, 300);
    routes_with_strat_display[2] = new RouteLabel(&me->routes_with_strat[2], "骑车最短时间", bike_allowed, this);
    routes_with_strat_display[2]->move(1100, 350);
    for (int iter = 0; iter < 3; iter++)
    {
        connect(routes_with_strat_display[iter], &RouteLabel::hover_in, me, &Player::show_route);
        connect(routes_with_strat_display[iter], &RouteLabel::hover_out, me, &Player::hide_route);
        connect(routes_with_strat_display[iter], &RouteLabel::clicked, this, &MainWindow::move_switch);
    }

    listlabel = new QLabel(this);
    listlabel->setText("附近建筑：");
    listlabel->move(1100, 405);

    listwidget = new QListWidget(this);
    listwidget->move(1100, 430);
    updateListWidget();
    connect(me, &Player::moving, this, &MainWindow::updateListWidget);
    connect(listwidget, &QListWidget::itemClicked, deswidget, &DesWidget::resetComboBox);

    sliderleftlabel = new QLabel(this);
    sliderleftlabel->setText("加速倍数：");
    sliderleftlabel->move(1100, 620);

    sliderrightlabel = new QLabel(this);
    sliderrightlabel->setText("6x");
    sliderrightlabel->move(1245, 620);

    slider = new QSlider(Qt::Horizontal, this);
    slider->move(1160, 630);
    slider->resize(80, 15);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setRange(0, 2);
    slider->setPageStep(1);
    slider->setValue(1);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::setFactor);
    animation = new AnimationLabel(this);
    connect(me, &Player::playAnimation, animation, &AnimationLabel::play);
    connect(me, &Player::changeMapFrom, this, &MainWindow::map_auto_change);
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 初始化完毕" << Qt::endl;
}

MainWindow::~MainWindow()
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 程序关闭" << Qt::endl;
    delete ui;
}

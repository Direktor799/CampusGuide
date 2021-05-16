#include "deswidget.h"

DesComboBox::DesComboBox(Map *m, QWidget *parent) : QWidget(parent)
{
    map = m;
    combobox = new QComboBox(this);
    label = new QLabel(this);
    combobox->addItems(map->list);
    label->setText(m->name);
    combobox->move(label->pos() + QPoint(30, 0));
    label->move(label->pos() + QPoint(0, 3));
    combobox->setFixedSize(100, 25);
    resize(130, 25);
}

void DesWidget::addComboBox()
{
    if (deslist.size() >= 5)
        return;
    QPoint pos;
    if (!deslist.empty())
    {
        pos = deslist.back()->pos() + QPoint(0, 30);
        deslist.back()->setDisabled(true);
    }
    deslist.push_back(new DesComboBox(now_show, this));
    deslist.back()->move(pos);
    deslist.back()->show();
    connect(deslist.back()->combobox, &QComboBox::currentTextChanged, this, &DesWidget::nextAllowed);
    delete_btn->setEnabled(true);
    if (deslist.size() >= 5)
        add_btn->setDisabled(true);

    add_btn->setDisabled(true);
    add_btn->move(add_btn->pos() + QPoint(0, 30));
    delete_btn->move(delete_btn->pos() + QPoint(0, 30));
}

void DesWidget::deleteComboBox()
{
    disconnect(deslist.back()->combobox, &QComboBox::currentTextChanged, this, &DesWidget::nextAllowed);
    delete deslist.back();
    deslist.pop_back();
    if (!deslist.empty())
        deslist.back()->setEnabled(true);
    add_btn->setEnabled(true);
    if (deslist.empty())
        delete_btn->setDisabled(true);
    add_btn->move(add_btn->pos() - QPoint(0, 30));
    delete_btn->move(delete_btn->pos() - QPoint(0, 30));
}

void DesWidget::setComboBox(QString text)
{
    text = text.left(text.indexOf("("));
    if (!deslist.empty() && deslist.back()->map->isVisible())
        deslist.back()->combobox->setCurrentText(text);
}

void DesWidget::clear()
{
    while (!deslist.empty())
        deleteComboBox();
}

void DesWidget::resetComboBox(QListWidgetItem *item)
{
    if (item->text() == "")
        return;
    clear();
    addComboBox();
    setComboBox(item->text());
}

void DesWidget::nextAllowed()
{
    *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 目的地列表更改为: ";
    for (auto i = deslist.begin(); i < deslist.end(); i++)
        *log << (*i)->combobox->currentText() << " ";
    *log << Qt::endl;
    if (deslist.back()->combobox->currentText() != "")
    {
        if (deslist.size() < 5)
            add_btn->setEnabled(true);
    }
}

DesWidget::DesWidget(QTextStream *textstream, QWidget *parent) : QWidget(parent)
{
    log = textstream;
    move(1100, 0);
    add_btn = new QPushButton("+", this);
    add_btn->move(30, 0);
    add_btn->setFixedSize(50, 30);
    add_btn->setDisabled(true);
    connect(add_btn, &QPushButton::clicked, this, &DesWidget::addComboBox);

    delete_btn = new QPushButton("-", this);
    delete_btn->move(80, 0);
    delete_btn->setFixedSize(50, 30);
    connect(delete_btn, &QPushButton::clicked, this, &DesWidget::deleteComboBox);

    resize(130, 500);
}
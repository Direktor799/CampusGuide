#ifndef DESWIDGET_H
#define DESWIDGET_H

#include <QComboBox>
#include <QListWidget>
#include "functions.h"
#include "map.h"

class DesComboBox : public QWidget
{
    Q_OBJECT

public:
    QComboBox *combobox;
    QLabel *label;
    Map *map;
    DesComboBox(Map *m, QWidget *parent = nullptr);

private:
};

class DesWidget : public QWidget
{
    Q_OBJECT

public:
    QTextStream *log;
    Map *now_show;
    QVector<DesComboBox *> deslist;
    QPushButton *add_btn;
    QPushButton *delete_btn;
    DesWidget(QTextStream *textstream, QWidget *parent = nullptr);
    void addComboBox();
    void deleteComboBox();
    void setComboBox(QString text);
    void resetComboBox(QListWidgetItem *item);
    void clear();
    void nextAllowed();

private:
};

#endif // DESWIDGET_H
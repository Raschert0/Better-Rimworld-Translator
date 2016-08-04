#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QMap>
#include <QXmlStreamReader>

const QString working_file_path{"D:/Git/BetterRim/files/wf.xml"};

const int return_exist{1};
const int return_add{2};
const int return_continue{3};


class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget(){}
private:
    QPushButton start{"Тест"};
    QMap<QString,int> working_set;
    void tryLoadWorkingSet();
    void saveWorkingSet();
    int checkElementInSet(QString &name, QXmlStreamReader &xml, QVector<QString> &chain);
public slots:
    void Start();
};

#endif // WIDGET_H

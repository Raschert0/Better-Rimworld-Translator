#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QMap>

const QString working_file_path{"D:/Git/BetterRim/files/wf.xml"};

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
public slots:
    void Start();
};

#endif // WIDGET_H

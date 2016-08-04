#ifndef DEF_H
#define DEF_H

#include <QObject>
#include <QString>
#include <QVector>

class Def : public QObject
{
    Q_OBJECT
public:
    Def(QString &name, QObject *parent = 0);
    void addDefinition(QString buf);
private:
    QString def_name;
    QVector<QString> sufixes;
signals:

public slots:
};

#endif // DEF_H

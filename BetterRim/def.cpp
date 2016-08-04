#include "def.h"

Def::Def(QString &name, QObject *parent) : QObject(parent)
{
    def_name = name;
}

void Def::addDefinition(QString buf)
{
    sufixes.push_back(buf);
}

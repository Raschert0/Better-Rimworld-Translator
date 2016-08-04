#include "widget.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>
#include <QVector>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    resize(200,200);
    start.setParent(this);
    start.move(10,10);
    QObject::connect(&start,SIGNAL(clicked(bool)),this,SLOT(Start()));
}

void Widget::tryLoadWorkingSet()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load map from file", "", "Map (*.ttx)");
    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "Can't to open file.", file.errorString());
            return;
        }
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_7);
        in >> working_set;
    }
}

void Widget::saveWorkingSet()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save map to file", "", "Map (*.ttx);;All files (*)");
    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, "Can't to open file.", file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_7);
        out << working_set;
    }
}

int Widget::checkElementInSet(QString &name, QXmlStreamReader &xml, QVector<QString> &chain)
{
    QMessageBox::StandardButton reply;
    if(xml.isCharacters() && !xml.isWhitespace()){
        if(working_set.count(name)){  //in working_set
            if(working_set[name]){    //and allowed
                return return_exist;
            }else{                          //and declined
                if(chain.size() > 1 && working_set[chain[chain.size() - 2]] > 1){
                    return return_exist;
                }
            }
        }else{                              //not in working_set
            reply = QMessageBox::question(this,"Add to set","You want to add\nKey: " + name + "\nValue: " + xml.text().toString(),
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                working_set.insert(name,1);
            }else{
                working_set.insert(name,0);
            }
        return return_add;
        }
    }else{
        if(!working_set.count(name)){
            reply = QMessageBox::question(this,"Add to set","You want to add\nKey: " + name,
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                working_set.insert(name,2);
            }else{
                working_set.insert(name,0);
            }
        }
    }
    return return_continue;
}

void Widget::Start()
{
    start.setVisible(false);
    update();
    tryLoadWorkingSet();
    QFile working_file(working_file_path);
    if(!working_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Cannot open the working file!";
        start.setVisible(true);
        update();
        return;
    }

    QXmlStreamReader xml_wf(&working_file);
    QXmlStreamReader::TokenType token = xml_wf.readNext();
    QVector<QString> chain;
    QString buf_string{""};
    int status;
    while (!xml_wf.atEnd() && !xml_wf.hasError()){
        switch(token){
        case QXmlStreamReader::StartDocument:
            break;
        case QXmlStreamReader::StartElement:
            buf_string = xml_wf.name().toString();
            chain.push_back(buf_string);
            token = xml_wf.readNext();
            status = checkElementInSet(buf_string, xml_wf, chain);
            switch(status){
            case return_continue:
                continue;
            case return_add:
            case return_exist:
                qDebug() << xml_wf.text();
                break;
            default:
                break;
            }
            break;
        case QXmlStreamReader::EndElement:
            if(chain.size()){
                chain.removeLast();
            }else{
                qDebug() << "Something wrong with chain. Last name: " << buf_string;
            }
            break;
        default:
            break;
        }
        token = xml_wf.readNext();
    }
    working_file.close();
    saveWorkingSet();
    start.setVisible(true);
    update();
    return;
}

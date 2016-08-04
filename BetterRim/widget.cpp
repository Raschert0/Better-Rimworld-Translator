#include "widget.h"
#include <QFile>
#include <QXmlStreamReader>
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
    QMessageBox::StandardButton reply;
    QVector<QString> chain;
    QString buf_string{""};

    while (!xml_wf.atEnd() && !xml_wf.hasError()){
        switch(token){
            case QXmlStreamReader::StartDocument:
                break;
            case QXmlStreamReader::StartElement:
                buf_string = xml_wf.name().toString();
                chain.push_back(buf_string);
                token = xml_wf.readNext();
                if(token == QXmlStreamReader::Characters && !xml_wf.isWhitespace()){
                    if(working_set.count(buf_string)){  //in working_set
                        if(working_set[buf_string]){    //and allowed
                            qDebug() << xml_wf.text();
                        }else{                          //and declined
                            if(chain.size() > 1){
                                if(working_set[chain[chain.size() - 2]] > 1){
                                    qDebug() << xml_wf.text();
                                }
                            }
                        }
                    }else{                              //not in working_set
                        reply = QMessageBox::question(this,"Add to set","You want to add\nKey: " + buf_string + "\nValue: " + xml_wf.text().toString(),
                                                      QMessageBox::Yes|QMessageBox::No);
                        if(reply == QMessageBox::Yes){
                            working_set.insert(buf_string,1);
                            qDebug() << xml_wf.text();
                        }else{
                            working_set.insert(buf_string,0);
                        }
                    }
                    break;
                }else{
                    if(!working_set.count(buf_string)){
                        reply = QMessageBox::question(this,"Add to set","You want to add\nKey: " + buf_string,
                                                      QMessageBox::Yes|QMessageBox::No);
                        if(reply == QMessageBox::Yes){
                            working_set.insert(buf_string,2);
                            qDebug() << buf_string << " added";
                        }else{
                            working_set.insert(buf_string,0);
                        }
                    }
                    continue;
                }
            case QXmlStreamReader::EndElement:
                if(chain.size()){
                    chain.removeLast();
                }else{
                    qDebug() << "Something wrong with chain. Last name: " << buf_string;
                }
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

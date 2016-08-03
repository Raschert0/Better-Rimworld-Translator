#include "widget.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>

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

    while (!xml_wf.atEnd() && !xml_wf.hasError()){
        QString buf_string;
        switch(token){
            case QXmlStreamReader::StartDocument:
                break;
            case QXmlStreamReader::StartElement:
                buf_string = xml_wf.name().toString();
                token = xml_wf.readNext();
                if(token == QXmlStreamReader::Characters && !xml_wf.isWhitespace()){
                    if(working_set.count(buf_string)){  //in working_set
                        if(working_set[buf_string]){    //and allowed
                            qDebug() << xml_wf.text();
                        }else{                          //and declined
                            break;
                        }
                    }else{                              //not in working_set
                        reply = QMessageBox::question(this,"Add to set","You want to add key: " + buf_string + " " + xml_wf.text().toString(),
                                                      QMessageBox::Yes|QMessageBox::No);
                        if(reply == QMessageBox::Yes){
                            working_set.insert(buf_string,true);
                            qDebug() << xml_wf.text();
                        }else{
                            working_set.insert(buf_string,false);
                        }
                    }
                }else{
                    continue;
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

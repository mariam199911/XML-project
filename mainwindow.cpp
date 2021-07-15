#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/xml-tree.h"
#include "lz77.hpp"

#include<QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QFile mytempfile("out.txt");
QFile myfile("myfile.txt");
QString text = "";
QString s="";
void MainWindow::on_OpenFileButton_clicked()
{
    ui->input_text->clear();
    ui->output_text->clear();

    QFile input_file(QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("XML File (*.xml) ;;TextFile (*.txt)")));
    s=input_file.fileName();
    input_file.open(QIODevice::ReadOnly |QIODevice::Text);
    QTextStream stream(&input_file);
    text= stream.readAll();
    myfile.remove();
    mytempfile.resize(0);
    input_file.copy("myfile.txt");
    QFile myfile("myfile.txt");
    ui->input_text->setPlainText(text);
    ui->input_text->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->output_text->setPlainText(text);
    ui->output_text->setLineWrapMode(QPlainTextEdit::NoWrap);
    input_file.close();
}

void MainWindow::on_Save_Button_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
             tr("Save Address Book"), "",
             tr("XML (*.xml);;TEXT (*.txt)"));
    QFile output_file(fileName);
    output_file.open(QIODevice::ReadWrite|QIODevice::Text);
    QString text=ui->output_text->toPlainText();
    output_file.write(text.toUtf8());
    output_file.close();
}

void MainWindow::on_Exit_Button_clicked()
{
    qApp->quit();
}

void MainWindow::on_Reset_button_clicked()
{
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::on_Prettify_Button_clicked()
{

    XMLTree* treeNode = new XMLTree(ui->output_text->toPlainText());
    MainBlock* root = treeNode->getXMLFileRoot();
    QString output = "";
    int spacesNum = 0;
    QString out = treeNode->prettifyingXMLTreeFile(root, spacesNum, output);
    ui->output_text->setPlainText(out);
}

void MainWindow::on_Remove_Spaces_clicked()
{
    XMLTree* treeNode = new XMLTree(text);
    MainBlock* root = treeNode->getXMLFileRoot();
    QString output = "";
    QString out = treeNode->minfyingXMLTreeFile(root, output);
    ui->output_text->setPlainText(out);
}

void MainWindow::on_JSON_Button_clicked()
{
    XMLTree* treeNode = new XMLTree(text);
    QString out = treeNode->convertXMLFileIntoJSONFile();
    ui->output_text->setPlainText(out);
}

void MainWindow::on_Check_Button_clicked()
{
    QString output = "";
    XMLTree* treeNode = new XMLTree(text);
    QString out = treeNode->error_checking(output);
    ui->output_text->setPlainText(out);
}

void MainWindow::on_pushButton_4_clicked()
{
    QString output = "";
    XMLTree* treeNode = new XMLTree(text);
    QString out = treeNode->error_correction(output);
    ui->output_text->setPlainText(out);
}

void MainWindow::on_compression_Button_clicked()
{
    QFile file(s);
        if (!file.open(QFile::ReadOnly|QFile::Text))
        {
            QMessageBox::warning(this,"..","can not open the file");
            return ;
        }
        QTextStream in(&file);
        QString text = in.readAll();
        QByteArray compressed=compression(text);
        QString fileName = QFileDialog::getSaveFileName(this,
                 tr("Save Address Book"), "",
                 tr("COMP (*.comp);;All Files ()"));
        QFile newDoc(fileName);
        if(newDoc.open(QIODevice::WriteOnly)){
            newDoc.write(compressed);
        }

        newDoc.close();

}


void MainWindow::on_Decompression_Button_clicked()
{
        QFile file(QFileDialog::getOpenFileName(this, tr("Open File"), QString(),tr("Text Files (*.comp)")));
                char file_data;
                QByteArray arr;
                if(!file.open(QFile::ReadOnly))
                {
                    QMessageBox::warning(this,"..","can not open the file");
                    return;
                }

                while(!file.atEnd())
                {
                  file.read(&file_data,sizeof(char));
                  arr.push_back(file_data);
                }
                file.close();
            QString txt = decompression(arr);
            ui->output_text->clear();

            ui->output_text->setPlainText(txt);
}


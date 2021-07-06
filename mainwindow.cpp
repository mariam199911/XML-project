#include "mainwindow.h"
#include "ui_mainwindow.h"

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
void MainWindow::on_OpenFileButton_clicked()
{
    ui->input_text->clear();
    QFile input_file(QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("XML File (*.xml) ;;TextFile (*.txt)")));
    input_file.open(QIODevice::ReadOnly |QIODevice::Text);
    QTextStream stream(&input_file);
    QString text= stream.readAll();
    myfile.remove();
    mytempfile.resize(0);
    input_file.copy("myfile.txt");
    QFile myfile("myfile.txt");
    ui->input_text->setPlainText(text);
    ui->input_text->setLineWrapMode(QPlainTextEdit::NoWrap);
   input_file.close();
}
void MainWindow::on_Save_Button_clicked()
{
 QFile output_file(QFileDialog::getSaveFileName(this,tr("Save File"),"",tr("Text File ()*.txt;;XML File ()*.xml")));
 output_file.open(QIODevice::ReadWrite|QIODevice::Text);
 QString text=ui->output_text->toPlainText();
     output_file.write(text.toUtf8());
     output_file.close();
}


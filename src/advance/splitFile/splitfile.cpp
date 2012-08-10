#include "splitfile.h"
#include "ui_splitfile.h"
#include "config.h"
#include "pcommon.h"

#include <QFileDialog>
#include <QFile>

SplitFile::SplitFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplitFile)
{
    ui->setupUi(this);

    createConnect();

    setData();
    QString str = "/home/files/Documents/Bible/unrar/my/1Co.htm";
    ui->LEFilePath->setText(str);

    QString tag = "<A NAME";
    ui->LETagSplit->setText(tag);

    //    showFileHtml(str);
    ui->cBAutoOn->setChecked(true);
    AutoSplitOn();
    showFileText();
    //    AutoEstimate();
    AutoRun();
}
//--------------------------------------------
SplitFile::~SplitFile()
{
    delete ui;
}
//--------------------------------------------
void SplitFile::createConnect()
{
    /// manual
    connect(ui->pBBrowse, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui->pBShow, SIGNAL(clicked()), this, SLOT(showFileText()));

    /// auto
    connect(ui->cBAutoOn, SIGNAL(stateChanged(int)), this, SLOT(AutoSplitOn()));
    connect(ui->pBAutoRun, SIGNAL(clicked()), this, SLOT(AutoRun()));
    connect(ui->pBAutoBrowse, SIGNAL(clicked()), this, SLOT(AutoBrowse()));
    connect(ui->pBAutoEstimate, SIGNAL(clicked()), this, SLOT(AutoEstimate()));
}
//--------------------------------------------
void SplitFile::browse()
{
    QString beginpath = Config::configuration ()->AppDir ();
    //    QString beginpath = "/home/files/Documents/Bible/unrar/NT_Greek_WH-E_UTF8";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select file"),
                                                    beginpath,
                                                    tr("Html Files (*.html *.htm);;All Files (*)"));
    if (!fileName.isNull())
    {
        ui->LEFilePath->setText(fileName);
    }
}
//--------------------------------------------
void SplitFile::setData()
{

}
//--------------------------------------------
void SplitFile::showFileText()
{
    TextOfFile = getTextFromFile(ui->LEFilePath->text());
    showFileHtml(ui->LEFilePath->text());
    showFileEdit(ui->LEFilePath->text());
}
//--------------------------------------------
void SplitFile::showFileHtml(QString filepath)
{
    if(QFile::exists(filepath))
    {
        //        QString filetext = getTextFromFile(filepath);
        ui->textBrowser->setHtml(TextOfFile);
    }
}
//--------------------------------------------
void SplitFile::showFileEdit(QString filepath)
{
    if(QFile::exists(filepath))
    {
        //        QString filetext = getTextFromFile(filepath);
        ui->textEdit->setPlainText(TextOfFile);
    }
}
//-----------------------------------------------
void SplitFile::AutoSplitOn()
{
    bool flag = ui->cBAutoOn->isChecked();
    QString filepath = ui->LEFilePath->text();
    ui->LAInputFile->setText(filepath);

    QString outputpath = QString(filepath)
            .remove(QString(filepath).split("/").last())
            + "Output/";
    ui->LEDirOutput->setText(outputpath);

    /// visible
    ui->LAInputFile->setVisible(flag);
    ui->LAValueFiles->setVisible(flag);
    ui->LEDirOutput->setVisible(flag);
    ui->LETagSplit->setVisible(flag);

    ui->label_5->setVisible(flag);
    ui->label_6->setVisible(flag);
    ui->label_7->setVisible(flag);
    ui->label_8->setVisible(flag);

    ui->pBAutoBrowse->setVisible(flag);
    ui->pBAutoEstimate->setVisible(flag);
    ui->pBAutoRun->setVisible(flag);

}
//-----------------------------------------------
void SplitFile::AutoEstimate()
{
    TagOfFile = ui->LETagSplit->text();
    TextOfFile = ui->textEdit->toPlainText();
    countFiles = countTheNumberOfFiles(&TextOfFile, TagOfFile);
    ui->LAValueFiles->setText(QString::number(countFiles));
}
//-----------------------------------------------
void SplitFile::AutoBrowse()
{
    QString firstpath = Config::configuration()->AppDir();
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Browse path for ouput"),
                                                          firstpath,
                                                          options);
    if (!directory.isEmpty())
        ui->LEDirOutput->setText(directory);
}
//-----------------------------------------------
void SplitFile::AutoRun()
{
    AutoEstimate();


    QString outputPath = ui->LEDirOutput->text();
    QDir dir(outputPath);
    //    qDebug() << "path = " << path <<  "last = " << path.last();
    if (!dir.exists())
    {
        /// create folder

        QString last = QString(outputPath).remove(outputPath.length()-1,1);
        last =  QString(last).split("/").last();
        QString dir = QString(outputPath).remove("/" + last);
        QDir dir2(dir);
        dir2.mkdir(last);
    }
    else
    {
        QStringList lstFiles = dir.entryList(QDir::Files); //Получаем список файлов
        //Удаляем файлы
        foreach (QString entry, lstFiles)
        {
            QString entryAbsPath = dir.absolutePath() + "/" + entry;
            //qDebug() << entryAbsPath;
            QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
            QFile::remove(entryAbsPath);
        }
    }



    QString filename, text;
    QString textinput = TextOfFile;
//    qDebug() << textinput;
    int pos;

///first delete
    pos = textinput.indexOf(TagOfFile);
    text = QString(textinput).remove(pos+TagOfFile.length(), textinput.length()-pos);
    textinput.remove(text);

//    qDebug() << textinput;
    for (int i = 1; i < countFiles+1; i++)
    {
        /// create files
        filename = outputPath + "output_file_" + QString::number(i) + ".htm";

        pos = textinput.indexOf(TagOfFile)  ;
        if (pos == -1)
        {
            pos = 0;
            text = textinput;
        }
        else
        {
            pos += TagOfFile.length();
            text = QString(textinput).remove(pos, textinput.length()-pos);
        }

        qDebug() << pos;


        textinput.remove(text);

//        qDebug() << textinput;
        createEmptyHtml(filename, QString::number(i), text);
    }
}
//-----------------------------------------------

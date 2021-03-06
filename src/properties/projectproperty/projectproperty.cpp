/*!
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *    Author: Sapronov Alexander/WarmongeR
 *    E-Mail: sapronov.alexander92@.gmail.com
 *    Years: 2011, 2012
 *    WebSite: https://github.com/WarmongeR1/bQella
 */


#include "projectproperty.h"
#include "config.h"
#include "mainwindow.h"
#include "pcommon.h"
#include "filecommon.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QStringListModel>

//===================== class ProjectProperties ===========================

ProjectProperties::ProjectProperties(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    modeNewProject = false;

//    connect(ui.cbStrongNumber, SLOT(setChecked(bool)), ui.LEStrongDir, SLOT(hide()));
    connect(ui.cbStrongNumber, SIGNAL(stateChanged(int)), this, SLOT(hideshow()));
    connect(ui.cbSound, SIGNAL(stateChanged(int)), this, SLOT(hideshow()));
    setData(); /// set firsrt data (list encoding)
    setToolTipLabels(); ///set what'is labels
}
//-------------------------------------------------------------------
void ProjectProperties::setProperties(bool newPrj, ModuleProperties pr)
{
//    printToDebugModuleProperties(&pr);
    prjTitle= pr.prjTitle;
    prjFN = pr.prjFN;
    prjStartPage = pr.prjStartPage;
    modeNewProject = newPrj;

    QString version;
    version.setNum(pr.moduleBVersion);

    ui.LEBibleName -> setText(pr.moduleBiblename);
    ui.LEBibleShortName -> setText(pr.moduleBibleShortName);
    ui.LECopyright -> setText(pr.moduleCopyright);
    ui.doubleSpinBoxVersion -> setValue(pr.moduleBVersion);
    ui.cbOldTestament -> setChecked(pr.oldTestament);
    ui.cbNewTestament -> setChecked(pr.newTestament);
    ui.cbmoduleType -> setChecked(pr.moduleType);
    ui.cbApocrypha -> setChecked(pr.apocrypha);
    ui.cbChapterZero -> setChecked(pr.chapterZero);
    ui.cbEnglishPsalms -> setChecked(pr.englishPsalms);
    ui.cbStrongNumber -> setChecked(pr.strongNumber);
    ui.cbUseChapterHead -> setChecked(pr.useChapterHead);
    ui.cbUseRightAlignment -> setChecked(pr.useRightAlignment);
    ui.cbNoForcedLineBreaks -> setChecked(pr.noForcedLineBreaks);
    ui.LECategories -> setText(pr.categories);
    ui.comBEncoding -> setCurrentIndex( ui.comBEncoding->findText(pr.defaultEncoding));
    ui.LEDesiredFontName -> setText(pr.desiredFontName);
    ui.LEDesiredFontPath -> setText(pr.desiredFontPath);
    ui.LEStrongDir -> setText(pr.strongsDirectory);
    ui.LESoundDir -> setText(pr.soundDirectory);
    ui.LEInstallFonts -> setText(pr.installFonts);
    ui.LEHtmlFilter ->setText (pr.htmlFilter);
    ui.LEDesiredUIFont -> setText(pr.desiredUIFont);
    ui.comBLanguage->setCurrentIndex(ui.comBLanguage->findText(pr.language));
    hideshow();
}
//-------------------------------------------------------------------
void ProjectProperties::reject()
{
    validProperties = false;
    QWidget::hide();  //close dialog
}
//-------------------------------------------------------------------
void ProjectProperties::showUpdate()
{
    if(!modeNewProject)
    {
        ui.LEBibleName->setEnabled(false);
    }
    else
    {
        ui.LEBibleName->setEnabled(true);
    }
}
//-------------------------------------------------------------------
void ProjectProperties::accept()
{
    QString s = "";  //holds list of errors
    bool er = false;
    validProperties = false;
    if (ui.LEBibleName -> text().isEmpty()){
        s = tr("- Please enter a bible name.\n");
        er = true;
    }
    if (ui.LEBibleShortName -> text().isEmpty()) {
        s += tr("- Please enter a short bible name.\n");
        er = true;
    }

    if (er)
    {
        QMessageBox::critical(this, tr("Project property error"), s);
    }
    else
    {
        QDir dir(prjFN);

        QString pathOfBibleName = QString(ui.LEBibleName->text()).replace(" ", "_");

        /// create project dir
        dir.mkdir(pathOfBibleName);

        //check for valid project file name
        QString filePrjFilename = prjFN+pathOfBibleName+"/"+pathOfBibleName+GL_PROJECT_FILE;
//        qDebug() << "filePrjFilename = " << filePrjFilename
//                 << " prjFN = " << prjFN;
        QFile filePrj(filePrjFilename);
        if (!filePrj.exists())
        {		//create file if it's not exist
            if (filePrj.open(QIODevice::ReadWrite))
            {	//try to create file
                QTextStream ts(&filePrj);
                ts << "<pemproject>\n</pemproject>";
                filePrj.close();
            }
            else
            {
                QMessageBox::critical(this, tr("Project property error"), tr("Can not create file:\n%1").arg(filePrjFilename));
                er = true;
            }
        }

        if (!er)
        {
            /// create first files
            createEmptyHtml(prjFN + pathOfBibleName + "/" + "   ___Instruction", "   ___Instruction" );
            createEmptyHtml(prjFN + pathOfBibleName + "/ru.html", "ru");
            createEmptyHtml(prjFN + pathOfBibleName + "/en.html", "en");

            qDebug() << "prjFN + pathOfBibleName = " << prjFN + pathOfBibleName;

            /// add to config
            Config::configuration() -> setModuleBiblename(ui.LEBibleName -> text());
            Config::configuration() -> setModuleBibleShortName(ui.LEBibleShortName -> text());
            Config::configuration() -> setModuleCopyright(ui.LECopyright -> text());
            Config::configuration() -> setOldTestament(ui.cbOldTestament -> checkState());
            Config::configuration() -> setNewTestament(ui.cbNewTestament -> checkState());
            Config::configuration() -> setApocrypha(ui.cbApocrypha -> checkState());
            Config::configuration() -> setChapterZero(ui.cbChapterZero -> checkState());
            Config::configuration() -> setEnglishPsalms(ui.cbEnglishPsalms -> checkState());
            Config::configuration() -> setStrongNumber(ui.cbStrongNumber -> checkState());
            Config::configuration() -> setUseChapterHead(ui.cbUseChapterHead -> checkState());
            Config::configuration() -> setUseRightAlignment(ui.cbUseRightAlignment ->checkState());
            Config::configuration() -> setNoForcedLineBreaks(ui.cbNoForcedLineBreaks -> checkState());
            Config::configuration() -> setModuleType(ui.cbmoduleType ->checkState());
            Config::configuration() -> setCategories(ui.LECategories -> text());
            Config::configuration() -> setDefaultEncoding(ui.comBEncoding->currentText());
            Config::configuration() -> setDesiredFontName(ui.LEDesiredFontName -> text());
            Config::configuration() -> setDesiredFontPath(ui.LEDesiredFontPath -> text());
            Config::configuration() -> setStrongsDirectory(ui.LEStrongDir -> text());
            Config::configuration() -> setSoundDirectory(ui.LESoundDir -> text());
            Config::configuration() -> setHtmlFilter(ui.LEHtmlFilter -> text());
            Config::configuration() -> setInstallFonts(ui.LEInstallFonts -> text());
            Config::configuration() -> setDesiredUIFont(ui.LEDesiredUIFont -> text());
            Config::configuration() -> setLanguage(ui.comBLanguage->currentText());


            //project, start page and sources files do exist we can proceed with setting project properties
            ModuleProperties prop;
            prop.prjTitle= ui.LEBibleName -> text();
            prop.prjStartPage = urlifyFileName(prjFN+pathOfBibleName+"/"+"ru.html");
            prop.moduleBiblename = ui.LEBibleName -> text();
            prop.moduleBibleShortName = ui.LEBibleShortName -> text();
            prop.moduleCopyright = ui.LECopyright -> text();
            prop.moduleBVersion = ui.doubleSpinBoxVersion -> value();
            prop.prjFN = urlifyFileName(prjFN+pathOfBibleName+"/"+pathOfBibleName+GL_PROJECT_FILE);
            prop.moduleType = ui.cbmoduleType ->isChecked();
            prop.oldTestament = ui.cbOldTestament -> isChecked();
            prop.newTestament = ui.cbNewTestament -> isChecked();
            prop.apocrypha = ui.cbApocrypha -> isChecked();
            prop.chapterZero = ui.cbChapterZero -> isChecked();
            prop.englishPsalms = ui.cbEnglishPsalms -> isChecked();
            prop.strongNumber = ui.cbStrongNumber -> isChecked();
            prop.useChapterHead = ui.cbUseChapterHead -> isChecked();
            prop.useRightAlignment = ui.cbUseRightAlignment -> isChecked();
            prop.noForcedLineBreaks = ui.cbNoForcedLineBreaks -> isChecked();
            prop.categories = ui.LECategories -> text();
            prop.defaultEncoding = ui.comBEncoding->currentText();
            prop.language = ui.comBLanguage->currentText();
            prop.desiredFontName = ui.LEDesiredFontName -> text();
            prop.desiredFontPath = ui.LEDesiredFontPath -> text();
            prop.strongsDirectory = ui.LEStrongDir -> text();
            prop.htmlFilter = ui.LEHtmlFilter -> text();
            prop.soundDirectory = ui.LESoundDir -> text();
            prop.installFonts = ui.LEInstallFonts -> text();
            prop.desiredUIFont = ui.LEDesiredUIFont -> text();

            ui.tabWidget->setCurrentIndex (0);
            validProperties = true;
            if (modeNewProject)
            {
                modeNewProject = false;
                emit createProject(prop);
            }
            else
            {
                prop.prjFN = Config::configuration()->CurProject();
                emit updateProjectProperties(prop);
            }
            QWidget::hide();  //close dialog
        }
    }
}
//-------------------------------------------------------------------
void ProjectProperties::setToolTipLabels()
{
    QString str;

    // used translate.google. retranslate pls bqt.narod.ru/03.htm first table

    // module name
    str = QString(tr("<i>The full name of the module</i><br><br>"
                     "Must be in the language module, as far as possible according to the printed edition.<br>"
                     "Must be unique for each module."));
    ui.sModuleName->setToolTip(str);

    // copyright
    str = QString(tr("<i>Copyright</i><br><br>"
                     "The year of publication and copyright<br>"
                     "Implies copyright.htm file in the root directory of the module.<br>"
                     "If no option is specified in the file equals PublicDomainText interface language lng.<br>"
                     "If you specify an owner, then the quote will display the specified text and pressing a "
                     "button will transfer the copyright on the page Copyright data from a file copyright.htm."
                     ));
    ui.sAuthor->setToolTip(str);

    // categories
    str = QString(tr("<i>Binding to the module category</i><br><br>"
                     "By default, the program divides the Bible into the category OT, NT, Apocrypha.<br>"
                     "You must specify additional parameters for the Bible:<br>"
                     "| Language | Translation Type | Denomination |<br>"
                     "For books, specify the subject, keywords (tags)."
                     ));
    ui.sCategories->setToolTip(str);

    // desiredfontname
    str = QString(tr("<i>The font, which will use the program to display the text of books (parts) in the main window.</i><br><br>"
                     "If <b>DesiredFontName</b> coincides not only with the name of the font, "
                     "but also with the name of the font file (no file extension TTF), "
                     "the program will automatically upload it when referring to the module. Therefore, "
                     "a file with special characters when creating the module should be called by the name of "
                     "the font (with the extension TTF). Specifies the font supports Unicode: Arial, "
                     "Comic Sans MS, Courier New, Microsoft Sans Serif, Times New Roman."
                     "We recommend using Times New Roman for Vista and 7, and Microsoft Sans Serif for XP."
                     ));
    ui.sDesiredfontName->setToolTip(str);

    // desiredfontpath
    str = QString(tr("<i>The path to font file</i><br><br>"));
    ui.sDesiredFontPath->setToolTip(str);

    // desireuifont
    str = QString(tr("<i>Specifies the font header titles</i><br><br>"));
    ui.sDesiredUIfont->setToolTip(str);

    // encoding
    str = QString(tr("<i>Encoding module by default</i><br><br>"
                     "This parameter defaults to <b>UTF-8</b>.<br>"
                     "Windows-1250 for Central European languages ​​that use Latin script letters "
                     "(Polish, Czech, Slovak, Hungarian, Slovenian, Croatian, Romanian and Albanian)"
                     "<b>Windows-1251</b> for Cyrillic alphabets<br>"
                     "<b>Windows-1252</b> for Western languages<br>"
                     "<b>Windows-1253</b> for the Greek Language<br>"
                     "<b>Windows-1254</b> for the Turkish language<br>"
                     "<b>Windows-1255</b> Hebrew<br>"
                     "<b>Windows-1256</b> for the Arabic language<br>"
                     "<b>Windows-1257</b> for the Baltic languages<br>"
                     "<b>Windows-1258</b> for Vietnamese Language<br>"
                     "<b>Utf-8</b>, <b>Utf-16</b> and <b>Utf-32</b> Unicode character set"

                     ));
    ui.sEncoding->setToolTip(str);

    // installfonts
    str = QString(tr("<i>Embedding fonts from a folder in the program module without having to install them in OS<br><b>Fonts Install Pending</b><i><br><br>"
                     "Font files to load must lie at the root of the module. "
                     "Listed separated by commas with no spaces, indicating expansion. "
                     "In the file names spaces are allowed. Cloudy. <br>"
                     "<b>InstallFonts = Greek Uncials.ttf, GU Greek.ttf, GU Hebrew.ttf, NetBibleGreek.ttf</b>"
                     ));
    ui.sInstallFonts->setToolTip(str);

    // shortname
    str = QString(tr("<i>The short name of the module (reduction).</i><br><br>"
                     "Must be unique for each module.<br>"
                     "Option only English characters (if possible - see below).<br>"
                     "Coincides with the module name of the folder \ backup (optional).<br>"
                     "Language through an underscore in those modules "
                     "that exist in multiple languages​​, and then a year after "
                     "an underscore where there are several different editions."
                     "If there is a well-established literary abbreviation for the translation,"
                     "it is used, such as the "
                     "Septuagint - LXX, <br>"
                     "Russian Synodal Version - RSV, <br>"
                     "Translation Kuznetsova - TK, <br>"
                     "the Byzantine text-type (Majority Text) is denoted by the letter [ ]."
                     ));
    ui.sShortName->setToolTip(str);

    str = QString(tr("<i>Version</i><br>"

                     ));
    ui.sVersion->setToolTip(str);

    str = QString(tr("<i></i>Use aprocrypha<br><br>"
                     "The presence of a module or comments deuterocanonical books "
                     "(11 books according to the file in the root directory BibleBooks.cfg bQ)<br>"
                     "This parameter can be set to <b>Y</b> only if the Bible = <b>Y</b>"
                     ));
    ui.cbApocrypha->setToolTip(str);

    str = QString(tr("<i>The presence of a zero in the module chapter</i><br><br>"
                     "This option affects the display chapter numbers in the control panel "
                     "and to synchronize at the transition between the modules. "
                     "Put <b>Y</b> when there is an introduction to the book.<br>"
                     "If <b>ChapterZero = Y</b>, the null head is synchronized with the "
                     "file 001. * In the folder with the audio book, the first - to 002. *, Etc."
                     ));
    ui.cbChapterZero->setToolTip(str);

    str = QString(tr("<i>The use of English in the module numbering of verses</i><br><br>"
                     "If EnglishPsalms = Y, then the numbering changes to the adopted in the CPR as follows:"
                     "(code snippet)<br><br>"
                     "JOB = 40:1-40:5 # -1; 41:1-41:8 # -1 <br>"
                     "JOB40 = 1-5 # 30, # 6-24 -5<br>"
                     "JOB41 = 1-8 # 19, # 9-34 -8<br>"
                     "PSA = 10:1-147:11 # -1<br>"
                     "ECC = 5:1 # -1<br>"
                     "SNG = 6:13 # +1<br>"
                     "DAN = 4:1-4:3 # -1<br>"
                     "DAN3 = 24-30 # 67<br>"
                     "DAN4 = 1-3 # 97, # 4-37 -3<br>"
                     "JON = 1:17 # 1<br>"
                     "ROM = 16:25-16:27 # -2<br>"
                     "ROM16 = 25-27 # -1<br>"
                     "ECC5 = 1 # 16, # -1 2-20<br>"
                     "SNG1 = 2-16 # -1<br>"
                     "SNG6 = 13 # -12<br>"
                     "SNG7 = 1-13 # 1<br>"
                     "JON2 = 1-10 # 1<br>"
                     "PRO13 = 14-25 # 1<br>"
                     "PRO18 = 8-24 # 1<br>"
                     "2CO13 = 13 # -1<br><br>"
                     "This parameter can be set to <b>Y</b> only if the <b>Bible = Y</b>"
                     ));
    ui.cbEnglishPsalms->setToolTip(str);

    str = QString(tr("<i>Module Type (Bible or comments or a book).</i><br><br>"
                     "<b>Y</b> if it is put to the Bible or commentary, <b>N</b> if it is a book."));
    ui.cbmoduleType->setToolTip(str);

    str = QString(tr("<i>New testament</i><br><br>"
                     "The presence of books in the module or the comments "
                     "of the New Testament (27 books according to the file in the root directory BibleBooks.cfg bQoute)"
                     ));
    ui.cbNewTestament->setToolTip(str);

    str = QString(tr("<i>Separation is not the verses, and in paragraphs<i><br><br>"
                     "Poems do not appear in the form of paragraphs force."
                     ));
    ui.cbNoForcedLineBreaks->setToolTip(str);

    str = QString(tr("<i>The presence of the module numbers Stronge</i><br><br>"
                     "Put Y if the module is Strong Room.<br>"
                     "This parameter can be set to <b>Y</b> only if the <b>Bible = Y</b>"
                     ));
    ui.cbStrongNumber->setToolTip(str);

    str = QString(tr("<i>Support for displaying chapter titles</i><br><br>"
                     "Displays the preface in the form of the zero line without a number, "
                     "if it is present between the sign and the sign of the head of the first verse."
                     ));
    ui.cbUseChapterHead->setToolTip(str);

    // userightalignment
    str = QString(tr("<i>Support for right alignment</i><br><br>"
                     "Put <b>Y</b> if the module you want to display the entire text to the right"));
    ui.cbUseRightAlignment->setToolTip(str);
}
//-------------------------------------------------------------------
void ProjectProperties::setData()
{
    QStringListModel *modelEncoding;
    modelEncoding = new QStringListModel(getFillEncoding(), this);
    ui.comBEncoding->setModel(modelEncoding);
}
//-------------------------------------------------------------------
void ProjectProperties::hideshow()
{
    /// strong number
    ui.LEStrongDir->setVisible(ui.cbStrongNumber->isChecked());
    ui.pushButtonStrongDir->setVisible(ui.cbStrongNumber->isChecked());

    /// sound
    ui.LESoundDir->setVisible(ui.cbSound->isChecked());
    ui.pushButtonSoundDir->setVisible(ui.cbSound->isChecked());
}

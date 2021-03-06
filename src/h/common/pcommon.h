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


#ifndef __PCOMMON_H__
#define __PCOMMON_H__

class QString;
class QStringList;
#include  <QTreeWidgetItem>
class QTextCodec;


#include "projectproperty.h"
#include "config.h"

//======== процедуры и функции общего назначения ============================
//---------------------------------------------------------------------------
QStringList SplitNumString(QString Str, QString Deviders);
int pSign(int x);                //сигнум. Возвращает 1 if x>0; -1 if x<0; 0 if x=0
//int pRound(double x);             //округляет вещ.число до целого;
double pRound(double x, int Accuracy); //округляет вещ.число до указанного знака;
QString pNumToStr(int N, QString One, QString Two, QString Five); //определяем склонение слова (день,дня,дней)
QString pBoolToStr(bool B);
bool pStrToBool(QString S);
QString pQString(char str[65000]);  //преобразуем тип убирая символ перевода строки
//дополняет строку до длины Len, выравнивая по Align (Left,Center,Right)
QString pFillString(QString Str, QString Chr, int Len, QString Align);
//QString FontStyleToString(TFont *FS);
//TFontStyles StringToFontStyle(QString FS);
//QString AlignmentToString(TAlignment A);
//TAlignment StringToAlignment(QString S);
QString GetOptionName(QString OptN);
QString GetOptionValue(QString OptN);
QString pMonthName(int n); //Полное название месяца из его номера
QString pMonthName(int n, QString format); //Название месяца из его номера в формате format
QString ZodiakName(int n);    //Название зодиакального зозвездия из его номера
int DaysInYear(long cYear);      //кол-во дней в году
int DaysInYear_Jul(long cYear);  //кол-во дней в году по Юлианскому календарю
bool LeapYear(long cYear);	//високосный ли год
bool LeapYear_Jul(long cYear);
int HoursInYear(long cYear);   //кол-во часов в году
int MinutesInYear(long cYear); //кол-во минут в году
int SecondsInYear(long cYear); //кол-во секунд в году
int Digit(int Number);  //ссумирует цифры числа между собой до получения одной цифры
QString IntToLetter(int n); //возвращает букву латинского алфавита
QString IntToLetter(int n, QString str); //возвращает букву из строки
int LetterToInt(QString letter); //возвращает номер буквы латинского алфавита
int LetterToInt(QString letter, QString srcAlphabet);//возвращает номер буквы в указанном алфавите
int r2i(QChar letter); //вспомогательная функция для RomanToInt()
int RomanToInt(QString romanInt); //переводит латинские числа в арабские
QString IntToRoman(int n); //переводит арабские числа в латинские
int max(int x1, int x2, int x3); //возвращает максимальный из аргументов
bool SameDigits(int x); //true если все цифры числа одинаковы, напр. 777
double Proporsion(double x, double y);//вычисляет пропорцию
int BooltoInt(bool foo); // возвращает true=1 false=0
QString BooltoQString(bool foo); // возвращает true= "1" false =  "0"
bool QStringtoBool(QString foo); // возвращает false if str.indexOf("0") else true
QString ist(QString str); // возвращает "none" если строка "", если нет, то возвращает саму строку
QString incstr(QString str, int n, QString mychar);  // increase the line, увеличивает строку добавляя символ в начале строки
QString tagToQuestion(QString tag); // функции, которые делают из <p>  ?p? и обратно.
QString questionToTag(QString question);
QString editStringList(QString list, QStringList tags, bool f); // функция нужная для экспорта, в зависимости от bool меняет теги на ?*_? или <*>
QString getCenterTag(QString str); // заменяет <p align="center" на <center
QString getHtmlCoolCode(QString str, QString i, QString chapter, bool chap);
QString getParseTagSpan(QString str, QString text, QString tag);
int getDepthTreeWidgetItem(QTreeWidgetItem *item); // вернуть глубину QTreeWidgetItem
QStringList getFillShortName(); // возвращает заполнение для combobox базовых коротких названий
QStringList getFillEncoding(); // возвращает заполнение для combobox кодировок
QString getTextInStr(QString str, int begin = 5, int end = 9); // возвращает текст от begin до end символа ( не реализована, работает чтобы доставать shortname)
QString getCheckShortNameForFile(QString str, QString full); // возвращает порядковый номер shortname(str) или полное название книги. Работает для уменьшения длины названия файла
QString checkTag(QString tag); // проверяет строку, начинается и заканчивается ли она на < и >
QString uncheckTag(QString tag); // обратная операция
QString checkEndTag(QString tag); // возвращает из тега <tag> </tag>
QString checkProcentRol(QString shortname, QString out, int procent = GL_PROCENT_OF_MATCHES); // разбивает shortname на части и проверяет насколько совпадает с getFillShortName()
//если с точностью до процента совпадает, то возвращает номер в shortname, а если нету такого, то out Применяется для импорта книги
QTextCodec* getCodecOfEncoding(QString encoding); // получает строку с названием кодировки и возвращает кодес с этй кодировкой ( написана для уменьшения дублирования кода)
QString removeFirst(QString str, QString remove); // возвращает строку с удаленным первым вхождением

/**
  @function
  Number of files with the division of the text tag (split files)
  @param *text  text
  @param tag  tag
  */
int countTheNumberOfFiles(QString *text, QString tag);
/**
  @function
  Replace spaces in qstring to text(default = }<B-B>{)
  @param str  qstring
  @param text  default = }<B-B>{
  */
QString replaceSpaceInStrToText(QString str, QString text = "}<B-B>{");

/**
  @function
  Function to show to qDebug moduleproperties
  @param *pr  moduleproperties
  */
void printToDebugModuleProperties(ModuleProperties *pr);

void visitTree(QStringList &list, QTreeWidgetItem *item);
QStringList visitTree(QTreeWidget *tree);
QStringList getParentText(QTreeWidgetItem *item);

/**
  @function
  Return path of file
  @param file  path of file
  @return path  path
  */
QString getFolderFile(QString *file);



#endif // __PCOMMON_H__

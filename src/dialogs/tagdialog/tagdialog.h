#ifndef TAGDIALOG_H
#define TAGDIALOG_H

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
 *    Years: 2012
 *    WebSite: https://github.com/WarmongeR1/bQella
 */



#include <QDialog>

namespace Ui {
    class TagDialog;
}

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(QWidget *parent = 0);
    ~TagDialog();
    bool valid() { return validProperties; }
    QString getTag() { return tag; }

signals:
    void addTag(QString lText);

public slots:
    void accept();
    void reject();
    void setProperties(QString lText);

private:
    Ui::TagDialog *ui;

    QString tag;
    bool validProperties;

};

#endif // TAGDIALOG_H

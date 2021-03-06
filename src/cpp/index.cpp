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

#include "index.h"

#include <QApplication>
#include <QStringList>
#include <QUrl>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QTextCodec>


struct Term {
    Term() : frequency(-1) {}
    Term( const QString &t, int f, QVector<Document> l ) : term( t ), frequency( f ), documents( l ) {}
    QString term;
    int frequency;
    QVector<Document>documents;
    bool operator<( const Term &i2 ) const { return frequency < i2.frequency; }
};

QDataStream &operator>>( QDataStream &s, Document &l )
{
    s >> l.docNumber;
    s >> l.frequency;
    return s;
}

QDataStream &operator<<( QDataStream &s, const Document &l )
{
    s << (qint16)l.docNumber;
    s << (qint16)l.frequency;
    return s;
}

Index::Index( const QString &dp, const QString &hp )
    : QObject( 0 ), docPath( dp )
{
    Q_UNUSED(hp);

    alreadyHaveDocList = false;
    lastWindowClosed = false;
    connect( qApp, SIGNAL(lastWindowClosed()),
             this, SLOT(setLastWinClosed()) );
}

Index::Index( const QStringList &dl, const QString &hp )
    : QObject( 0 )
{
    Q_UNUSED(hp);
    docList = dl;
    alreadyHaveDocList = true;
    lastWindowClosed = false;
    connect( qApp, SIGNAL(lastWindowClosed()),
             this, SLOT(setLastWinClosed()) );
}

void Index::setLastWinClosed()
{
    lastWindowClosed = true;
}

void Index::setDictionaryFile( const QString &f )
{
    dictFile = f;
}

void Index::setDocListFile( const QString &f )
{
    docListFile = f;
}

void Index::setDocList( const QStringList &lst )
{
    docList = lst;
}

int Index::makeIndex()
{
    if ( !alreadyHaveDocList )
        setupDocumentList();
    if ( docList.isEmpty() )
        return 1;
    QStringList::Iterator it = docList.begin();
    int steps = docList.count() / 100;
    if ( !steps )
        steps++;
    int prog = 0;
    for ( int i = 0; it != docList.end(); ++it, ++i ) {
        if ( lastWindowClosed ) {
            return -1;
        }
        QUrl url(*it);
        parseDocument( url.toLocalFile(), i );
        if ( i%steps == 0 ) {
            prog++;
            emit indexingProgress( prog );
        }
    }
    return 0;
}

void Index::setupDocumentList()
{
    QDir d( docPath );
    QStringList filters;
    filters.append(QString("*.html"));
    QStringList lst = d.entryList(filters);
    QStringList::ConstIterator it = lst.constBegin();
    for ( ; it != lst.constEnd(); ++it )
        docList.append( QString("file:") + docPath + QString("/") + *it );
}

void Index::insertInDict( const QString &str, int docNum )
{
    if ( str == QString("amp") || str == QString("nbsp"))
        return;
    Entry *e = 0;
    if ( dict.count() )
        e = dict[ str ];

    if ( e ) {
        if ( e -> documents.last().docNumber != docNum )
            e -> documents.append( Document(docNum, 1 ) );
        else
            e -> documents.last().frequency++;
    } else {
        dict.insert( str, new Entry( docNum ) );
    }
}

QString Index::getCharsetForDocument(QFile *file)
{
    QTextStream s(file);
    QString contents = s.readAll();

    QString encoding;
    int start = contents.indexOf(QString("<meta"), 0, Qt::CaseInsensitive);
    if (start > 0) {
        int end = contents.indexOf(QString(">"), start);
        QString meta = contents.mid(start+5, end-start);
        meta = meta;
        QRegExp r(QString("charset=([^\"\\s]+)"));
        if (r.indexIn(meta) != -1) {
            encoding = r.cap(1);
        }
    }

    file -> seek(0);
    if (encoding.isEmpty())
        return QString("UTF-8");
    return encoding;
}

void Index::parseDocument( const QString &filename, int docNum )
{
    QFile file( filename );
//    qDebug() << "opening file parseDocument: " << filename;
    if ( !file.open(QFile::ReadOnly) ) {
        qWarning( (QString("can not open file ") + filename).toAscii().constData() );
        return;
    }

    QTextStream s(&file);
    QString en = getCharsetForDocument(&file);
    s.setCodec(QTextCodec::codecForName(en.toUtf8().constData()));

    QString text = s.readAll();
    if (text.isNull())
        return;

    bool valid = true;
    const QChar *buf = text.unicode();
    QChar str[64];
    QChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( j < text.length() ) {
        if ( c == QChar('<') || c == QChar('&') ) {
            valid = false;
            if ( i > 1 )
                insertInDict( QString(str,i), docNum );
            i = 0;
            c = buf[++j];
            continue;
        }
        if ( ( c == QChar('>') || c == QChar(';') ) && !valid ) {
            valid = true;
            c = buf[++j];
            continue;
        }
        if ( !valid ) {
            c = buf[++j];
            continue;
        }
        if ( ( c.isLetterOrNumber() || c == QChar('_') ) && i < 63 ) {
            str[i] = c;
            ++i;
        } else {
            if ( i > 1 )
                insertInDict( QString(str,i), docNum );
            i = 0;
        }
        c = buf[++j];
    }
    if ( i > 1 )
        insertInDict( QString(str,i), docNum );
    file.close();
}

void Index::writeDict()
{
    QFile f( dictFile );
    if ( !f.open(QFile::WriteOnly ) )
        return;
    QDataStream s( &f );
    for(QHash<QString, Entry *>::Iterator it = dict.begin(); it != dict.end(); ++it) {
        s << it.key();
        s << it.value() -> documents.count();
        s << it.value() -> documents;
    }
    f.close();
    writeDocumentList();
}

void Index::writeDocumentList()
{
    QFile f( docListFile );
    if ( !f.open(QFile::WriteOnly ) )
        return;
    QDataStream s( &f );
    s << docList;
}

void Index::readDict()
{
    QFile f( dictFile );
    if ( !f.open(QFile::ReadOnly ) )
        return;

    dict.clear();
    QDataStream s( &f );
    QString key;
    int numOfDocs;
    QVector<Document> docs;
    while ( !s.atEnd() ) {
        s >> key;
        s >> numOfDocs;
        docs.resize(numOfDocs);
        s >> docs;
        dict.insert( key, new Entry( docs ) );
    }
    f.close();
    readDocumentList();
}

void Index::readDocumentList()
{
    QFile f( docListFile );
    if ( !f.open(QFile::ReadOnly ) )
        return;
    QDataStream s( &f );
    s >> docList;
}

QStringList Index::query( const QStringList &terms, const QStringList &termSeq, const QStringList &seqWords )
{
    QList<Term> termList;
    for (QStringList::ConstIterator it = terms.begin(); it != terms.end(); ++it ) {
        Entry *e = 0;
        if ( (*it).contains(QChar('*')) ) {
            QVector<Document> wcts = setupDummyTerm( getWildcardTerms( *it ) );
            termList.append( Term(QString("dummy"), wcts.count(), wcts ) );
        } else if ( dict[ *it ] ) {
            e = dict[ *it ];
            termList.append( Term( *it, e -> documents.count(), e -> documents ) );
        } else {
            return QStringList();
        }
    }
    if ( !termList.count() )
        return QStringList();
    qSort(termList);

    QVector<Document> minDocs = termList.takeFirst().documents;
    for(QList<Term>::Iterator it = termList.begin(); it != termList.end(); ++it) {
        Term *t = &(*it);
        QVector<Document> docs = t -> documents;
        for(QVector<Document>::Iterator minDoc_it = minDocs.begin(); minDoc_it != minDocs.end(); ) {
            bool found = false;
            for (QVector<Document>::ConstIterator doc_it = docs.constBegin(); doc_it != docs.constEnd(); ++doc_it ) {
                if ( (*minDoc_it).docNumber == (*doc_it).docNumber ) {
                    (*minDoc_it).frequency += (*doc_it).frequency;
                    found = true;
                    break;
                }
            }
            if ( !found )
                minDoc_it = minDocs.erase( minDoc_it );
            else
                ++minDoc_it;
        }
    }

    QStringList results;
    qSort( minDocs );
    if ( termSeq.isEmpty() ) {
        for(QVector<Document>::Iterator it = minDocs.begin(); it != minDocs.end(); ++it)
            results << docList.at((int)(*it).docNumber);
        return results;
    }

    QString fileName;
    for(QVector<Document>::Iterator it = minDocs.begin(); it != minDocs.end(); ++it) {
        fileName =  docList[ (int)(*it).docNumber ];
        if ( searchForPattern( termSeq, seqWords, fileName ) )
            results << fileName;
    }
    return results;
}

QString Index::getDocumentTitle( const QString &fullFileName )
{
    QUrl url(fullFileName);
    QString fileName = url.toLocalFile();

    if (documentTitleCache.contains(fileName))
        return documentTitleCache.value(fileName);

    QFile file( fileName );
//    qDebug() << "opening file getDocumentTitle: " << fileName;
        if ( !file.open( QFile::ReadOnly ) ) {
        qWarning( (QString("cannot open file ") + fileName).toAscii().constData() );
        return fileName;
    }
    QTextStream s( &file );
    QString text = s.readAll();

    int start = text.indexOf(QString("<title>"), 0, Qt::CaseInsensitive) + 7;
    int end = text.indexOf(QString("</title>"), 0, Qt::CaseInsensitive);

    QString title = ( end - start <= 0 ? tr("Untitled") : text.mid( start, end - start ) );
    documentTitleCache.insert(fileName, title);
    return title;
}

QStringList Index::getWildcardTerms( const QString &term )
{
    QStringList lst;
    QStringList terms = split( term );
    QStringList::Iterator iter;

    for(QHash<QString, Entry*>::Iterator it = dict.begin(); it != dict.end(); ++it) {
        int index = 0;
        bool found = false;
        QString text( it.key() );
        for ( iter = terms.begin(); iter != terms.end(); ++iter ) {
            if ( *iter == QString("*") ) {
                found = true;
                continue;
            }
            if ( iter == terms.begin() && (*iter)[0] != text[0] ) {
                found = false;
                break;
            }
            index = text.indexOf( *iter, index );
            if ( *iter == terms.last() && index != (int)text.length()-1 ) {
                index = text.lastIndexOf( *iter );
                if ( index != (int)text.length() - (int)(*iter).length() ) {
                    found = false;
                    break;
                }
            }
            if ( index != -1 ) {
                found = true;
                index += (*iter).length();
                continue;
            } else {
                found = false;
                break;
            }
        }
        if ( found )
            lst << text;
    }

    return lst;
}

QStringList Index::split( const QString &str )
{
    QStringList lst;
    int j = 0;
    int i = str.indexOf(QChar('*'), j );

    if (str.startsWith(QString("*")))
        lst << QString("*");

    while ( i != -1 ) {
        if ( i > j && i <= (int)str.length() ) {
            lst << str.mid( j, i - j );
            lst << QString("*");
        }
        j = i + 1;
        i = str.indexOf(QChar('*'), j );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
        lst << str.mid( j, l - j + 1 );

    return lst;
}

QVector<Document> Index::setupDummyTerm( const QStringList &terms )
{
    QList<Term> termList;
    for (QStringList::ConstIterator it = terms.begin(); it != terms.end(); ++it) {
        Entry *e = 0;
        if ( dict[ *it ] ) {
            e = dict[ *it ];
            termList.append( Term( *it, e -> documents.count(), e -> documents ) );
        }
    }
    QVector<Document> maxList(0);
    if ( !termList.count() )
        return maxList;
    qSort(termList);

    maxList = termList.takeLast().documents;
    for(QList<Term>::Iterator it = termList.begin(); it != termList.end(); ++it) {
        Term *t = &(*it);
        QVector<Document> docs = t -> documents;
        for (QVector<Document>::iterator docIt = docs.begin(); docIt != docs.end(); ++docIt ) {
            if ( maxList.indexOf( *docIt ) == -1 )
                maxList.append( *docIt );
        }
    }
    return maxList;
}

void Index::buildMiniDict( const QString &str )
{
    if ( miniDict[ str ] )
        miniDict[ str ] -> positions.append( wordNum );
    ++wordNum;
}

bool Index::searchForPattern( const QStringList &patterns, const QStringList &words, const QString &fileName )
{
    QUrl url(fileName);
    QString fName = url.toLocalFile();
    QFile file( fName );
        qDebug() << "opening file searchForPattern: " << fName;
    if ( !file.open( QFile::ReadOnly ) ) {
        qWarning( (QString("cannot open file ") + fName).toAscii().constData() );
        return false;
    }

    wordNum = 3;
    miniDict.clear();
    QStringList::ConstIterator cIt = words.begin();
    for ( ; cIt != words.end(); ++cIt )
        miniDict.insert( *cIt, new PosEntry( 0 ) );

    QTextStream s( &file );
    QString text = s.readAll();
    bool valid = true;
    const QChar *buf = text.unicode();
    QChar str[64];
    QChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( j < text.length() ) {
        if ( c == QChar('<') || c == QChar('&') ) {
            valid = false;
            if ( i > 1 )
                buildMiniDict( QString(str,i) );
            i = 0;
            c = buf[++j];
            continue;
        }
        if ( ( c == QChar('>') || c == QChar(';') ) && !valid ) {
            valid = true;
            c = buf[++j];
            continue;
        }
        if ( !valid ) {
            c = buf[++j];
            continue;
        }
        if ( ( c.isLetterOrNumber() || c == QChar('_') ) && i < 63 ) {
            str[i] = c;
            ++i;
        } else {
            if ( i > 1 )
                buildMiniDict( QString(str,i) );
            i = 0;
        }
        c = buf[++j];
    }
    if ( i > 1 )
        buildMiniDict( QString(str,i) );
    file.close();

    QStringList::ConstIterator patIt = patterns.begin();
    QStringList wordLst;
    QList<uint> a, b;
    QList<uint>::iterator aIt;
    for ( ; patIt != patterns.end(); ++patIt ) {
        wordLst = (*patIt).split(QChar(' '));
        a = miniDict[ wordLst[0] ] -> positions;
        for ( int j = 1; j < (int)wordLst.count(); ++j ) {
            b = miniDict[ wordLst[j] ] -> positions;
            aIt = a.begin();
            while ( aIt != a.end() ) {
                if ( b.contains( *aIt + 1 )) {
                    (*aIt)++;
                    ++aIt;
                } else {
                    aIt = a.erase( aIt );
                }
            }
        }
    }
    if ( a.count() )
        return true;
    return false;
}

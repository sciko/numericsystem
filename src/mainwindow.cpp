/********************************************************************************/
/*                                                                              */
/*    Copyright 2012 Alexander Vorobyev (Voral)                                 */
/*    http://va-soft.ru/                                                        */
/*                                                                              */
/*    This file is part of basetest.                                            */
/*                                                                              */
/*    Basetest is free software: you can redistribute it and/or modify          */
/*    it under the terms of the GNU General Public License as published by      */
/*    the Free Software Foundation, either version 3 of the License, or         */
/*    (at your option) any later version.                                       */
/*                                                                              */
/*    Basetest is distributed in the hope that it will be useful,               */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*    GNU General Public License for more details.                              */
/*                                                                              */
/*    You should have received a copy of the GNU General Public License         */
/*    along with basetest.  If not, see <http://www.gnu.org/licenses/>.         */
/*                                                                              */
/********************************************************************************/

#include <ctime>
#include "mainwindow.h"
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QMenuBar>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QStyle>
#include <QtGui/QDesktopWidget>
#include <QtEndian>
#include "vconfdlg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // For lang menu
    tr("English");
    qApp->installTranslator(&appTranslator);
    qApp->installTranslator(&qtTranslator);
    this->main =new QWidget(this);
    this->gbSettings = new QGroupBox();
    this->gbTest = new QGroupBox();
    this->laMain = new QVBoxLayout();
    this->laMain->addWidget(this->gbSettings,0);
    this->laMain->addWidget(this->gbTest,0,Qt::AlignTop);
    this->main->setLayout(this->laMain);
    this->setCentralWidget(this->main);

    this->lbBases = new QLabel();
    this->lbRange = new QLabel();
    this->lbBinInverse = new QLabel();;
    this->lbEndian = new QLabel();;
    this->laSettings = new QGridLayout();
    this->laSettings->addWidget(this->lbBases,0,0,1,1,Qt::AlignLeft);
    this->laSettings->addWidget(this->lbRange,1,0,1,1,Qt::AlignLeft);
    this->laSettings->addWidget(this->lbEndian,0,1,1,1,Qt::AlignLeft);
    this->laSettings->addWidget(this->lbBinInverse,1,1,1,1,Qt::AlignLeft);
    this->gbSettings->setLayout(this->laSettings);

    this->btGenerate = new QPushButton();
    connect(this->btGenerate,SIGNAL(clicked()),this,SLOT(onGenerate()));

    QFont ft =  this->font();
    ft.setFamily("Monospace");
    ft.setStyleHint(QFont::TypeWriter);

    this->laTest = new QGridLayout();
    for(register int i = 0; i < testItemCount; ++i)
    {
        this->lSource.append(new QLabel());
        this->lDestination.append(new QLineEdit());
        this->lResult.append(new QLabel());
        this->laTest->addWidget(this->lSource.at(i),i,0,1,1,Qt::AlignCenter);
        this->laTest->addWidget(this->lDestination.at(i),i,1,1,1,Qt::AlignLeft);
        this->laTest->addWidget(this->lResult.at(i),i,2,1,1,Qt::AlignLeft);
        this->lSource.at(i)->setFont(ft);
        this->lDestination.at(i)->setFont(ft);
    }
    this->lbResultTitle = new QLabel();
    this->lbResultCount = new QLabel();
    this->lbResultPercent = new QLabel();
    this->laTest->addWidget(this->lbResultTitle,0,3,1,1,Qt::AlignCenter);
    this->laTest->addWidget(this->lbResultCount,1,3,1,1,Qt::AlignCenter);
    this->laTest->addWidget(this->lbResultPercent,2,3,1,1,Qt::AlignCenter);

    this->btCheck = new QPushButton();
    connect(this->btCheck,SIGNAL(clicked()),this,SLOT(onCheck()));
    this->btStop = new QPushButton();
    this->btStop->setFocusPolicy(Qt::NoFocus);
    connect(this->btStop,SIGNAL(clicked()),this,SLOT(onStop()));
    this->laTest->addWidget(this->btGenerate,testItemCount-3,3,1,1,Qt::AlignCenter);
    this->laTest->addWidget(this->btStop,testItemCount-2,3,1,1,Qt::AlignCenter);
    this->laTest->addWidget(this->btCheck,testItemCount-1,3,1,1,Qt::AlignCenter);
    this->gbTest->setLayout(this->laTest);

    this->mnConf = this->menuBar()->addMenu("");
    this->acConfig= new QAction("", this);
    connect(this->acConfig, SIGNAL(triggered()), this, SLOT(onConfig()));

    this->mnLang = this->menuBar()->addMenu(tr(""));

    this->agLang = new QActionGroup(this);
    connect(this->agLang, SIGNAL(triggered(QAction *)), this, SLOT(switchLanguage(QAction *)));
    QDir dir(":/trans");
    QStringList fileNames = dir.entryList(QStringList("basetest_*.qm"));
    for (int i = 0; i < fileNames.size(); ++i)
    {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.truncate(locale.lastIndexOf('.'));
        QTranslator translator;
        translator.load(fileNames[i], ":/trans");
        QString language = translator.translate("MainWindow", "English");
        QAction *action = new QAction(language, this);
        action->setCheckable(true);
        action->setData(locale);
        this->mnLang->addAction(action);
        this->agLang->addAction(action);
        if (locale.compare(QLocale::system().name().left(2)) == 0) action->setChecked(true);
    }

    this->mnAbout = this->menuBar()->addMenu("");
    this->acAboutQt= new QAction("", this);
    connect(this->acAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    this->acAbout= new QAction("", this);
    connect(this->acAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
    this->mnAbout->addAction(this->acAbout);
    this->mnAbout->addAction(this->acAboutQt);
    this->btCheck->setEnabled(false);
    this->btStop->setEnabled(false);

    this->agPreset = new QActionGroup(this);
    connect(this->agPreset, SIGNAL(triggered(QAction *)), this, SLOT(onLoadPreset(QAction*)));
    onUpdatePresets();
    retranslateUi();
}

MainWindow::~MainWindow()
{
    this->acAboutQt->deleteLater();
    this->acAbout->deleteLater();
    this->agLang->deleteLater();
    this->mnAbout->deleteLater();
    this->mnLang->deleteLater();
    this->btGenerate->deleteLater();
    this->btCheck->deleteLater();
    this->btStop->deleteLater();
    for(register int i = 0; i < testItemCount; ++i)
    {
        QLabel * label = qobject_cast<QLabel *>(this->lSource.at(i));
        delete label;
        QLineEdit *answer = qobject_cast<QLineEdit *>(this->lDestination.at(i));
        delete answer;
        label = qobject_cast<QLabel *>(this->lResult.at(i));
        delete label;
    }
    this->lbBinInverse->deleteLater();
    this->lbEndian->deleteLater();
    this->lbBases->deleteLater();
    this->lbRange->deleteLater();
    this->lbResultTitle->deleteLater();
    this->lbResultCount->deleteLater();
    this->lbResultPercent->deleteLater();
    this->laSettings->deleteLater();
    this->laTest->deleteLater();
    this->gbSettings->deleteLater();
    this->gbTest->deleteLater();
    this->laMain->deleteLater();
    this->main->deleteLater();
}
int MainWindow::inverseWord(int value)
{
    int low = (value & 0x00FF) << 8;
    value  = (value & 0xFF00) >> 8;
    return (value + low);
}

void MainWindow::onGenerate()
{
    this->btGenerate->setEnabled(false);
    this->btCheck->setEnabled(true);
    this->btStop->setEnabled(true);
    this->mnConf->setEnabled(false);
    // QT 4.7
    this->lbResultCount->setText("");
    this->lbResultPercent->setText("");
    qsrand((uint)time (NULL));

    int maxLenght = QString::number((VConfDlg::getRangeMax()<=255) ? 255 : 65535,VConfDlg::getBaseSource()).length();
    for(register int i = 0; i < testItemCount; ++i)
    {
        int rnd = qrand();
        unsigned int value = rnd % (VConfDlg::getRangeMax() - VConfDlg::getRangeMin() + 1)+ VConfDlg::getRangeMin();
        QLabel * label = qobject_cast<QLabel *>(this->lSource.at(i));
        if (VConfDlg::getEndianBig()&&(VConfDlg::getRangeMax()>255)
                &&((VConfDlg::getBaseSource()==2)||(VConfDlg::getBaseSource()==16)))
        {
            value=this->inverseWord(value);
        }
        QString str =QString("%1").arg(QString::number(value,VConfDlg::getBaseSource()),maxLenght,'0');
        label->setText(((VConfDlg::getBaseSource()==2)&&(VConfDlg::getBinInverse())) ? this->inverse(str) : str);
        label = qobject_cast<QLabel *>(this->lResult.at(i));
        label->setText("");
        QLineEdit *answer = qobject_cast<QLineEdit *>(this->lDestination.at(i));
        answer->setText("");
    }
}
void MainWindow::onStop()
{
    this->mnConf->setEnabled(true);
    this->btGenerate->setEnabled(true);
    this->btCheck->setEnabled(false);
    this->btStop->setEnabled(false);
}
void MainWindow::onCheck()
{
    int goodAnswer = 0;
    for(register int i = 0; i < testItemCount; ++i)
    {
        QLabel * label = qobject_cast<QLabel *>(this->lSource.at(i));
        bool okSrc, okDst;
        QString text = ((VConfDlg::getBaseSource()==2)&&(VConfDlg::getBinInverse())) ?
                    this->inverse(label->text()) : label->text();
        int source = text.toInt(&okSrc,VConfDlg::getBaseSource());
        QLineEdit *answer = qobject_cast<QLineEdit *>(this->lDestination.at(i));
        text = ((VConfDlg::getBaseSource()==2)&&(VConfDlg::getBinInverse())) ?
                    this->inverse(answer->text()) : answer->text();
        int destination = text.toInt(&okDst,VConfDlg::getBaseDestination());
        label = qobject_cast<QLabel *>(this->lResult.at(i));
        if ((!okSrc)||(!okDst))
        {
            label->setText(tr("Error"));
        }
        else
        {
            if (VConfDlg::getEndianBig()&&(VConfDlg::getRangeMax()>255))
            {
                if ((VConfDlg::getBaseSource()==2)||(VConfDlg::getBaseSource()==16))
                {
                    source=this->inverseWord(source);
                }
                if ((VConfDlg::getBaseDestination()==2)||(VConfDlg::getBaseDestination()==16))
                {
                    destination=this->inverseWord(destination);
                }
            }
            if (source==destination)
            {
                label->setText(tr("Ok"));
                ++goodAnswer;
            }
            else
            {
                label->setText(tr("No"));
            }
        }
    }
    this->lbResultCount->setText(QString(tr("%1 from %2")).arg(goodAnswer).arg(testItemCount));
    this->lbResultPercent->setText(QString(tr("%1 %")).arg(goodAnswer*100/testItemCount));
    this->onStop();
}
void MainWindow::onAbout()
{
    QMessageBox::about(this,tr("About Number system"),tr("Simualtor \"Number systems\" for bases between 2 and 36.<br>Version: 1.0<br>Copyright 2012 Alexander Vorobyev (Voral)<br>Autor: Alexander Vorobyev<br>Site: http://va-soft.ru/"));
}
void MainWindow::onConfig()
{
    VConfDlg *dlg = new VConfDlg(this);
    connect(dlg,SIGNAL(updateList()),this,SLOT(onUpdatePresets()));
    dlg->exec();
    this->retranslateUi();
    dlg->deleteLater();
}

void MainWindow::switchLanguage(QAction *action)
{
    QString locale = action->data().toString();
    appTranslator.load("basetest_" + locale, ":/trans");
    qtTranslator.load("qt_" + locale, ":/trans");
    retranslateUi();
}
void MainWindow::retranslateUi()
{
    this->setMaximumSize(this->width()+100,this->height()+10);

    this->mnAbout->setTitle(tr("About"));
    this->mnConf->setTitle(tr("Preset"));
    this->acAboutQt->setText(tr("About &Qt..."));
    this->acAbout->setText(tr("About..."));
    this->acConfig->setText(tr("Preset..."));
    this->mnLang->setTitle(tr("Language"));
    this->btStop->setText(tr("Stop"));
    this->btCheck->setText(tr("Check"));
    this->lbResultTitle->setText(tr("Result:"));
    this->btGenerate->setText(tr("Start"));
    this->lbBases->setText(tr("Bases (source/destination): <b>%1 / %2</b>")
                           .arg(VConfDlg::getBaseSource())
                           .arg(VConfDlg::getBaseDestination()));
    this->lbRange->setText(tr("Numbers range (min/max): <b>%1 / %2</b>")
                           .arg(VConfDlg::getRangeMin())
                           .arg(VConfDlg::getRangeMax()));
    this->lbBinInverse->setText((((VConfDlg::getBaseSource()==2)||(VConfDlg::getBaseDestination()==2))
                                 &&VConfDlg::getBinInverse()) ? tr("Inverse byte order") : "");

    if (((VConfDlg::getBaseSource()!=2)&&(VConfDlg::getBaseDestination()!=2)
         &&(VConfDlg::getBaseSource()!=16)&&(VConfDlg::getBaseDestination()!=16))
        ||(VConfDlg::getRangeMax()<256))
    {
        this->lbEndian->setText("");
    }
    else
    {
        this->lbEndian->setText(tr("Endianness: <b>%1</b>").arg((VConfDlg::getEndianBig()) ? "big-endian" : "little-endian"));
    }

    this->gbTest->setTitle(tr("Test"));
    this->setWindowTitle(tr("Number system"));

    this->btCheck->adjustSize();
    this->btStop->setFixedWidth(this->btCheck->width());
    this->btGenerate->setFixedWidth(this->btCheck->width());
    //calc max field size
    QString tSrc, tDst, tErr;
    QLabel * label = qobject_cast<QLabel *>(this->lSource.at(0));
    tSrc = label->text();

    label->setText(QString("%1").arg(QString::number((VConfDlg::getRangeMax()<=255) ? 255 : 65535,2),VConfDlg::getBaseSource(),'0'));
    label->adjustSize();
    label->setMinimumWidth(label->width());

    label = qobject_cast<QLabel *>(this->lResult.at(0));
    tErr = label->text();
    label->setText(tr("not checking"));
    label->adjustSize();
    label->setMinimumWidth(label->width());
    QLineEdit *answer = qobject_cast<QLineEdit *>(this->lDestination.at(0));
    tDst = answer->text();
    answer->setText(QString("%1").arg(QString::number(VConfDlg::getMaxRangeValue(),2),8,'0'));
    answer->adjustSize();
    answer->setMinimumWidth(answer->width());

    this->gbSettings->adjustSize();
    this->gbTest->adjustSize();
    this->adjustSize();
    this->setMaximumSize(this->size());

    label = qobject_cast<QLabel *>(this->lSource.at(0));
    label->setText(tSrc);
    label = qobject_cast<QLabel *>(this->lResult.at(0));
    label->setText(tErr);
    answer = qobject_cast<QLineEdit *>(this->lDestination.at(0));
    answer->setText(tDst);
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

void MainWindow::onUpdatePresets()
{
    this->mnConf->clear();
    this->mnConf->addAction(this->acConfig);
    QSettings cfg(QSettings::IniFormat, QSettings::UserScope,PROGRAM_NAME,CFG_NAME,this);
    cfg.setIniCodec("UTF-8");
    QStringList presetList = cfg.childGroups();

    if (presetList.count()>0)
    {
        this->mnConf->addSeparator();
        foreach(QString name, cfg.childGroups())
        {
            QAction *action = new QAction(name, this);
            action->setData(name);
            this->mnConf->addAction(action);
            this->agPreset->addAction(action);
        }
    }
}
void MainWindow::onLoadPreset(QAction* action)
{
    VConfDlg::onLoad(action->text());
    retranslateUi();
}
QString MainWindow::inverse(QString str)
{
    QString result;
    for (int i=(str.length()-1); i >=0; --i)
    {
        result.append(str.at(i));
    }
    return result;
}

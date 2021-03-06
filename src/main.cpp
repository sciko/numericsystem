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
#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifndef Q_OS_MAC
    a.setWindowIcon(QPixmap(":/ico/icon.png"));
#endif
    a.setApplicationVersion("1.0");
    a.setApplicationName("Simulator \"Number system\"");
    QTranslator appTranslator;
    appTranslator.load("basetest_" + QLocale::system().name().left(2),":/trans");
    a.installTranslator(&appTranslator);
    MainWindow w;
    w.show();

    return a.exec();
}

/**
 * \file   main.cpp
 * \author Michał Kamiński
 * \date   20-01-2015
 * \brief  Start programu
 */

#include "erdoswindow.h"
#include <QApplication>


/**
 * \brief Funkcja startowa programu
 */
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    ErdosWindow w;
    w.show();

    return a.exec();
}

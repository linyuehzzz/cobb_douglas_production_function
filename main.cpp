#include <QCoreApplication>
#include "RNDTheme.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RNDTheme* t = new RNDTheme();
    t->RunModel();

    return a.exec();
}

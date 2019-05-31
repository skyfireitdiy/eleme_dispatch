#include <orderdispatch.h>
#include "mainform.h"
#include <QApplication>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    MainForm w;
    w.show();
    return app.exec();
}

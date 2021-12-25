#include "nighsky.h"


#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    nightSky w;
    
    w.show();
    
    return a.exec();
}

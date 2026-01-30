#include "nighsky.h"
#include "console.h"
#include "logger.h"

#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
  int ret = -1;

  allocConsole();
    
  QApplication a(argc, argv);
  
  CLogger::getInstance()->setLevel(CLogger::level::INFO);                      
  CLogger::getInstance()->regOutDevice(cmdLine, cmdColorOut);
  CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "Logging engine initialized");
    
  nightSky w;
    
  w.show();
    
  ret = a.exec();

  CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "Loggine engine shutting down");
  CLogger::delInstance();

  deallocConsole();

  return ret;
}

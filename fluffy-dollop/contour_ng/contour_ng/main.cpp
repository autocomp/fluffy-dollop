#include <ctrcore/ctrcore/ctrcore.h>
#include "appcomposer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  bool allInitializationsSuccess = true;
  ctrcore::CtrCore contour;
  allInitializationsSuccess = contour.init();
  if(allInitializationsSuccess)
  {
      AppComposer comp;
      comp.init();
      return a.exec();
  }
  else
  {
      exit(0);
  }
}

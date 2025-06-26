#include <iostream>

#include "ImageViewer.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));

  app.setStyle("Fusion");

  PCO_InitializeLib();
  static int exit = 0;
  try
  {
    ImageViewer imageViewer;
    imageViewer.show();

    exit = app.exec();
  }
  catch (pco::CameraException& err)
  {
    std::cout << "Error Code: " << err.error_code() << std::endl;
    std::cout << err.what() << std::endl;
  }
  

  PCO_CleanupLib();

  return exit;
}

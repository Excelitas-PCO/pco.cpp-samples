#include "ImageViewer.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));

  ImageViewer imageViewer;
  imageViewer.show();

  PCO_InitializeLib();
  auto exit = app.exec();
  PCO_CleanupLib();

  return exit;
}

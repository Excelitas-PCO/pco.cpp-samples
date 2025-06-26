#pragma once
#include <QMainWindow>
#include <QColorSpace>
#include <QLabel>
#include <QMenuBar>
#include <QWidget>
#include <QScreen>
#include <qtoolbar.h>
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QImage>
#include <qtimer.h>
#include <qgroupbox.h>
#include <qgridlayout.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <chrono>

#include "pco.camera/camera.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class ImageViewer : public QMainWindow
{
  Q_OBJECT

public:
  ImageViewer(QWidget* parent = nullptr);
  ~ImageViewer();

  pco::Camera* cam;

  pco::Configuration m_initialConfig;

  pco::Image m_picture_raw;

  pco::ConvertControlColor m_conv_col;
  pco::ConvertControlPseudoColor m_conv_pseudo_col;
  pco::ConvertControlMono m_conv_mono;

private slots:
  void zoomIn();
  void zoomOut();
  void normalSize();
  void oneImage();
  void startStream();
  void stopStream();
  void configChanged();
  void delayExposureTimeChanged();
  void resetCamConfig();
  void rescanForCamera();

private:
  void cameraInit();
  void createMenus();
  void createCameraSettings();
  void setImage();
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar* scrollBar, double factor);
  void stream();
  void recSequence();
  void showSequence();
  void setCamConfig();

  QImage m_image;
  QLabel* imageLabel;
  QScrollArea* scrollArea;
  double m_scaleFactor = 1;
  bool m_first_stream_picture;
  bool m_first_pic_global;
  bool m_recorded = false;
  bool m_menus_created = false;
  bool m_settings_created = false;
  bool m_is_colored;
  int m_image_counter;
  int m_image_amount_sequence;
  QTimer m_streamTimer;

  //Widgets for camera Settings
  QSpinBox* imageCountSeq;
  QDoubleSpinBox* exposureTime;
  QDoubleSpinBox* delayTime;
  QComboBox* timeStamp;
  QComboBox* pixelRate;
  QComboBox* triggerMode;
  QCheckBox* noiseFilter;
  QSpinBox* roiX0;
  QSpinBox* roiX1;
  QSpinBox* roiY0;
  QSpinBox* roiY1;
  QPushButton* resetCamConfigBtn;

  QMessageBox* error_message;

  //Actions for ViewMenu
  QAction* zoomInAct;
  QAction* zoomOutAct;
  QAction* normalSizeAct;
  QAction* streamAct;
  QAction* stopStreamAct;
  QAction* oneImageAct;
  QAction* recNewSequenceAct;
  QAction* showNextPictAct;
};

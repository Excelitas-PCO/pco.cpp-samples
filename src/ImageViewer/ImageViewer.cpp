#include <thread>
#include <chrono>

#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
  : QMainWindow(parent)
{
  cam = nullptr;
  try
  {
    cameraInit();
  }
  catch (pco::CameraException& err)
  {
    error_message = new QMessageBox(QMessageBox::NoIcon, tr("No Camera"), tr("No camera found, please make sure camera is connected and press 'Scan for Camera'"));
    error_message->show();
  }
  createCameraSettings();

  createMenus();

  m_menus_created = true;
  m_settings_created = true;

  m_first_pic_global = true;
  m_image_amount_sequence = 10;
  resize(QGuiApplication::primaryScreen()->availableSize() * 4 / 5);
}

ImageViewer::~ImageViewer()
{
  if(cam != nullptr)
  {
    delete cam;
    cam = nullptr;
  }
}

void ImageViewer::setImage()
{
  if (m_first_pic_global)
  {
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
    normalSizeAct->setEnabled(true);
    m_first_pic_global = false;
    scrollArea->setVisible(true);
  }

  imageLabel->setPixmap(QPixmap::fromImage(m_image));

  if (m_first_stream_picture)
  {
    imageLabel->adjustSize();
    scaleImage(1);
    m_first_stream_picture = false;
  }
}

void ImageViewer::zoomIn()

{
  scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
  scaleImage(0.8);
}

void ImageViewer::normalSize()

{
  imageLabel->adjustSize();
  m_scaleFactor = 1.0;
}

void ImageViewer::createMenus()
{
  if (m_menus_created)
  {
    if (cam == nullptr)
    {
      zoomInAct->setDisabled(true);
      zoomOutAct->setDisabled(true);
      normalSizeAct->setDisabled(true);
      streamAct->setDisabled(true);
      stopStreamAct->setDisabled(true);
      oneImageAct->setDisabled(true);
      recNewSequenceAct->setDisabled(true);
      showNextPictAct->setDisabled(true);
    }
    if (cam != nullptr)
    {
      zoomInAct->setEnabled(true);
      zoomOutAct->setEnabled(true);
      normalSizeAct->setEnabled(true);
      streamAct->setEnabled(true);
      stopStreamAct->setEnabled(true);
      oneImageAct->setEnabled(true);
      recNewSequenceAct->setEnabled(true);
      showNextPictAct->setEnabled(true);
    }
    return;
  }
  //MenuBar config
  QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

  zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
  zoomInAct->setShortcut(QKeySequence::ZoomIn);
  zoomInAct->setEnabled(false);

  zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
  zoomOutAct->setShortcut(QKeySequence::ZoomOut);
  zoomOutAct->setEnabled(false);

  normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
  normalSizeAct->setShortcut(tr("Ctrl+S"));
  normalSizeAct->setEnabled(false);

  QMenu* recCtrlMenu = menuBar()->addMenu(tr("Record Control"));

  streamAct = recCtrlMenu->addAction(QIcon(":/res/play.png"), tr("Play"), this, &ImageViewer::startStream);
  stopStreamAct = recCtrlMenu->addAction(QIcon(":/res/stop.png"), tr("Stop"), this, &ImageViewer::stopStream);
  oneImageAct = recCtrlMenu->addAction(QIcon(":/res/snap.png"), tr("Single Image"), this, &ImageViewer::oneImage);
  recNewSequenceAct = recCtrlMenu->addAction(QIcon(":/res/record.png"), tr("Record new Sequence"), this, &ImageViewer::recSequence);
  showNextPictAct = recCtrlMenu->addAction(QIcon(":/res/next.png"), tr("Show next Picture"), this, &ImageViewer::showSequence);

  QMenu* cameraMenu = menuBar()->addMenu(tr("Camera"));

  QAction* rescanAct = cameraMenu->addAction(tr("Scan for Camera"), this, &ImageViewer::rescanForCamera);

  //ToolBar config

  QToolBar* toolbar = addToolBar(tr("Control"));
  toolbar->addAction(streamAct);
  toolbar->addAction(stopStreamAct);
  toolbar->addSeparator();
  toolbar->addAction(oneImageAct);
  toolbar->addSeparator();
  toolbar->addAction(recNewSequenceAct);
  toolbar->addAction(showNextPictAct);

  connect(&m_streamTimer, &QTimer::timeout, this, &ImageViewer::stream);

  if (cam == nullptr)
  {
    zoomInAct->setDisabled(true);
    zoomOutAct->setDisabled(true);
    normalSizeAct->setDisabled(true);
    streamAct->setDisabled(true);
    stopStreamAct->setDisabled(true);
    oneImageAct->setDisabled(true);
    recNewSequenceAct->setDisabled(true);
    showNextPictAct->setDisabled(true);
  }
}

void ImageViewer::createCameraSettings()
{
  //ScrollArea Configf
  imageLabel = new QLabel(this);
  imageLabel->setBackgroundRole(QPalette::NoRole);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);

  scrollArea = new QScrollArea(this);
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(imageLabel);
  scrollArea->setVisible(true);

  //Complete InputField Configuration
  QGroupBox* inputField;
  QVBoxLayout* inputFieldLayout;
  inputField = new QGroupBox(tr("Camera Settings"), this);
  inputField->setMaximumWidth(200);
  inputField->setMinimumWidth(150);
  inputFieldLayout = new QVBoxLayout();

  //sequence_image_count config
  QGroupBox* imageCountSeqBox;
  QVBoxLayout* imageCountSeqLayout;
  imageCountSeq = new QSpinBox(this);
  imageCountSeq->setMinimum(1);
  imageCountSeq->setMaximum(100000);
  imageCountSeq->setValue(10);
  imageCountSeqLayout = new QVBoxLayout();
  imageCountSeqLayout->addWidget(imageCountSeq);
  imageCountSeqBox = new QGroupBox(tr("Images per Sequence:"), this);
  imageCountSeqBox->setLayout(imageCountSeqLayout);
  imageCountSeqBox->setMaximumHeight(60);

  //ExposureTimeLayout Config
  QGroupBox* exposureTimeBox;
  QVBoxLayout* expTimeLayout;
  exposureTime = new QDoubleSpinBox(this);
  exposureTime->setDecimals(5);
  if (cam != nullptr)
  {
    exposureTime->setRange(0, cam->getDescription().max_exposure_time_s);
    exposureTime->setValue(cam->getConfiguration().exposure_time_s);
  }
  exposureTime->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
  expTimeLayout = new QVBoxLayout();
  expTimeLayout->addWidget(exposureTime);
  exposureTimeBox = new QGroupBox(tr("Exposure time in s:"), this);
  exposureTimeBox->setLayout(expTimeLayout);
  exposureTimeBox->setMaximumHeight(60);

  //DelayTimeLayout Config
  QGroupBox* delayTimeBox;
  QVBoxLayout* delayTimeLayout;
  delayTime = new QDoubleSpinBox(this);
  delayTime->setDecimals(5);
  if (cam != nullptr)
  {
    delayTime->setRange(0, cam->getDescription().max_delay_time_s);
    delayTime->setValue(cam->getConfiguration().delay_time_s);
  }
  delayTime->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
  delayTimeLayout = new QVBoxLayout();
  delayTimeLayout->addWidget(delayTime);
  delayTimeBox = new QGroupBox(tr("Delay time in s:"), this);
  delayTimeBox->setLayout(delayTimeLayout);
  delayTimeBox->setMaximumHeight(60);

  //TimeStampLayout config
  bool timestamp = false;
  bool has_ascii_only = false;
  if (cam != nullptr)
  {
    timestamp = cam->getDescription().has_timestamp_mode;
    has_ascii_only = cam->getDescription().has_timestamp_mode_ascii_only;
  }
  QGroupBox* timeStampBox;
  QVBoxLayout* timeStampLayout;
  timeStamp = new QComboBox(this);
  timeStamp->addItem(tr("OFF"));
  if (timestamp)
  {
    timeStamp->addItem(tr("Binary"));
    timeStamp->addItem(tr("ASCII + Binary"));
  }
  if (has_ascii_only)
  {
    timeStamp->addItem(tr("ASCII"));
  }
  timeStampLayout = new QVBoxLayout();
  timeStampLayout->addWidget(timeStamp);
  timeStampBox = new QGroupBox(tr("Timestamp:"), this);
  timeStampBox->setLayout(timeStampLayout);
  timeStampBox->setMaximumHeight(60);

  //PixelRateLyout Config
  QGroupBox* pixelRateBox;
  QVBoxLayout* pixelRateLayout;
  pixelRate = new QComboBox(this);

  if (cam != nullptr)
  {
    for (DWORD& rate : cam->getDescription().pixelrate_vec)
      pixelRate->addItem(tr(std::to_string(rate).c_str()));
  }

  pixelRateLayout = new QVBoxLayout();
  pixelRateLayout->addWidget(pixelRate);
  pixelRateBox = new QGroupBox(tr("Pixelrate:"), this);
  pixelRateBox->setLayout(pixelRateLayout);
  pixelRateBox->setMaximumHeight(60);

  //Triggermode Config
  QGroupBox* triggerModeBox;
  QVBoxLayout* triggerModeLayout;
  triggerMode = new QComboBox(this);
  triggerMode->addItem(tr("Auto Sequence"));
  triggerMode->addItem(tr("Soft Trigger"));
  triggerMode->addItem(tr("Ext. Start"));
  triggerMode->addItem(tr("Ext. Exp. Ctrl"));
  triggerModeLayout = new QVBoxLayout();
  triggerModeLayout->addWidget(triggerMode);
  triggerModeBox = new QGroupBox(tr("Triggermode:"), this);
  triggerModeBox->setLayout(triggerModeLayout);
  triggerModeBox->setMaximumHeight(60);

  //ROI Config
  QGroupBox* roiOuterBox;
  QGroupBox* roiInnerBox;
  QVBoxLayout* roiOuterLayout;
  QGridLayout* roiInnerLayout;
  QLabel* roiOuterLabel;
  QLabel* roiX0Label;
  QLabel* roiX1Label;
  QLabel* roiY0Label;
  QLabel* roiY1Label;
  roiX0Label = new QLabel(tr("X0"));
  roiX1Label = new QLabel(tr("X1"));
  roiY0Label = new QLabel(tr("Y0"));
  roiY1Label = new QLabel(tr("Y1"));
  roiX0 = new QSpinBox(this);
  roiX1 = new QSpinBox(this);
  roiY0 = new QSpinBox(this);
  roiY1 = new QSpinBox(this);

  if (cam != nullptr)
  {
    bool roi_symmetric_horz = cam->getDescription().roi_symmetric_horz;
    bool roi_symmetric_vert = cam->getDescription().roi_symmetric_vert;

    roiX0->setMinimum(1);
    roiX0->setMaximum(cam->getDescription().max_width - cam->getDescription().roi_step_horz);
    roiX0->setValue(m_initialConfig.roi.x0);
    roiX0->setSingleStep(cam->getDescription().roi_step_horz);

    roiX1->setMaximum(cam->getDescription().max_width);
    roiX1->setValue(m_initialConfig.roi.x1);
    roiX1->setSingleStep(cam->getDescription().roi_step_horz);

    if (roi_symmetric_horz)
    {
      roiX0->setMaximum(cam->getDescription().max_width / 2);
      roiX1->setMinimum(cam->getDescription().max_width / 2 + cam->getDescription().roi_step_horz);
    }

    roiY0->setMinimum(1);
    roiY0->setMaximum(cam->getDescription().max_height - cam->getDescription().roi_step_vert);
    roiY0->setValue(m_initialConfig.roi.y0);
    roiY0->setSingleStep(cam->getDescription().roi_step_vert);

    roiY1->setMaximum(cam->getDescription().max_height);
    roiY1->setValue(m_initialConfig.roi.y1);
    roiY1->setSingleStep(cam->getDescription().roi_step_vert);

    if (roi_symmetric_vert)
    {
      roiY0->setMaximum(cam->getDescription().max_height / 2);
      roiY1->setMinimum(cam->getDescription().max_height / 2 + cam->getDescription().roi_step_vert);
    }
  }
  roiInnerLayout = new QGridLayout();
  roiInnerLayout->addWidget(roiX0Label, 0, 0);
  roiInnerLayout->addWidget(roiX0, 0, 1);
  roiInnerLayout->addWidget(roiX1Label, 1, 0);
  roiInnerLayout->addWidget(roiX1, 1, 1);
  roiInnerLayout->addWidget(roiY0Label, 0, 2);
  roiInnerLayout->addWidget(roiY0, 0, 3);
  roiInnerLayout->addWidget(roiY1Label, 1, 2);
  roiInnerLayout->addWidget(roiY1, 1, 3);

  roiInnerBox = new QGroupBox(this);
  roiInnerBox->setLayout(roiInnerLayout);

  roiOuterLabel = new QLabel(tr("ROI:"));
  roiOuterLayout = new QVBoxLayout();
  roiOuterLayout->addWidget(roiOuterLabel);
  roiOuterLayout->addWidget(roiInnerBox);

  roiOuterBox = new QGroupBox(this);
  roiOuterBox->setLayout(roiOuterLayout);
  roiOuterBox->setMaximumHeight(120);
  if (cam != nullptr)
  {
    if (cam->getDescription().roi_step_horz == 0)
    {
      roiX0->setDisabled(true);
      roiX1->setDisabled(true);
    }
    if (cam->getDescription().roi_step_vert == 0)
    {
      roiY0->setDisabled(true);
      roiY1->setDisabled(true);
    }
  }

  //NoiseFilter Config
  QGroupBox* noiseFilterBox;
  QVBoxLayout* noiseFilterLayout;
  noiseFilter = new QCheckBox(tr("Noisefilter"), this);
  noiseFilterLayout = new QVBoxLayout();
  noiseFilterLayout->addWidget(noiseFilter);
  noiseFilterBox = new QGroupBox(this);
  noiseFilterBox->setLayout(noiseFilterLayout);
  noiseFilterBox->setMaximumHeight(40);

  if (cam != nullptr)
  {
    if (cam->getConfiguration().noise_filter_mode == UNDEF_W)
    {
      noiseFilter->setDisabled(true);
    }
    else
    {
      noiseFilter->setChecked(cam->getConfiguration().noise_filter_mode);
    }
  }
  //ResetCam config
  resetCamConfigBtn = new QPushButton(this);
  resetCamConfigBtn->setText(tr("Reset camerasettings"));

  //adding all Boxes to InputFieldLayout
  inputFieldLayout->addWidget(resetCamConfigBtn);
  inputFieldLayout->addWidget(imageCountSeqBox);
  inputFieldLayout->addWidget(exposureTimeBox);
  inputFieldLayout->addWidget(delayTimeBox);
  inputFieldLayout->addWidget(timeStampBox);
  inputFieldLayout->addWidget(pixelRateBox);
  inputFieldLayout->addWidget(triggerModeBox);
  inputFieldLayout->addWidget(noiseFilterBox);
  inputFieldLayout->addWidget(roiOuterBox);
  inputFieldLayout->addStretch();

  //setting complete Layout for InputField-Box
  inputField->setLayout(inputFieldLayout);

  //setting up MainWidget with scrollarea and Inputfield
  QWidget* mainWidget = new QWidget(this);
  QHBoxLayout* mainLayout = new QHBoxLayout;
  mainLayout->addWidget(scrollArea, Qt::AlignLeft);
  mainLayout->addWidget(inputField, Qt::AlignRight);
  mainWidget->setLayout(mainLayout);
  mainWidget->setMinimumHeight(650);
  setCentralWidget(mainWidget);

  //Connects to read out Configuration
  connect(imageCountSeq, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImageViewer::configChanged);
  connect(exposureTime, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ImageViewer::delayExposureTimeChanged);
  connect(delayTime, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ImageViewer::delayExposureTimeChanged);
  connect(timeStamp, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &ImageViewer::configChanged);
  connect(pixelRate, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &ImageViewer::configChanged);
  connect(triggerMode, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &ImageViewer::configChanged);

  connect(noiseFilter, &QCheckBox::stateChanged, this, &ImageViewer::configChanged);

  connect(roiX0, &QSpinBox::editingFinished, this, &ImageViewer::configChanged);
  connect(roiX1, &QSpinBox::editingFinished, this, &ImageViewer::configChanged);
  connect(roiY0, &QSpinBox::editingFinished, this, &ImageViewer::configChanged);
  connect(roiY1, &QSpinBox::editingFinished, this, &ImageViewer::configChanged);

  connect(resetCamConfigBtn, &QPushButton::clicked, this, &ImageViewer::resetCamConfig);

  if (cam == nullptr)
  {
    imageCountSeq->setDisabled(true);
    exposureTime->setDisabled(true);
    delayTime->setDisabled(true);
    timeStamp->setDisabled(true);
    pixelRate->setDisabled(true);
    triggerMode->setDisabled(true);
    noiseFilter->setDisabled(true);
    roiX0->setDisabled(true);
    roiX1->setDisabled(true);
    roiY0->setDisabled(true);
    roiY1->setDisabled(true);
    resetCamConfigBtn->setDisabled(true);
  }
}

void ImageViewer::scaleImage(double factor)
{
  m_scaleFactor *= factor;
  imageLabel->resize(m_scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size());

  adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(scrollArea->verticalScrollBar(), factor);

  zoomInAct->setEnabled(m_scaleFactor < 8.0);
  zoomOutAct->setEnabled(m_scaleFactor > 0.2);
}

void ImageViewer::adjustScrollBar(QScrollBar* scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value()
    + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void ImageViewer::oneImage()
{
  try
  {
    if (cam->isRecording()) { cam->stop(); }
    if (m_streamTimer.isActive()) { stopStream(); }
    m_recorded = false;
    cam->record(1, pco::RecordMode::sequence_non_blocking);

    if (cam->getConfiguration().trigger_mode != 0)
    {
      int i = 0;
      int count = cam->getRecordedImageCount();
      while (i < 100 && count == 0)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i += 1;
        count = cam->getRecordedImageCount();
      }
    }
    else
    {
      cam->waitForFirstImage(true, 10);
    }

    if (m_is_colored)
    {
      cam->image(m_picture_raw, PCO_RECORDER_LATEST_IMAGE, pco::DataFormat::BGR8);
      const std::vector<BYTE>& pictures = m_picture_raw.vector_8bit();
      m_image = QImage((uchar*)pictures.data(), m_picture_raw.width(), m_picture_raw.height(), QImage::Format_BGR888);
    }
    else
    {
      cam->image(m_picture_raw, PCO_RECORDER_LATEST_IMAGE, pco::DataFormat::Mono8);

      const std::vector<BYTE>& pictures = m_picture_raw.vector_8bit();

      m_image = QImage((uchar*)pictures.data(), m_picture_raw.width(), m_picture_raw.height(), QImage::Format_Grayscale8);
    }

    m_first_stream_picture = true;
    setImage();
    cam->stop();
  }
  catch (pco::CameraException& err)
  {
    stopStream();
    error_message = new QMessageBox(QMessageBox::NoIcon, tr("Error"), tr("No Image recorded\nMay happen if you are not on auto trigger mode and didn't provide any triggersignal"));
    error_message->show();
  }
}

void ImageViewer::startStream()
{
  if (!m_streamTimer.isActive())
  {
    m_recorded = false;
    cam->record(10, pco::RecordMode::ring_buffer);

    if (cam->getConfiguration().trigger_mode != 0)
    {
      int i = 0;
      int count = cam->getRecordedImageCount();
      while (i < 100 && count == 0)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i += 1;
        count = cam->getRecordedImageCount();
      }
    }
    else
    {
      cam->waitForFirstImage(true, 10);
    }
    m_streamTimer.start(34);
    m_first_stream_picture = true;
  }
}

void ImageViewer::stream()
{
  try
  {
    if (m_is_colored)
    {
      cam->image(m_picture_raw, PCO_RECORDER_LATEST_IMAGE, pco::DataFormat::BGR8);

      const std::vector<BYTE>& pictures = m_picture_raw.vector_8bit();

      m_image = QImage((uchar*)pictures.data(), m_picture_raw.width(), m_picture_raw.height(), QImage::Format_BGR888);
    }
    else
    {
      cam->image(m_picture_raw, PCO_RECORDER_LATEST_IMAGE, pco::DataFormat::Mono8); //If Dimax shows dimm Picture-> LSB Alignment Problem then switch this all to Mono8 + image->mirror(false,true)

      const std::vector<BYTE>& pictures = m_picture_raw.vector_8bit();

      m_image = QImage((uchar*)pictures.data(), m_picture_raw.width(), m_picture_raw.height(), QImage::Format_Grayscale8);
    }

    setImage();
  }
  catch (pco::CameraException& err)
  {
    stopStream();
    error_message = new QMessageBox(QMessageBox::NoIcon, tr("Error"), tr("No Image recorded\nMay happen if you are not on auto trigger mode and didn't provide any triggersignal "));
    error_message->show();
  }
}

void ImageViewer::stopStream()
{
  m_streamTimer.stop();
  cam->stop();
}

void ImageViewer::recSequence()
{
  try
  {
    if (m_streamTimer.isActive()) { stopStream(); }
    if (m_image_amount_sequence == 0) { m_image_amount_sequence = 1; }
    cam->record(m_image_amount_sequence, pco::RecordMode::sequence_non_blocking);
    if (cam->getConfiguration().trigger_mode != 0)
    {
      int i = 0;
      int count = cam->getRecordedImageCount();
      while (i < 100 && count == 0)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        i += 1;
        count = cam->getRecordedImageCount();
      }
    }
    else
    {
      cam->waitForFirstImage(true, 10);
    }
    m_image_counter = 0;
    m_recorded = true;
    showSequence();
  }
  catch (pco::CameraException& err)
  {
    stopStream();
    error_message = new QMessageBox(QMessageBox::NoIcon, tr("Error"), tr("No Image recorded \nMay happen if you are not on auto trigger mode and didn't provide any triggersignal"));
    error_message->show();
  }
}

void ImageViewer::showSequence()
{
  if (m_streamTimer.isActive()) { return; }
  if (m_image_counter >= cam->getRecordedImageCount()) { m_image_counter = 0; }
  if (m_recorded)
  {
    if (m_is_colored)
    {
      cam->image(m_picture_raw, m_image_counter, pco::DataFormat::BGR8);
      std::vector<BYTE>& one_picture = m_picture_raw.vector_8bit();
      m_image = QImage((uchar*)one_picture.data(), m_picture_raw.width(), m_picture_raw.height(), QImage::Format_BGR888);
    }
    else
    {
      cam->image(m_picture_raw, m_image_counter, pco::DataFormat::Mono8);
      std::vector<BYTE>& one_picture = m_picture_raw.vector_8bit();
      m_image = QImage((uchar*)one_picture.data(), m_picture_raw.width(), m_picture_raw.height(), QImage::Format_Grayscale8);
    }
    m_first_stream_picture = true;
    setImage();
    m_image_counter++;
  }
}

void ImageViewer::cameraInit()
{
  cam = new pco::Camera();

  cam->setExposureTime(0.01);
  cam->setDelayTime(0);

  m_picture_raw.resize(cam->getConfiguration().roi, pco::DataFormat::Mono8, cam->getRawFormat());
  m_initialConfig = cam->getConfiguration();
  m_is_colored = cam->isColored();

  if (cam->isColored())
  {
    m_conv_col = std::get<pco::ConvertControlColor>(cam->getConvertControl(pco::DataFormat::BGR8));
    m_conv_col.flip_vertical = false;
    cam->setConvertControl(pco::DataFormat::BGR8, m_conv_col);
  }
  if(!cam->isColored())
  {
    m_conv_pseudo_col = std::get<pco::ConvertControlPseudoColor>(cam->getConvertControl(pco::DataFormat::BGR8));
    m_conv_pseudo_col.flip_vertical = false;
    cam->setConvertControl(pco::DataFormat::BGR8, m_conv_pseudo_col);
  }
  m_conv_mono = std::get<pco::ConvertControlMono>(cam->getConvertControl(pco::DataFormat::Mono8));
  m_conv_mono.flip_vertical = false;
  cam->setConvertControl(pco::DataFormat::Mono8, m_conv_mono);
}

void ImageViewer::setCamConfig()
{
  if (cam->isRecording() && !m_streamTimer.isActive()) { cam->stop(); }

  bool was_streaming = false;
  bool roi_symmetric_horz = cam->getDescription().roi_symmetric_horz;
  bool roi_symmetric_vert = cam->getDescription().roi_symmetric_vert;
  pco::Configuration config;
  config = cam->getConfiguration();
  int roi_x0 = 0;
  int roi_x1 = 0;
  int roi_y0 = 0;
  int roi_y1 = 0;

  if (m_streamTimer.isActive())
  {
    stopStream();
    was_streaming = true;
  }
  m_image_amount_sequence = imageCountSeq->value();
  int time_stamp = timeStamp->currentIndex();
  int pixel_rate = pixelRate->currentIndex();
  int trigger_mode = triggerMode->currentIndex();
  bool noise_filter = noiseFilter->isChecked();

  roiX0->blockSignals(true);
  roiX1->blockSignals(true);
  roiY0->blockSignals(true);
  roiY1->blockSignals(true);

  QObject* sender = QObject::sender();

  if (sender == roiX0)
  {
    roi_x0 = (((roiX0->value() - 1) / cam->getDescription().roi_step_horz) * cam->getDescription().roi_step_horz) + 1;
    config.roi.x0 = roi_x0;
    roiX0->setValue(roi_x0);
    if (roi_symmetric_horz)
    {
      int steps_right_x = roi_x0 / cam->getDescription().roi_step_horz;
      roi_x1 = cam->getDescription().max_width - (steps_right_x * cam->getDescription().roi_step_horz);
      config.roi.x1 = roi_x1;
      roiX1->setValue(roi_x1);
    }
  }

  if (sender == roiX1)
  {
    roi_x1 = ((roiX1->value() / cam->getDescription().roi_step_horz) * cam->getDescription().roi_step_horz);
    config.roi.x1 = roi_x1;
    roiX1->setValue(roi_x1);
    if (roi_symmetric_horz)
    {
      int steps_left_x = (cam->getDescription().max_width - roi_x1) / cam->getDescription().roi_step_horz;
      roi_x0 = steps_left_x * cam->getDescription().roi_step_horz + 1;
      config.roi.x0 = roi_x0;
      roiX0->setValue(roi_x0);
    }
  }

  if (sender == roiY0)
  {
    roi_y0 = (((roiY0->value() - 1) / cam->getDescription().roi_step_vert) * cam->getDescription().roi_step_vert) + 1;
    config.roi.y0 = roi_y0;
    roiY0->setValue(roi_y0);
    if (roi_symmetric_vert)
    {
      int steps_down_y = roi_y0 / cam->getDescription().roi_step_vert;
      roi_y1 = cam->getDescription().max_height - (steps_down_y * cam->getDescription().roi_step_vert);
      config.roi.y1 = roi_y1;
      roiY1->setValue(roi_y1);
    }
  }

  if (sender == roiY1)
  {
    roi_y1 = ((roiY1->value() / cam->getDescription().roi_step_vert) * cam->getDescription().roi_step_vert);
    config.roi.y1 = roi_y1;
    roiY1->setValue(roi_y1);
    if (roi_symmetric_vert)
    {
      int steps_up_y = (cam->getDescription().max_height - roi_y1) / cam->getDescription().roi_step_vert;
      roi_y0 = steps_up_y * cam->getDescription().roi_step_vert + 1;
      config.roi.y0 = roi_y0;
      roiX0->setValue(roi_y0);
    }
  }
  //Check for valid sizes
  if (!roi_symmetric_horz && !roi_symmetric_vert)
  {
    if (roiX1->value() < roiX0->value())
    {
      roi_x1 = roiX0->value() + cam->getDescription().roi_step_horz - 1;
      config.roi.x1 = roi_x1;
      roiX1->setValue(roi_x1);
    }
    if (roiY1->value() < roiY0->value())
    {
      roi_y1 = roiY0->value() + cam->getDescription().roi_step_vert - 1;
      config.roi.y1 = roi_y1;
      roiY1->setValue(roi_y1);
    }
  }

  roiX0->blockSignals(false);
  roiX1->blockSignals(false);
  roiY0->blockSignals(false);
  roiY1->blockSignals(false);

  bool has_acquire_mode = cam->getDescription().has_acquire_mode;;
  bool has_metadata_mode = false;

  config.timestamp_mode = time_stamp;
  config.pixelrate = cam->getDescription().pixelrate_vec.at(pixel_rate);
  config.trigger_mode = trigger_mode;
  if (sender == noiseFilter) { config.noise_filter_mode = noise_filter; }
  if (has_acquire_mode) { config.acquire_mode = 0; }
  if (has_metadata_mode) { config.metadata_mode = 1; }
  cam->setConfiguration(config);
  if (was_streaming)
  {
    startStream();
    was_streaming = false;
  }
}

void ImageViewer::configChanged()
{
  setCamConfig();
}

void ImageViewer::delayExposureTimeChanged()
{
  double exposure_time = exposureTime->value();
  if (exposure_time == 0)
  {
    exposure_time = m_initialConfig.exposure_time_s;
    exposureTime->setValue(exposure_time);
  }
  double delay_time = delayTime->value();

  cam->setDelayTime(delay_time);
  cam->setExposureTime(exposure_time);
}

void ImageViewer::resetCamConfig()
{
  bool was_streaming = false;
  if (m_streamTimer.isActive())
  {
    stopStream();
    was_streaming = true;
  }

  cam->setConfiguration(m_initialConfig);

  exposureTime->setValue(m_initialConfig.exposure_time_s);
  delayTime->setValue(m_initialConfig.delay_time_s);
  roiX0->setValue(m_initialConfig.roi.x0);
  roiX1->setValue(m_initialConfig.roi.x1);
  roiY0->setValue(m_initialConfig.roi.y0);
  roiY1->setValue(m_initialConfig.roi.y1);
  timeStamp->setCurrentIndex(m_initialConfig.timestamp_mode);
  pixelRate->setCurrentIndex(0);
  triggerMode->setCurrentIndex(m_initialConfig.trigger_mode);
  noiseFilter->setChecked(m_initialConfig.noise_filter_mode);

  if (was_streaming)
  {
    startStream();
    was_streaming = false;
  }
}

void ImageViewer::rescanForCamera()
{
  if (m_streamTimer.isActive()) { stopStream(); }
  if (cam != nullptr)
  {
    cam->~Camera();
  }
  try
  {
    cameraInit();
  }
  catch (pco::CameraException& err)
  {
    error_message = new QMessageBox(QMessageBox::NoIcon, tr("No Camera"), tr("No camera found, please make sure camera is connected and press 'Scan for Camera'"));
    error_message->show();
  }

  createMenus();
  createCameraSettings();
}

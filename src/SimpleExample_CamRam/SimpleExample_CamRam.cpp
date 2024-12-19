// SimpleExample.cpp
//
//#pragma once

#include <stdio.h>
#include <string.h>
#include "pco.camera/stdafx.h"
#include "pco.camera/camera.h"
#include "pco.camera/cameraexception.h"


int main()
{
  using namespace pco;
  try
  {
    int err = PCO_InitializeLib();
    if (err)
    {
      throw pco::CameraException(err);
    }
    Camera camera;
    camera.setCamRamAllocation({ 50, 20, 5 });
    camera.switchToCamRam(3);
    Configuration config = camera.getConfiguration();
    config.timestamp_mode = TIMESTAMP_MODE_BINARYANDASCII;
    DWORD valid_images = camera.getCamRamNumImages();
    DWORD max_images = camera.getCamRamMaxImages();
    WORD segment = camera.getCamRamSegment();
    Image image_latest(config.roi, DataFormat::Mono16, camera.getRawFormat());
    Image image_end(config.roi, DataFormat::Mono16, camera.getRawFormat());



    std::cout << "CamRam segment " << segment << ": " << valid_images << " / " << max_images << "\n";

    // segment
    std::cout << "Record with CamRam segment " << segment << " as ring buffer ... \n";

    camera.record(camera.getCamRamMaxImages(), RecordMode::camram_ring);

    camera.waitForFirstImage();
    int i = 0;
    auto starttime = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - starttime < std::chrono::seconds(10))
    {
      camera.image(image_latest, PCO_RECORDER_LATEST_IMAGE);
      std::cout << "image " << i++ << ": " << image_latest.getRecorderImageNumber() << ", bcd: " << bcd_counter(image_latest.meta_data->bIMAGE_COUNTER_BCD) << "\n";
      camera.waitForNewImage();
    }

    camera.stop();
    camera.image(image_latest, 0);
    camera.image(image_end, camera.getCamRamNumImages() - 1);
    std::cout << "CamRam ring buffer holds images from " << bcd_counter(image_latest.meta_data->bIMAGE_COUNTER_BCD) << " to " << bcd_counter(image_end.meta_data->bIMAGE_COUNTER_BCD) << "\n";
    valid_images = camera.getCamRamNumImages();
    max_images = camera.getCamRamMaxImages();
    segment = camera.getCamRamSegment();

    std::cout << "CamRam segment " << segment << ": " << valid_images << " / " << max_images << "\n";

    // segment
    std::cout << "Record till CamRam segment " << segment << " is full ... \n";
    camera.record(camera.getCamRamMaxImages(), RecordMode::camram_segment);

    camera.waitForFirstImage();
    i = 0;
    while (camera.isRecording())
    {
      camera.image(image_latest, PCO_RECORDER_LATEST_IMAGE);
      std::cout << "image " << i++ << ": " << image_latest.getRecorderImageNumber() << ", bcd: " << bcd_counter(image_latest.meta_data->bIMAGE_COUNTER_BCD) << "\n";
      camera.waitForNewImage();
    }

    camera.image(image_latest, 0);
    camera.image(image_end, camera.getCamRamNumImages() - 1);
    std::cout << "CamRam ring buffer holds images from " << bcd_counter(image_latest.meta_data->bIMAGE_COUNTER_BCD) << " to " << bcd_counter(image_end.meta_data->bIMAGE_COUNTER_BCD) << "\n";
    valid_images = camera.getCamRamNumImages();
    max_images = camera.getCamRamMaxImages();
    segment = camera.getCamRamSegment();

    std::cout << "CamRam segment " << segment << ": " << valid_images << " / " << max_images << "\n";


  }
  catch (CameraException& ex)
  {
    std::cout << ex.what() << std::endl;
    std::cout << "0x" << std::hex << ex.error_code() << std::dec << std::endl;
    std::cout << ex.error_text() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown Exception caught." << std::endl;
  }
  PCO_CleanupLib();

  return 0;
}

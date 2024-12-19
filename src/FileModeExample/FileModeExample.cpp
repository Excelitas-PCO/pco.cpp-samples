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
  try
  {
    int err = PCO_InitializeLib();
    if (err)
    {
      throw pco::CameraException(err);
    }
    pco::Camera cam;
    pco::Image img;
    int image_count = 10;
    cam.defaultConfiguration();

    cam.setExposureTime(0.01);

    std::cout << "Enter Filepath where Images should go (default: .):" << std::endl;
    std::string path;
    std::getline(std::cin, path);
    if (path.empty())
      path = ".";

    size_t i = 0;
    cam.record(image_count, pco::RecordMode::tif, std::filesystem::absolute(path));
    while (cam.isRecording())
    {
      if (i != cam.getRecordedImageCount())
        i = cam.getRecordedImageCount();
      else
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(pco::WAIT_TIME_MS));
        continue;
      }
      std::cout << "Image Count:" << i << " > " << std::filesystem::absolute(path) <<  std::endl;
    }

  }
  catch (pco::CameraException& err)
  {
    std::cout << "Error Code: " << err.error_code() << std::endl;
    std::cout << err.what() << std::endl;
  }
  PCO_CleanupLib();

  return 0;
}

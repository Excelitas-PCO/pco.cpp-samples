// ColorConvertExample.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
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
    int width, height;
    int image_count = 10;
    cam.defaultConfiguration();
    cam.setExposureTime(0.01);

    std::cout << "Enter Filepath where Images should go (default: .):" << std::endl;
    std::string path;
    std::getline(std::cin, path);
    if (path.empty())
      path = ".";

    if (!cam.isColored())
    {
      std::string lut_file;
      std::cout << "Enter filepath to LUT file \n(default: /opt/pco/pco.cpp/samples/ColorConvertExample/lut/LUT_rainbow.lt1):" << std::endl;
      std::getline(std::cin, lut_file);
      if (lut_file.empty())
        lut_file = "/opt/pco/pco.cpp/samples/ColorConvertExample/lut/LUT_rainbow.lt1";
      if (!std::filesystem::exists(lut_file))
        throw pco::CameraException("path to LUT file is invalid."); 

      pco::ConvertControlPseudoColor cc = std::get<pco::ConvertControlPseudoColor>(cam.getConvertControl(pco::DataFormat::BGR8));
      cc.lut_file = lut_file;
      cam.setConvertControl(pco::DataFormat::BGR8, cc);
    }

    cam.record(image_count, pco::RecordMode::sequence);
    cam.waitForFirstImage();
    for (int i = 0; i < image_count; i++)
    {
      cam.image(img, i, pco::DataFormat::BGR8);
      width = img.width();
      height = img.height();
      std::filesystem::path name = std::filesystem::absolute(path).append("ColorConvert_img_" + std::to_string(i + 1) + ".tif");
      std::cout << "Image Count:" << i + 1 << " > " << name << std::endl;
      int err = PCO_RecorderSaveImage(img.data().first, width, height, FILESAVE_IMAGE_BGR_8, true, name.string().c_str(), true, img.getMetaDataPtr());
      if (err)
      {
        throw pco::CameraException(err);
      }
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

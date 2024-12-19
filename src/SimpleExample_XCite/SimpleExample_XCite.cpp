// SimpleExample.cpp
//
// #pragma once

#include <stdio.h>
#include <string.h>

#include "pco.camera/stdafx.h"
#include "pco.camera/camera.h"
#include "pco.camera/xcite.h"
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

    etc::XCite x = etc::XCite();

    etc::XCITE_Description desc = x.getDescription();

    std::vector<DWORD> independent_wavelength_vec;

    std::cout << "serial: " << desc.serial << ", type: " << desc.name << ", wavelenghts: \n[";

    for (auto &w : desc.wavelenghts_vec)
      std::cout << w << ", ";
    std::cout << "]\nexclusivity: \n[";

    for (auto &w : desc.exclusivity_vec)
      std::cout << w << ", ";
    std::cout << "]\n";

    etc::XCITE_Configuration conf = x.getConfiguration();

    std::cout << "getConfiguration:\n";
    std::cout << "------------------------\n";
    std::cout << "intensities: \n[";

    for (auto &w : conf.intensities)
      std::cout << w << ", ";
    std::cout << "]\nstates: \n[";
    for (bool s : conf.on_states)
      std::cout << s << ", ";
    std::cout << "]\n";
    std::cout << "------------------------\n";

    conf.intensities[0] = 60;
    conf.on_states[0] = true;

    x.setConfiguration(conf);


    conf = x.getConfiguration();

    std::cout << "getConfiguration:\n";
    std::cout << "------------------------\n";
    std::cout << "intensities: \n[";

    for (auto &w : conf.intensities)
      std::cout << w << ", ";
    std::cout << "]\nstates: \n[";
    for (bool s : conf.on_states)
      std::cout << s << ", ";
    std::cout << "]\n";
    std::cout << "------------------------\n";

    x.switchOn();

    cam.record(image_count, pco::RecordMode::sequence);

    for (int i = 0; i < image_count; i++)
    {
      if (cam.isColored())
      {
        cam.image(img, i, pco::DataFormat::BGR8);
      }
      else
      {
        cam.image(img, i, pco::DataFormat::Mono8);
      }
      width = img.width();
      height = img.height();
      std::cout << "Image Count:" << i + 1 << std::endl;
    }

    x.defaultConfiguration();
  }
  catch (etc::XCiteException &err)
  {
    std::cout << "etc::XCiteException - Error Code: " << err.error_code() << std::endl;
    std::cout << err.what() << std::endl;
  }
  catch (pco::CameraException &err)
  {
    std::cout << "pco::CameraException - Error Code: " << err.error_code() << std::endl;
    std::cout << err.what() << std::endl;
  }
  PCO_CleanupLib();

  return 0;
}

#include <string>
#include <iostream>
#include <filesystem>
#include "pco.camera/camera.h"

int main()
{
  try
  {
    int err = PCO_InitializeLib();
    if (err)
    {
      throw pco::CameraException(err);
    }
    pco::Camera cam1, cam2;
    pco::Image img_cam1, img_cam2;
    int width_1, width_2, height_1, height_2;
    int image_count = 10;
    cam1.defaultConfiguration();
    cam1.setExposureTime(0.01);
    cam2.defaultConfiguration();
    cam2.setExposureTime(0.01);

    std::cout << "Enter Filepath where Images should go (default: .):" << std::endl;
    std::string path;
    std::getline(std::cin, path);
    if (path.empty())
      path = ".";

    cam1.record(image_count, pco::RecordMode::sequence);
    cam2.record(image_count, pco::RecordMode::sequence);

    std::cout << "Cam 1 recorded image count:" << cam1.getRecordedImageCount() << std::endl;
    std::cout << "Cam 2 recorded image count::" << cam2.getRecordedImageCount() << std::endl;

    for (int i = 0; i < image_count; i++)
    {
      cam1.image(img_cam1, i, pco::DataFormat::Mono16);
      cam2.image(img_cam2, i, pco::DataFormat::Mono16);

      width_1 = img_cam1.width();
      height_1 = img_cam1.height();
      width_2 = img_cam2.width();
      height_2 = img_cam2.height();

      std::filesystem::path name_cam1 = std::filesystem::absolute(path).append("img_cam1_nr" + std::to_string(i + 1) + ".tif");
      std::filesystem::path name_cam2 = std::filesystem::absolute(path).append("img_cam2_nr" + std::to_string(i + 1) + ".tif");

      std::cout << "Image Count:" << (i + 1) * 2 << "\n";

      int err = PCO_RecorderSaveImage(img_cam1.raw_data().first, width_1, height_1, FILESAVE_IMAGE_BW_16, false, name_cam1.string().c_str(), true, img_cam1.getMetaDataPtr());
      if (err)
      {
        throw pco::CameraException(err);
      }
      std::cout << " > " << name_cam1 << "\n";
      err = PCO_RecorderSaveImage(img_cam2.raw_data().first, width_2, height_2, FILESAVE_IMAGE_BW_16, false, name_cam2.string().c_str(), true, img_cam2.getMetaDataPtr());
      if (err)
      {
        throw pco::CameraException(err);
      }
      std::cout << " > " << name_cam2 << std::endl;
    }

  }
  catch (pco::CameraException& err)
  {

    std::cout << "Error Code:" << err.error_code() << std::endl;
    std::cout << err.what() << std::endl;
  }
  PCO_CleanupLib();

  return 0;
}

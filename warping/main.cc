#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "lib/warp.h"

using namespace std;
using namespace cv;
namespace fs=std::experimental::filesystem;

int main(int argc, char **argv) {
    fs::path calib_src{"/home/jiun/datasets/amano/raw/capital"};
    fs::path src{"/home/jiun/datasets/amano/calib/images"};
    fs::path dst{"/home/jiun/datasets/amano/calib/warp/out.jpg"};
    /*
    fs::path calib_src{"/home/jiun/workspace/amano/LPR-dump/test-src-calib"};
    fs::path src{"/home/jiun/workspace/amano/LPR-dump/test-src"};
    fs::path dst{"/home/jiun/workspace/amano/LPR-dump/test-dst/out.jpg"};
    */

    const Size output_size{1920, 1080};

    for (const auto & entry : fs::directory_iterator(src)) {
      string name{fs::path(entry).stem()};

      if (name.size() <= 0)
        continue;

      Mat image = imread(entry.path(), 1);

      Calibration calib(image.size());
      Warp warp(calib, output_size, 2);

      std::ifstream in(calib_src / entry.path().filename().string().substr(0, 6) / (entry.path().filename().string().substr(7, 2) + ".txt"));

      if (in.is_open()) {
        in >> calib.cx >> calib.cy >> calib.fx >> calib.fy;
        in >> warp.center[0] >> warp.center[1] >> warp.center[2];
        in >> warp.up[0] >> warp.up[1] >> warp.up[2];
        in >> warp.right[0] >> warp.right[1] >> warp.right[2];

        if (calib.fx > 1000) {
          calib.fx = 979.200012f;
        }
        if (calib.fy > 1000) {
          calib.fy = 979.200012f;
        }
      }
      in.close();

      Mat output;

      /*
      warp.Map(image, &output);
      imwrite(dst.replace_filename(name + "-1.jpg").string(), output);

      warp.Rotate(M_PI / 2.f );
      warp.Update();
      warp.Map(image, &output);
      imwrite(dst.replace_filename(name + "-2.jpg").string(), output);

      warp.plane_mode = 2;

      warp.Rotate(M_PI * 3 / 2.f );
      warp.Update();
      */
      warp.Update();
      warp.Map(image, &output);
      imwrite(dst.replace_filename(name + "+1.jpg").string(), output);

      /*
      warp.Rotate(M_PI / 2.f );
      warp.Update();
      warp.Map(image, &output);
      imwrite(dst.replace_filename(name + "+2.jpg").string(), output);
      */
    }

    return 0;
}

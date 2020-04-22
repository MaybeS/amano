#include <iostream>
#include <experimental/filesystem>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "lib/argparse.hpp"
#include "lib/libWarping.h"
#include "lib/pool.h"

using namespace std;
using namespace cv;
namespace fs=std::experimental::filesystem;

int main(int argc, char **argv) {
    std::ios::sync_with_stdio(false);

    argparse::ArgumentParser app("warp utility");

    function<fs::path(const std::string&)> to_path = [](const std::string& value) {
        return fs::path{value};
    };
    function<int(const std::string&)> to_int = [](const std::string& value) {
        return std::stoi(value);
    };

    app.add_argument("-s", "--source")
       .required().action(to_path).help("Source image path");
    app.add_argument("-d", "--dest")
       .required().action(to_path).help("Destination path");
    app.add_argument("-c", "--calib")
       .help("Calibration file").default_value("").action(to_path);
    app.add_argument("--worker")
       .help("Number of threads").default_value(4).action(to_int);
    app.add_argument("--size")
       .help("Size {width, height}").nargs(2).default_value(std::vector<int>{1920, 1080}).action(to_int);

    try {
        app.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << app;
        exit(0);
    }

    auto src = app.get<fs::path>("-s");
    auto dst = app.get<fs::path>("-d");
    auto calib_file = app.get<fs::path>("-c");
    auto worker = app.get<int>("--worker");
    auto size_ = app.get<std::vector<int>>("--size");
    Size output_size = Size(size_[0], size_[1]);

    fs::create_directory(dst);
    if (!fs::exists(calib_file)) {
        std::cout << "Cannot open " << calib_file << std::endl;
    }

    std::queue<std::future<void>> tasks;
    thread::Pool pool(worker);

    for (auto entry : fs::directory_iterator(src)) {
        string name{fs::path(entry).stem()};
        string path{entry.path().string()};

        if (name.empty())
            continue;

        tasks.emplace(pool.push([name, path, calib_file, output_size, dst](size_t id) {
            Mat image = imread(path, 1), output;

            Calibration calib(image.size());
            Warp warp(calib, output_size, 2);

            std::ifstream in(calib_file.string());
            if (in.is_open()) {
                in >> calib.cx >> calib.cy >> calib.fx >> calib.fy;
                in >> warp.center[0] >> warp.center[1] >> warp.center[2];
                in >> warp.up[0] >> warp.up[1] >> warp.up[2];
                in >> warp.right[0] >> warp.right[1] >> warp.right[2];
            }
            in.close();

            warp.Update();
            warp.Map(image, output);
            imwrite((dst / (name + "+1.jpg")).string(), output);

            image.release();
            output.release();
        }));
    }

    while(!tasks.empty()) {
        tasks.front().get();
        tasks.pop();
    }

    return 0;
}

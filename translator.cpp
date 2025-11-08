#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

void f_to_a(cv::Mat& frame, int w, const char* cset, std::ostream& os) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    int h = gray.rows;
    int width = gray.cols;
    float cw = (float)width / w;
    float ch = cw * 2;
    int rh = (int)(h / ch);

    cv::Mat res;
    cv::resize(gray, res, cv::Size(w, rh), 0, 0, cv::INTER_LINEAR);

    os << "";
    for (int r = 0; r < rh; r++) {
        for (int c = 0; c < w; c++) {
            unsigned char br = res.at<unsigned char>(r, c);
            int cidx = (int)(br / 256.0 * strlen(cset));
            os << cset[cidx];
        }
        if (r < rh - 1) {
            os << "\n";
        }
    }
    os << "";
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_video> <output_obj> [--width <w>]" << std::endl;
        return 1;
    }

    std::string vid_path = argv[1];
    std::string out_path = argv[2];
    int w = 120;

    for (int i = 3; i < argc; i++) {
        if (std::string(argv[i]) == "--width" && i + 1 < argc) {
            w = std::stoi(argv[i + 1]);
            break;
        }
    }

    cv::VideoCapture cap(vid_path);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file '" << vid_path << "'." << std::endl;
        return 1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int total = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);
    std::cout << "Processing video: " << vid_path << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Total frames: " << total << std::endl;
    std::cout << "ASCII width: " << w << " columns" << std::endl;

    std::ofstream f(out_path);
    if (!f.is_open()) {
        std::cerr << "Error: Could not open output file '" << out_path << "'." << std::endl;
        return 1;
    }

    const char* cset = " .:-=+*#%@";
    int processed = 0;
    bool first = true;

    cv::Mat frame;
    int rh = 0;

    if (cap.read(frame)) {
        float ch = ((float)frame.cols / w) * 2;
        rh = (int)(frame.rows / ch);
    }
    cap.set(cv::CAP_PROP_POS_FRAMES, 0);

    f << "{\n";
    f << "  \"fps\": " << fps << ",\n";
    f << "  \"width\": " << w << ",\n";
    f << "  \"height\": " << rh << ",\n";
    f << "  \"frames\": [\n    ";

    while (cap.read(frame)) {
        if (!first) {
            f << ",\n    ";
        }
        f_to_a(frame, w, cset, f);
        first = false;

        processed++;
        if (processed % 100 == 0) {
            std::cout << "Processed " << processed << "/" << total << " frames..." << std::endl;
        }
    }

    f << "\n  ]\n}\n";

    std::cout << "Processed all " << processed << " frames." << std::endl;
    std::cout << "ASCII video data saved to '" << out_path << "'" << std::endl;

    return 0;
}
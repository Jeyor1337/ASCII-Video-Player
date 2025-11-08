#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "ascii_decode.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_video> <output_obj> [--width <w>] [--charset <name>]" << std::endl;
        std::cerr << "Charset options: short, medium (default), long, blocks" << std::endl;
        return 1;
    }

    std::string vid_path = argv[1];
    std::string out_path = argv[2];
    int w = 120;
    std::string charset_name = "medium";

    for (int i = 3; i < argc; i++) {
        if (std::string(argv[i]) == "--width" && i + 1 < argc) {
            w = std::stoi(argv[i + 1]);
            i++;
        } else if (std::string(argv[i]) == "--charset" && i + 1 < argc) {
            charset_name = argv[i + 1];
            i++;
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

    const char* cset = CharSets::get_charset(charset_name);
    std::cout << "Character set: " << charset_name << " (" << strlen(cset) << " chars)" << std::endl;
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
        ASCIIDecode::frame_to_ascii_stream(frame, w, cset, f);
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
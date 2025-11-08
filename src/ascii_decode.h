#ifndef ASCII_DECODE_H
#define ASCII_DECODE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>

// Character set definitions
namespace CharSets {
    inline const char* get_charset(const std::string& name) {
        if (name == "short") {
            return " .:|";  // 4 characters - minimal detail
        } else if (name == "medium") {
            return " .:-=+*#%@";  // 10 characters - balanced (default)
        } else if (name == "long") {
            return " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";  // 70 characters - maximum detail
        } else if (name == "blocks") {
            return " ░▒▓█";  // 5 characters - block style
        } else {
            return " .:-=+*#%@";  // default to medium
        }
    }
}

// Convert a single frame to ASCII with color codes
namespace ASCIIDecode {
    // Convert frame to ASCII and write to output stream
    inline void frame_to_ascii_stream(cv::Mat& frame, int w, const char* cset, std::ostream& os) {
        // Keep the original BGR frame for color information
        int h = frame.rows;
        int width = frame.cols;
        float cw = (float)width / w;
        float ch = cw * 2;
        int rh = (int)(h / ch);

        // Resize the color frame
        cv::Mat res;
        cv::resize(frame, res, cv::Size(w, rh), 0, 0, cv::INTER_LINEAR);

        os << "\"";
        for (int r = 0; r < rh; r++) {
            for (int c = 0; c < w; c++) {
                cv::Vec3b pixel = res.at<cv::Vec3b>(r, c);
                unsigned char b = pixel[0];
                unsigned char g = pixel[1];
                unsigned char red = pixel[2];

                // Calculate brightness for character selection
                unsigned char br = (unsigned char)(0.299 * red + 0.587 * g + 0.114 * b);
                int cidx = (int)(br / 256.0 * strlen(cset));

                // Generate ANSI 24-bit truecolor escape code
                // Format: \033[38;2;R;G;Bm for foreground color
                // Write as \u001b (unicode escape) for valid JSON
                os << "\\u001b[38;2;" << (int)red << ";" << (int)g << ";" << (int)b << "m";

                // Escape special JSON characters
                char ch = cset[cidx];
                if (ch == '\\') {
                    os << "\\\\";  // Backslash must be escaped
                } else if (ch == '"') {
                    os << "\\\"";  // Quote must be escaped
                } else {
                    os << ch;
                }
            }
            // Reset color at end of line
            os << "\\u001b[0m";
            if (r < rh - 1) {
                os << "\\n";
            }
        }
        os << "\"";
    }

    // Convert frame to ASCII and print directly to stdout
    inline void frame_to_ascii_stdout(cv::Mat& frame, int w, const char* cset) {
        // Keep the original BGR frame for color information
        int h = frame.rows;
        int width = frame.cols;
        float cw = (float)width / w;
        float ch = cw * 2;
        int rh = (int)(h / ch);

        // Resize the color frame
        cv::Mat res;
        cv::resize(frame, res, cv::Size(w, rh), 0, 0, cv::INTER_LINEAR);

        for (int r = 0; r < rh; r++) {
            for (int c = 0; c < w; c++) {
                cv::Vec3b pixel = res.at<cv::Vec3b>(r, c);
                unsigned char b = pixel[0];
                unsigned char g = pixel[1];
                unsigned char red = pixel[2];

                // Calculate brightness for character selection
                unsigned char br = (unsigned char)(0.299 * red + 0.587 * g + 0.114 * b);
                int cidx = (int)(br / 256.0 * strlen(cset));

                // Generate ANSI 24-bit truecolor escape code
                printf("\033[38;2;%d;%d;%dm", (int)red, (int)g, (int)b);
                printf("%c", cset[cidx]);
            }
            // Reset color at end of line
            printf("\033[0m");
            if (r < rh - 1) {
                printf("\n");
            }
        }
        fflush(stdout);
    }
}

#endif // ASCII_DECODE_H

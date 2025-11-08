#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <json-c/json.h>
#include <opencv2/opencv.hpp>
#include "ascii_decode.h"

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    // Use ANSI escape code to clear screen and move cursor to top-left
    // This is faster than system("clear") and preserves color support
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

void hide_cursor() {
    printf("\033[?25l");
    fflush(stdout);
}

void show_cursor() {
    printf("\033[?25h");
    fflush(stdout);
}


void play_mp4_direct(const char *video_path, int width, const char* charset) {
    cv::VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file '" << video_path << "'." << std::endl;
        return;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int total = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);

    std::cout << "Playing video: " << video_path << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Total frames: " << total << std::endl;
    std::cout << "ASCII width: " << width << " columns" << std::endl;
    std::cout << "Character set: " << strlen(charset) << " chars" << std::endl;
    std::cout << "Starting playback in 2 seconds... Press Ctrl+C to stop." << std::endl;

    sleep(2);

    const char* cset = charset;
    long frame_delay = (long)(1000000.0 / fps);

    hide_cursor();

    cv::Mat frame;
    while (cap.read(frame)) {
        clear_screen();
        ASCIIDecode::frame_to_ascii_stdout(frame, width, cset);
        printf("\n");
        usleep(frame_delay);
    }

    clear_screen();
    show_cursor();
}

void play_obj_file(const char *ascii_art_file_path) {
    FILE *fp;
    char *buffer;
    long file_size;

    fp = fopen(ascii_art_file_path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: File not found at '%s'\n", ascii_art_file_path);
        return;
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate memory to read file.\n");
        fclose(fp);
        return;
    }

    fread(buffer, 1, file_size, fp);
    buffer[file_size] = '\0';

    fclose(fp);

    struct json_object *parsed_json;
    struct json_object *frames;
    struct json_object *fps_obj;
    double fps = 24.0;

    parsed_json = json_tokener_parse(buffer);
    free(buffer);
    if (parsed_json == NULL) {
        fprintf(stderr, "Error: '%s' is not a valid JSON file.\n", ascii_art_file_path);
        return;
    }

    if (!json_object_object_get_ex(parsed_json, "frames", &frames) || json_object_get_type(frames) != json_type_array) {
        fprintf(stderr, "Error: No valid 'frames' data found in the .obj file.\n");
        json_object_put(parsed_json);
        return;
    }

    if (json_object_object_get_ex(parsed_json, "fps", &fps_obj) && (json_object_get_type(fps_obj) == json_type_double || json_object_get_type(fps_obj) == json_type_int)) {
        fps = json_object_get_double(fps_obj);
        if (fps <= 0) {
            fprintf(stderr, "Warning: Invalid 'fps' data found. Defaulting to 24.\n");
            fps = 24.0;
        }
    } else {
        fprintf(stderr, "Warning: No valid 'fps' data found. Defaulting to 24.\n");
    }

    long frame_delay = (long)(1000000.0 / fps);

    printf("Starting playback... Press Ctrl+C to stop.\n");
    sleep(2);

    hide_cursor();

    int num_frames = json_object_array_length(frames);
    for (int i = 0; i < num_frames; i++) {
        struct json_object *frame_obj = json_object_array_get_idx(frames, i);
        const char *frame_str = json_object_get_string(frame_obj);
        clear_screen();
        printf("%s\n", frame_str);
        fflush(stdout);
        usleep(frame_delay);
    }

    json_object_put(parsed_json);
    clear_screen();
    show_cursor();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video_file> [--width <w>] [--charset <name>]\n", argv[0]);
        fprintf(stderr, "  Supports .mp4, .avi, .mkv (direct playback) and .obj (pre-converted)\n");
        fprintf(stderr, "  Charset options: short, medium (default), long, blocks\n");
        return 1;
    }

    const char* file_path = argv[1];
    int width = 80;  // Default width for direct playback
    std::string charset_name = "medium";

    // Parse arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            width = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--charset") == 0 && i + 1 < argc) {
            charset_name = argv[i + 1];
            i++;
        }
    }

    const char* charset = CharSets::get_charset(charset_name);

    // Detect file type by extension
    const char* ext = strrchr(file_path, '.');
    if (ext != NULL) {
        if (strcmp(ext, ".obj") == 0) {
            // Play pre-converted .obj file
            play_obj_file(file_path);
        } else if (strcmp(ext, ".mp4") == 0 || strcmp(ext, ".avi") == 0 ||
                   strcmp(ext, ".mkv") == 0 || strcmp(ext, ".mov") == 0) {
            // Play video file directly
            play_mp4_direct(file_path, width, charset);
        } else {
            fprintf(stderr, "Error: Unsupported file format '%s'\n", ext);
            fprintf(stderr, "Supported formats: .mp4, .avi, .mkv, .mov, .obj\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Error: Could not detect file type (no extension)\n");
        return 1;
    }

    return 0;
}

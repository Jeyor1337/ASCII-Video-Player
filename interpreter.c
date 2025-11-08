#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <json-c/json.h>

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void play_video(const char *ascii_art_file_path) {
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

    int num_frames = json_object_array_length(frames);
    for (int i = 0; i < num_frames; i++) {
        struct json_object *frame_obj = json_object_array_get_idx(frames, i);
        const char *frame_str = json_object_get_string(frame_obj);
        clear_screen();
        printf("%s\n", frame_str);
        usleep(frame_delay);
    }

    json_object_put(parsed_json);
    clear_screen();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path_to_obj_file>\n", argv[0]);
        return 1;
    }

    play_video(argv[1]);

    return 0;
}

import cv2
import numpy as np
import json
import argparse
import os

def frame_to_ascii(frame, ascii_cols, character_set):

    gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # account for non-square pixels in terminals
    height, width = gray_frame.shape
    cell_width = width / ascii_cols
    cell_height = cell_width * 2
    aspect_ratio_corrected_rows = int(height / cell_height)

    resized_gray = cv2.resize(gray_frame, (ascii_cols, aspect_ratio_corrected_rows))

    ascii_frame = []
    for r in range(aspect_ratio_corrected_rows):
        row_of_chars = []
        for c in range(ascii_cols):
            pixel_brightness = resized_gray[r, c]
            char_index = int(pixel_brightness / 256 * len(character_set))
            row_of_chars.append(character_set[char_index])
        ascii_frame.append("".join(row_of_chars))
    return "\n".join(ascii_frame)

def translate_video(video_path, output_file, ascii_width=120):

    if not os.path.exists(video_path):
        print("Error: Video file not found at '{}'".format(video_path))
        return

    video_capture = cv2.VideoCapture(video_path)

    if not video_capture.isOpened():
        print("Error: Could not open video file '{}'. Check path and format.".format(video_path))
        return

    fps = video_capture.get(cv2.CAP_PROP_FPS)
    total_frames = int(video_capture.get(cv2.CAP_PROP_FRAME_COUNT))
    print('Processing video: {}'.format(video_path))
    print('FPS: {}'.format(fps))
    print('Total frames: {}'.format(total_frames))
    print('ASCII width: {} columns'.format(ascii_width))

    list_of_ascii_frames = []
    frames_processed = 0

    # this is a pretty standard character set for this kind of thing
    CHARACTER_SET = " .:-=+*#%@"

    while True:
        was_successful, frame = video_capture.read()
        if not was_successful:
            break

        ascii_output = frame_to_ascii(frame, ascii_width, CHARACTER_SET)
        list_of_ascii_frames.append(ascii_output)

        frames_processed += 1
        if frames_processed % 100 == 0:
            print("Processed {}/{} frames...".format(frames_processed, total_frames))

    video_capture.release()
    print("Processed all {} frames.".format(frames_processed))

    output_data = {
        "fps": fps,
        "width": ascii_width,
        "height": len(list_of_ascii_frames[0].split('\n')) if list_of_ascii_frames else 0,
        "frames": list_of_ascii_frames
    }

    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(output_data, f, ensure_ascii=False, indent=2)

    print("ASCII video data saved to '{}'".format(output_file))

if __name__ == "__main__":

    arg_parser = argparse.ArgumentParser(description="Convert MP4 videos to ASCII art .obj files.")

    arg_parser.add_argument("input_video", help="Path to the input MP4 video file.")
    arg_parser.add_argument("output_obj", help="Path to the output .obj file.")
    arg_parser.add_argument("--width", type=int, default=120, help="Width of the ASCII art in characters. Default: 120")

    parsed_args = arg_parser.parse_args()

    translate_video(parsed_args.input_video, parsed_args.output_obj, parsed_args.width)

import json
import time
import os
import argparse

def clear_screen():
    # for windows
    if os.name == "nt":
        os.system("cls")
    # for mac and linux
    else:
        os.system("clear")

def play_video(ascii_art_file_path):
    if not os.path.exists(ascii_art_file_path):
        print("Error: File not found at '{}'".format(ascii_art_file_path))
        return

    # load the ascii art file
    try:
        with open(ascii_art_file_path, "r", encoding="utf-8") as f:
            data = json.load(f)
    except json.JSONDecodeError:
        print("Error: '{}' is not a valid JSON file.".format(ascii_art_file_path))
        return
    except Exception as e:
        print("Error reading file: {}".format(e))
        return

    ascii_frames = data.get("frames")
    fps = data.get("fps")

    if not ascii_frames or not isinstance(ascii_frames, list):
        print("Error: No valid 'frames' data found in the .obj file.")
        return

    if not fps or not isinstance(fps, (int, float)) or fps <= 0:
        print("Warning: No valid 'fps' data found in the .obj file. Defaulting to 24.")
        fps = 24

    frame_delay = 1.0 / fps

    print("Starting playback... Press Ctrl+C to stop.")
    time.sleep(2)

    # play the video
    try:
        for frame in ascii_frames:
            clear_screen()
            print(frame)
            time.sleep(frame_delay)
    except KeyboardInterrupt:
        print("\nPlayback stopped.")
    finally:
        clear_screen()

if __name__ == "__main__":

    arg_parser = argparse.ArgumentParser(description="Play ASCII videos defined by .obj files.")

    arg_parser.add_argument("input_obj", help="Path to the input .obj file.")

    parsed_args = arg_parser.parse_args()

    play_video(parsed_args.input_obj)
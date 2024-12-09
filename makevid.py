import cv2
import os
from glob import glob

def frames_to_video(input_folder, output_file, frame_rate=24):
    """
    Combines image frames into a video.

    Parameters:
        input_folder (str): Path to the folder containing image frames.
        output_file (str): Path to the output video file (e.g., 'output.mp4').
        frame_rate (int): Frame rate of the output video (default is 24 fps).

    Returns:
        None
    """
    # Get a sorted list of image files in the input folder
    image_files = sorted(glob(os.path.join(input_folder, '*.png')))
    if not image_files:
        print("No image files found in the specified folder!")
        return

    # Read the first frame to get video dimensions
    frame = cv2.imread(image_files[0])
    height, width, _ = frame.shape

    # Define the codec and create a VideoWriter object
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # 'mp4v' for .mp4
    video_writer = cv2.VideoWriter(output_file, fourcc, frame_rate, (width, height))

    # Write frames to the video
    for img_file in image_files:
        frame = cv2.imread(img_file)
        video_writer.write(frame)

    # Release the video writer
    video_writer.release()
    print(f"Video saved to {output_file}")

if __name__ == "__main__":
    # Example usage
    input_folder = "output"  # Replace with your folder containing frames
    output_file = "output.mp4"  # Replace with desired output file path
    frames_to_video(input_folder, output_file)
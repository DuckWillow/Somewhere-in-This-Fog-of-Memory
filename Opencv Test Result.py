import cv2
import mediapipe as mp
from pynput.keyboard import Key, Controller
from time import time

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_pose = mp.solutions.pose

keyboard = Controller()
volume_up = 0
volume_down = 0

#https://google.github.io/mediapipe/solutions/pose.html

# For static images:
IMAGE_FILES = []
BG_COLOR = (192, 192, 192) # gray
with mp_pose.Pose(
    static_image_mode=True,
    model_complexity=2,
    enable_segmentation=True,
    min_detection_confidence=0.5) as pose:
  for idx, file in enumerate(IMAGE_FILES):
    image = cv2.imread(file)
    image_height, image_width, _ = image.shape
    # Convert the BGR image to RGB before processing.
    results = pose.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))

    if not results.pose_landmarks:
      continue
    print(
        f'Nose coordinates: ('
        f'{results.pose_landmarks.landmark[mp_pose.PoseLandmark.NOSE].x * image_width}, '
        f'{results.pose_landmarks.landmark[mp_pose.PoseLandmark.NOSE].y * image_height})'
    )

    annotated_image = image.copy()
    # Draw segmentation on the image.
    # To improve segmentation around boundaries, consider applying a joint
    # bilateral filter to "results.segmentation_mask" with "image".
    condition = np.stack((results.segmentation_mask,) * 3, axis=-1) > 0.1
    bg_image = np.zeros(image.shape, dtype=np.uint8)
    bg_image[:] = BG_COLOR
    annotated_image = np.where(condition, annotated_image, bg_image)
    # Draw pose landmarks on the image.
    mp_drawing.draw_landmarks(
        annotated_image,
        results.pose_landmarks,
        mp_pose.POSE_CONNECTIONS,
        landmark_drawing_spec=mp_drawing_styles.get_default_pose_landmarks_style())
    cv2.imwrite('/tmp/annotated_image' + str(idx) + '.png', annotated_image)
    # Plot pose world landmarks.
    mp_drawing.plot_landmarks(
        results.pose_world_landmarks, mp_pose.POSE_CONNECTIONS)

# For webcam input:
cap = cv2.VideoCapture(0)
with mp_pose.Pose(
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as pose:
  while cap.isOpened():
    success, image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      continue

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = pose.process(image)

    # Draw the pose annotation on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    mp_drawing.draw_landmarks(
        image,
        results.pose_landmarks,
        mp_pose.POSE_CONNECTIONS,
        landmark_drawing_spec=mp_drawing_styles.get_default_pose_landmarks_style())
    
    # Flip the image horizontally for a selfie-view display.
    cv2.imshow('MediaPipe Pose', cv2.flip(image, 1))
    if cv2.waitKey(5) & 0xFF == 113:
      break
    else:
      if not results.pose_landmarks:
        print(results.pose_landmarks)
      else:
        print(results.pose_landmarks)
        result_str = str(results.pose_landmarks)
        result_list = result_str.split("landmark ")
        # len(result_list) = 34
        # index 12-13 = shoulders
        # index 14 = left_elbow
        # index 15 = right_elbow
        shoulder1 = result_list[12].split(": ")
        shoulder2 = result_list[13].split(": ")
        left_elbow = result_list[14].split(": ")
        right_elbow = result_list[15].split(": ")
        # index 2 = y (0 at top/ 1 at bottom)
        # index 4 = visibility
        shoulder_visibility_threshold = 0.9
        elbow_visibility_threshold = 0.8
        if (float(shoulder1[4][0:5])> shoulder_visibility_threshold) and (float(shoulder2[4][0:5])> shoulder_visibility_threshold):
          if (float(left_elbow[4][0:5])> elbow_visibility_threshold) and (float(right_elbow[4][0:5])> elbow_visibility_threshold):
            #increment
            if float(left_elbow[2][0:5]) < float(shoulder1[2][0:5]):
              volume_down = volume_down + 1
              #print('left high')
            else:
              volume_down = 0
            if float(right_elbow[2][0:5]) < float(shoulder1[2][0:5]):
              volume_up = volume_up + 1
              #print('right high')
            else:
              volume_up = 0
            if float(left_elbow[2][0:5]) < float(shoulder1[2][0:5]) and float(right_elbow[2][0:5]) < float(shoulder1[2][0:5]):
              volume_up = 0
              volume_down = 0
            if volume_up >= 10:
              volume_up = 0
              print('right high')
              keyboard.press("a")
              keyboard.release("a")
            if volume_down >= 10:
              volume_down = 0
              print('left high')
              keyboard.press("b")
              keyboard.release("b")
##            print('see')
##            print(float(shoulder1[4][0:5]))
##            print(float(shoulder2[4][0:5]))
##            print(float(left_elbow[4][0:5]))
##            print(float(right_elbow[4][0:5]))
##            milliseconds = int(time() * 1000)
##            print("Time in milliseconds since epoch", milliseconds)+
          
cap.release()

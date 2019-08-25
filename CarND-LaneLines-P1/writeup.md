# **Finding Lane Lines on the Road** 

### Reflection

### 1. Describe your pipeline. As part of the description, explain how you modified the draw_lines() function.

My pipeline consisted of 6 steps. First, I converted the images to grayscale, then I .... 

1. Firstly , I converted image to grayscaled image with cv2.

2. Then, I applied gaussian blur before applyin Canny Edge Detection, It is required to smoothen the image in hope of better result, the kernel size was 3 without any tuning.

3. Next, I applied Canny Edge Detection - one of main technique from the course to the image; I spent some time to find the min and max threshold which reflected the best result from my observation. The final values were 80 and 240 which corresponded to recommended ratio 1:3.

4. Then we have to define the region of interest boundary that shows only necessary lane lines. I used 4-sided polygon to mask the region. the bottom 2 points were simply both ends of the image frame, while the other top two points were found using observation with a few trial and errors.

5. The next step was to do Hough Transformation,with default parameters and detect lines using it.

6. The final step was to draw the lines obtained from Hough Transformation in function draw_lines. I first calculated the slope from the 2 points of any line obtained from perious steps using the formular (y2-y1)/(x2-x1). If the slope is positive , I would put those point in a list of right lines and vice versa, I discarded points woth zero slope as they are horizonetal lines which are kind of noise. Using these 2 right and left lists, I find the coefficient of 1st-degree polynomial and then construct the left and right lines function. Next step was to fin y coordinates with x-coordinates of 2 left points and then 2 right points. With the list of all the (x,y) coordinates for both right and left lines, I simple draw the lines



### 2. Identify potential shortcomings with your current pipeline

Possible shortcomings with y current pipeline:
Lane lines are sometimes missed due to lightining,shadow or weather conditions.
Currently there are no cars infront of the car in that area of operation but if there are other cars it could lead to take them as noise.


### 3. Suggest possible improvements to your pipeline
 
One of the observation waws its not perfroming well on "test_videos_output/challenge.mp4" as lane line are very curved and car was keep turning.
also it might possible that above solution will not work path would be little zig-zag .

One possible improvement was to apply the color filter to identify the lane lines as its taking noise from shadow also.

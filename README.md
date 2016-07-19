# AutoROIdetection
Find the edges of the water canal.

1. Run Clahe filter (equalize the image)
2. Run Sobel filter with x-gradient of the 4th degree
3. Filter out pixels with value below 150
4. Sum up the columns
	We can notice the the wall edges are those where the spikes without noise are. (show image)
5. Find the columns with biggest gradient
6. Chose two of those that are biggest in its neighborhood (10 pixels left and right)
	Under the following conditions: they have to be far away from each other, cannot be on the image edges, and have to be one on each half of the image



Requirements: cmake, openCV

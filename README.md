# ImageSectionTool
A tool for the annotation of image areas and extraction of the resulting segmentation information for use in machine learning.

![Preview Image](https://raw.githubusercontent.com/R-Rudolph/ImageSectionTool/master/ImageSectionTool.png)

### Features
* Annotate Image Regions with bounding polygons with the 'Section' tool
* Annotate Points with the 'Point' tool
* Manage test and training data
* Extract image patches containing/not containing selected regions from the data sets (Extract Data->Classification Data)
* Extract image patches and image patches with their segmentation data (Extract Data->Classification Data)

### Controls
* Clicking left adds an annotation to the given image position.
* Double-clicking left finishes a bounding polygon.
* Double-clicking right removes an annotation, or the last point of the bounding polygon, if one is currently being edited.
* Holding the right or middle mouse button and moving the mouse will scroll the image.
* Using the mouse wheel, the image can be zoomed in and out.

### Requirements
* Qt 5

### Compiling
    mkdir build
    qmake ..
    make
    

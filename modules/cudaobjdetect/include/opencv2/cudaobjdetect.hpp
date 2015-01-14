/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_CUDAOBJDETECT_HPP__
#define __OPENCV_CUDAOBJDETECT_HPP__

#ifndef __cplusplus
#  error cudaobjdetect.hpp header must be compiled as C++
#endif

#include "opencv2/core/cuda.hpp"

/**
  @addtogroup cuda
  @{
      @defgroup cuda_objdetect Object Detection
  @}
 */

namespace cv { namespace cuda {

//! @addtogroup cuda_objdetect
//! @{

//
// HOG (Histogram-of-Oriented-Gradients) Descriptor and Object Detector
//

/** @brief The class implements Histogram of Oriented Gradients (@cite Dalal2005) object detector.

@note
    -   An example applying the HOG descriptor for people detection can be found at
        opencv_source_code/samples/cpp/peopledetect.cpp
    -   A CUDA example applying the HOG descriptor for people detection can be found at
        opencv_source_code/samples/gpu/hog.cpp
    -   (Python) An example applying the HOG descriptor for people detection can be found at
        opencv_source_code/samples/python2/peopledetect.py
 */
class CV_EXPORTS HOG : public cv::Algorithm
{
public:
    enum
    {
        DESCR_FORMAT_ROW_BY_ROW,
        DESCR_FORMAT_COL_BY_COL
    };

    /** @brief Creates the HOG descriptor and detector.

    @param win_size Detection window size. Align to block size and block stride.
    @param block_size Block size in pixels. Align to cell size. Only (16,16) is supported for now.
    @param block_stride Block stride. It must be a multiple of cell size.
    @param cell_size Cell size. Only (8, 8) is supported for now.
    @param nbins Number of bins. Only 9 bins per cell are supported for now.
     */
    static Ptr<HOG> create(Size win_size = Size(64, 128),
                           Size block_size = Size(16, 16),
                           Size block_stride = Size(8, 8),
                           Size cell_size = Size(8, 8),
                           int nbins = 9);

    //! Gaussian smoothing window parameter.
    virtual void setWinSigma(double win_sigma) = 0;
    virtual double getWinSigma() const = 0;

    //! L2-Hys normalization method shrinkage.
    virtual void setL2HysThreshold(double threshold_L2hys) = 0;
    virtual double getL2HysThreshold() const = 0;

    //! Flag to specify whether the gamma correction preprocessing is required or not.
    virtual void setGammaCorrection(bool gamma_correction) = 0;
    virtual bool getGammaCorrection() const = 0;

    //! Maximum number of detection window increases.
    virtual void setNumLevels(int nlevels) = 0;
    virtual int getNumLevels() const = 0;

    //! Threshold for the distance between features and SVM classifying plane.
    //! Usually it is 0 and should be specfied in the detector coefficients (as the last free
    //! coefficient). But if the free coefficient is omitted (which is allowed), you can specify it
    //! manually here.
    virtual void setHitThreshold(double hit_threshold) = 0;
    virtual double getHitThreshold() const = 0;

    //! Window stride. It must be a multiple of block stride.
    virtual void setWinStride(Size win_stride) = 0;
    virtual Size getWinStride() const = 0;

    //! Coefficient of the detection window increase.
    virtual void setScaleFactor(double scale0) = 0;
    virtual double getScaleFactor() const = 0;

    //! Coefficient to regulate the similarity threshold. When detected, some
    //! objects can be covered by many rectangles. 0 means not to perform grouping.
    //! See groupRectangles.
    virtual void setGroupThreshold(int group_threshold) = 0;
    virtual int getGroupThreshold() const = 0;

    //! Descriptor storage format:
    //!   - **DESCR_FORMAT_ROW_BY_ROW** - Row-major order.
    //!   - **DESCR_FORMAT_COL_BY_COL** - Column-major order.
    virtual void setDescriptorFormat(int descr_format) = 0;
    virtual int getDescriptorFormat() const = 0;

    /** @brief Returns the number of coefficients required for the classification.
     */
    virtual size_t getDescriptorSize() const = 0;

    /** @brief Returns the block histogram size.
     */
    virtual size_t getBlockHistogramSize() const = 0;

    /** @brief Sets coefficients for the linear SVM classifier.
     */
    virtual void setSVMDetector(InputArray detector) = 0;

    /** @brief Returns coefficients of the classifier trained for people detection.
     */
    virtual Mat getDefaultPeopleDetector() const = 0;

    /** @brief Performs object detection without a multi-scale window.

    @param img Source image. CV_8UC1 and CV_8UC4 types are supported for now.
    @param found_locations Left-top corner points of detected objects boundaries.
    @param confidences Optional output array for confidences.
     */
    virtual void detect(InputArray img,
                        std::vector<Point>& found_locations,
                        std::vector<double>* confidences = NULL) = 0;

    /** @brief Performs object detection with a multi-scale window.

    @param img Source image. See cuda::HOGDescriptor::detect for type limitations.
    @param found_locations Detected objects boundaries.
    @param confidences Optional output array for confidences.
    @param hit_threshold Threshold for the distance between features and SVM classifying plane. See
    cuda::HOGDescriptor::detect for details.
    @param win_stride Window stride. It must be a multiple of block stride.
    @param padding Mock parameter to keep the CPU interface compatibility. It must be (0,0).
     */
    virtual void detectMultiScale(InputArray img,
                                  std::vector<Rect>& found_locations,
                                  std::vector<double>* confidences = NULL) = 0;

    /** @brief Returns block descriptors computed for the whole image.

    @param img Source image. See cuda::HOGDescriptor::detect for type limitations.
    @param descriptors 2D array of descriptors.
    @param stream CUDA stream.
     */
    virtual void compute(InputArray img,
                         OutputArray descriptors,
                         Stream& stream = Stream::Null()) = 0;
};

//
// CascadeClassifier
//

/** @brief Cascade classifier class used for object detection. Supports HAAR and LBP cascades. :

@note
   -   A cascade classifier example can be found at
        opencv_source_code/samples/gpu/cascadeclassifier.cpp
    -   A Nvidea API specific cascade classifier example can be found at
        opencv_source_code/samples/gpu/cascadeclassifier_nvidia_api.cpp
 */
class CV_EXPORTS CascadeClassifier_CUDA
{
public:
    CascadeClassifier_CUDA();
    /** @brief Loads the classifier from a file. Cascade type is detected automatically by constructor parameter.

    @param filename Name of the file from which the classifier is loaded. Only the old haar classifier
    (trained by the haar training application) and NVIDIA's nvbin are supported for HAAR and only new
    type of OpenCV XML cascade supported for LBP.
     */
    CascadeClassifier_CUDA(const String& filename);
    ~CascadeClassifier_CUDA();

    /** @brief Checks whether the classifier is loaded or not.
    */
    bool empty() const;
    /** @brief Loads the classifier from a file. The previous content is destroyed.

    @param filename Name of the file from which the classifier is loaded. Only the old haar classifier
    (trained by the haar training application) and NVIDIA's nvbin are supported for HAAR and only new
    type of OpenCV XML cascade supported for LBP.
     */
    bool load(const String& filename);
    /** @brief Destroys the loaded classifier.
    */
    void release();

    /** @overload */
    int detectMultiScale(const GpuMat& image, GpuMat& objectsBuf, double scaleFactor = 1.2, int minNeighbors = 4, Size minSize = Size());
    /** @brief Detects objects of different sizes in the input image.

    @param image Matrix of type CV_8U containing an image where objects should be detected.
    @param objectsBuf Buffer to store detected objects (rectangles). If it is empty, it is allocated
    with the default size. If not empty, the function searches not more than N objects, where
    N = sizeof(objectsBufer's data)/sizeof(cv::Rect).
    @param maxObjectSize Maximum possible object size. Objects larger than that are ignored. Used for
    second signature and supported only for LBP cascades.
    @param scaleFactor Parameter specifying how much the image size is reduced at each image scale.
    @param minNeighbors Parameter specifying how many neighbors each candidate rectangle should have
    to retain it.
    @param minSize Minimum possible object size. Objects smaller than that are ignored.

    The detected objects are returned as a list of rectangles.

    The function returns the number of detected objects, so you can retrieve them as in the following
    example:
    @code
        cuda::CascadeClassifier_CUDA cascade_gpu(...);

        Mat image_cpu = imread(...)
        GpuMat image_gpu(image_cpu);

        GpuMat objbuf;
        int detections_number = cascade_gpu.detectMultiScale( image_gpu,
                  objbuf, 1.2, minNeighbors);

        Mat obj_host;
        // download only detected number of rectangles
        objbuf.colRange(0, detections_number).download(obj_host);

        Rect* faces = obj_host.ptr<Rect>();
        for(int i = 0; i < detections_num; ++i)
           cv::rectangle(image_cpu, faces[i], Scalar(255));

        imshow("Faces", image_cpu);
    @endcode
    @sa CascadeClassifier::detectMultiScale
     */
    int detectMultiScale(const GpuMat& image, GpuMat& objectsBuf, Size maxObjectSize, Size minSize = Size(), double scaleFactor = 1.1, int minNeighbors = 4);

    bool findLargestObject;
    bool visualizeInPlace;

    Size getClassifierSize() const;

private:
    struct CascadeClassifierImpl;
    CascadeClassifierImpl* impl;
    struct HaarCascade;
    struct LbpCascade;
    friend class CascadeClassifier_CUDA_LBP;
};

//! @}

}} // namespace cv { namespace cuda {

#endif /* __OPENCV_CUDAOBJDETECT_HPP__ */

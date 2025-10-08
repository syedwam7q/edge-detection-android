#ifndef EDGE_PROCESSOR_H
#define EDGE_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * EdgeProcessor - Handles OpenCV-based image processing operations
 */
class EdgeProcessor {
public:
    EdgeProcessor();
    ~EdgeProcessor();
    
    /**
     * Apply Canny edge detection to an image
     * @param inputImage Input BGR/RGB image
     * @param outputImage Output edge-detected image
     * @param lowThreshold Lower threshold for Canny detector
     * @param highThreshold Upper threshold for Canny detector
     */
    void applyCannyEdgeDetection(
        const cv::Mat& inputImage,
        cv::Mat& outputImage,
        double lowThreshold = 50.0,
        double highThreshold = 150.0
    );
    
    /**
     * Convert image to grayscale
     * @param inputImage Input BGR/RGB image
     * @param outputImage Output grayscale image
     */
    void convertToGrayscale(const cv::Mat& inputImage, cv::Mat& outputImage);
    
    /**
     * Apply Gaussian blur to reduce noise before edge detection
     * @param inputImage Input image
     * @param outputImage Output blurred image
     * @param kernelSize Kernel size (must be odd)
     */
    void applyGaussianBlur(
        const cv::Mat& inputImage,
        cv::Mat& outputImage,
        int kernelSize = 5
    );
    
    /**
     * Process frame with edge detection pipeline
     * @param inputData Input RGB data
     * @param width Image width
     * @param height Image height
     * @param outputData Output processed RGB data
     */
    void processFrame(
        const std::vector<uint8_t>& inputData,
        int width,
        int height,
        std::vector<uint8_t>& outputData
    );
    
    /**
     * Get last processing time in milliseconds
     */
    double getProcessingTime() const { return processingTime; }
    
private:
    double processingTime;
};

#endif // EDGE_PROCESSOR_H
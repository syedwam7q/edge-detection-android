#ifndef EDGE_PROCESSOR_H
#define EDGE_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <vector>

/**
 * EdgeProcessor - Handles OpenCV-based image processing operations
 * Enhanced with advanced preprocessing and edge refinement techniques
 */
class EdgeProcessor {
public:
    EdgeProcessor();
    ~EdgeProcessor();
    
    /**
     * Apply enhanced Canny edge detection to an image
     * Includes CLAHE, sharpening, bilateral filtering, and morphological operations
     * @param inputImage Input BGR/RGB image
     * @param outputImage Output edge-detected image
     * @param lowThreshold Lower threshold for Canny detector (default: 50.0)
     * @param highThreshold Upper threshold for Canny detector (default: 150.0)
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
     * Apply CLAHE (Contrast Limited Adaptive Histogram Equalization)
     * Enhances local contrast for better edge detection
     * @param inputImage Input grayscale image
     * @param outputImage Output enhanced image
     */
    void applyCLAHE(const cv::Mat& inputImage, cv::Mat& outputImage);
    
    /**
     * Apply unsharp masking to sharpen the image
     * Improves edge clarity before detection
     * @param inputImage Input image
     * @param outputImage Output sharpened image
     */
    void applySharpen(const cv::Mat& inputImage, cv::Mat& outputImage);
    
    /**
     * Apply morphological operations to clean up edges
     * Closes gaps and removes small noise
     * @param edgeImage Input edge image
     * @param outputImage Output cleaned edge image
     */
    void refineMorphology(const cv::Mat& edgeImage, cv::Mat& outputImage);
    
    /**
     * Process frame with enhanced edge detection pipeline
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
    
    /**
     * Set edge detection quality level
     * @param level 0=Fast (lower quality), 1=Normal (default), 2=High (slower)
     */
    void setQualityLevel(int level) { qualityLevel = level; }
    
private:
    double processingTime;
    int qualityLevel; // 0=Fast, 1=Normal, 2=High
    cv::Ptr<cv::CLAHE> clahe; // CLAHE processor instance
};

#endif // EDGE_PROCESSOR_H
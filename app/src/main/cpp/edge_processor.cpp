#include "edge_processor.h"
#include <chrono>
#include <android/log.h>

#define LOG_TAG "EdgeProcessor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EdgeProcessor::EdgeProcessor() : processingTime(0.0), qualityLevel(1) {
    // Initialize CLAHE with optimal parameters
    // clipLimit: Controls contrast enhancement strength
    // tileGridSize: Size of grid for local histogram equalization
    clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
    LOGI("EdgeProcessor initialized with enhanced preprocessing");
}

EdgeProcessor::~EdgeProcessor() {
    LOGI("EdgeProcessor destroyed");
}

void EdgeProcessor::applyCannyEdgeDetection(
    const cv::Mat& inputImage,
    cv::Mat& outputImage,
    double lowThreshold,
    double highThreshold
) {
    cv::Mat gray, enhanced, sharpened, filtered, edges, refined;
    
    // Step 1: Convert to grayscale if needed
    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, gray, cv::COLOR_RGB2GRAY);
    } else {
        gray = inputImage.clone();
    }
    
    // Step 2: Apply CLAHE for contrast enhancement
    // This significantly improves edge detection in varied lighting conditions
    applyCLAHE(gray, enhanced);
    
    // Step 3: Apply sharpening to make edges more distinct
    // Skip sharpening in fast mode to save processing time
    if (qualityLevel >= 1) {
        applySharpen(enhanced, sharpened);
    } else {
        sharpened = enhanced;
    }
    
    // Step 4: Noise reduction while preserving edges
    // Bilateral filter is better than Gaussian for edge preservation
    if (qualityLevel >= 2) {
        // High quality: stronger bilateral filter
        cv::bilateralFilter(sharpened, filtered, 9, 75, 75);
    } else {
        // Normal/Fast: lighter bilateral filter for speed
        cv::bilateralFilter(sharpened, filtered, 5, 50, 50);
    }
    
    // Step 5: Apply Canny edge detection with improved thresholds
    // Higher thresholds reduce noise and focus on strong edges
    cv::Canny(filtered, edges, lowThreshold, highThreshold, 3, true);
    
    // Step 6: Morphological operations to refine edges
    // This closes small gaps and removes isolated noise pixels
    refineMorphology(edges, refined);
    
    // Step 7: Convert back to RGB for display
    cv::cvtColor(refined, outputImage, cv::COLOR_GRAY2RGB);
    
    LOGI("Edge detection complete - thresholds: %.1f/%.1f, quality: %d", 
         lowThreshold, highThreshold, qualityLevel);
}

void EdgeProcessor::convertToGrayscale(const cv::Mat& inputImage, cv::Mat& outputImage) {
    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, outputImage, cv::COLOR_RGB2GRAY);
    } else {
        outputImage = inputImage.clone();
    }
}

void EdgeProcessor::applyGaussianBlur(
    const cv::Mat& inputImage,
    cv::Mat& outputImage,
    int kernelSize
) {
    // Ensure kernel size is odd
    if (kernelSize % 2 == 0) {
        kernelSize++;
    }
    
    cv::GaussianBlur(inputImage, outputImage, cv::Size(kernelSize, kernelSize), 0);
}

void EdgeProcessor::applyCLAHE(const cv::Mat& inputImage, cv::Mat& outputImage) {
    // CLAHE (Contrast Limited Adaptive Histogram Equalization)
    // Enhances local contrast without amplifying noise
    // This is crucial for detecting edges in poorly lit or low contrast images
    if (clahe.empty()) {
        clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
    }
    clahe->apply(inputImage, outputImage);
}

void EdgeProcessor::applySharpen(const cv::Mat& inputImage, cv::Mat& outputImage) {
    // Unsharp masking: enhances edges by subtracting a blurred version
    // This makes edges more pronounced before Canny detection
    cv::Mat blurred;
    cv::GaussianBlur(inputImage, blurred, cv::Size(0, 0), 3);
    cv::addWeighted(inputImage, 1.5, blurred, -0.5, 0, outputImage);
}

void EdgeProcessor::refineMorphology(const cv::Mat& edgeImage, cv::Mat& outputImage) {
    // Morphological operations to clean up and strengthen edges
    
    // Step 1: Dilate to thicken edges and close small gaps
    cv::Mat dilated;
    cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::dilate(edgeImage, dilated, dilateKernel, cv::Point(-1, -1), 1);
    
    // Step 2: Close operation to connect nearby edges
    cv::Mat closed;
    cv::Mat closeKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::morphologyEx(dilated, closed, cv::MORPH_CLOSE, closeKernel);
    
    // Step 3: Remove small noise with opening (erode then dilate)
    cv::Mat opened;
    cv::Mat openKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
    cv::morphologyEx(closed, opened, cv::MORPH_OPEN, openKernel);
    
    outputImage = opened;
}

void EdgeProcessor::processFrame(
    const std::vector<uint8_t>& inputData,
    int width,
    int height,
    std::vector<uint8_t>& outputData
) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create OpenCV Mat from input data
        cv::Mat inputMat(height, width, CV_8UC3, (void*)inputData.data());
        cv::Mat outputMat;
        
        // Apply enhanced edge detection with optimized thresholds
        // Using 50.0 and 150.0 instead of default 10.0 and 50.0
        applyCannyEdgeDetection(inputMat, outputMat, 50.0, 150.0);
        
        // Copy output to vector
        outputData.resize(width * height * 3);
        std::memcpy(outputData.data(), outputMat.data, outputData.size());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        LOGI("Frame processed in %.2f ms (quality level: %d)", processingTime, qualityLevel);
        
    } catch (const cv::Exception& e) {
        LOGE("OpenCV exception: %s", e.what());
        // Return input data as fallback
        outputData = inputData;
    }
}
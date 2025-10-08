#include "edge_processor.h"
#include <chrono>
#include <android/log.h>

#define LOG_TAG "EdgeProcessor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EdgeProcessor::EdgeProcessor() : processingTime(0.0) {
    LOGI("EdgeProcessor initialized");
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
    cv::Mat gray, blurred, edges;
    
    // Convert to grayscale if needed
    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, gray, cv::COLOR_RGB2GRAY);
    } else {
        gray = inputImage.clone();
    }
    
    // Apply bilateral filter for edge-preserving noise reduction
    cv::Mat filtered;
    cv::bilateralFilter(gray, filtered, 9, 75, 75);
    
    // Apply Canny edge detection with optimized thresholds
    cv::Canny(filtered, edges, lowThreshold, highThreshold);
    
    // Optional: Dilate edges slightly to make them more visible
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::dilate(edges, edges, kernel);
    
    // Convert back to RGB for display
    cv::cvtColor(edges, outputImage, cv::COLOR_GRAY2RGB);
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
        
        // Apply edge detection
        applyCannyEdgeDetection(inputMat, outputMat);
        
        // Copy output to vector
        outputData.resize(width * height * 3);
        std::memcpy(outputData.data(), outputMat.data, outputData.size());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        LOGI("Frame processed in %.2f ms", processingTime);
        
    } catch (const cv::Exception& e) {
        LOGE("OpenCV exception: %s", e.what());
        // Return input data as fallback
        outputData = inputData;
    }
}
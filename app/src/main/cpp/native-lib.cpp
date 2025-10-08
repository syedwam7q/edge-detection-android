#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include "edge_processor.h"

#define LOG_TAG "NativeLib"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Global edge processor instance
static EdgeProcessor* g_edgeProcessor = nullptr;

extern "C" {

/**
 * Initialize OpenCV library
 */
JNIEXPORT jboolean JNICALL
Java_com_flam_edgedetection_NativeLib_initOpenCV(JNIEnv* env, jobject /* this */) {
    LOGI("Initializing OpenCV");
    
    try {
        if (g_edgeProcessor == nullptr) {
            g_edgeProcessor = new EdgeProcessor();
        }
        LOGI("OpenCV initialized successfully");
        return JNI_TRUE;
    } catch (const std::exception& e) {
        LOGE("Failed to initialize OpenCV: %s", e.what());
        return JNI_FALSE;
    }
}

/**
 * Process frame with edge detection
 */
JNIEXPORT jbyteArray JNICALL
Java_com_flam_edgedetection_NativeLib_processFrame(
    JNIEnv* env,
    jobject /* this */,
    jbyteArray imageData,
    jint width,
    jint height,
    jboolean applyEdgeDetection
) {
    if (g_edgeProcessor == nullptr) {
        LOGE("EdgeProcessor not initialized");
        return imageData;
    }
    
    // Get input data
    jbyte* inputBytes = env->GetByteArrayElements(imageData, nullptr);
    jsize inputSize = env->GetArrayLength(imageData);
    
    std::vector<uint8_t> inputVector(inputBytes, inputBytes + inputSize);
    env->ReleaseByteArrayElements(imageData, inputBytes, JNI_ABORT);
    
    std::vector<uint8_t> outputVector;
    
    if (applyEdgeDetection) {
        // Process with edge detection
        g_edgeProcessor->processFrame(inputVector, width, height, outputVector);
    } else {
        // Return original data
        outputVector = inputVector;
    }
    
    // Create output array
    jbyteArray outputArray = env->NewByteArray(outputVector.size());
    env->SetByteArrayRegion(outputArray, 0, outputVector.size(),
                           reinterpret_cast<const jbyte*>(outputVector.data()));
    
    return outputArray;
}

/**
 * Convert YUV420 to RGB
 */
JNIEXPORT jbyteArray JNICALL
Java_com_flam_edgedetection_NativeLib_yuv420ToRgb(
    JNIEnv* env,
    jobject /* this */,
    jbyteArray yuvData,
    jint width,
    jint height,
    jint rowStride
) {
    jbyte* yuvBytes = env->GetByteArrayElements(yuvData, nullptr);
    jsize yuvSize = env->GetArrayLength(yuvData);
    
    // Create OpenCV Mat from YUV data
    cv::Mat yuvMat(height + height / 2, width, CV_8UC1, yuvBytes);
    cv::Mat rgbMat;
    
    // Convert YUV to RGB
    cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_NV21);
    
    env->ReleaseByteArrayElements(yuvData, yuvBytes, JNI_ABORT);
    
    // Create output array
    int rgbSize = rgbMat.total() * rgbMat.elemSize();
    jbyteArray rgbArray = env->NewByteArray(rgbSize);
    env->SetByteArrayRegion(rgbArray, 0, rgbSize,
                           reinterpret_cast<const jbyte*>(rgbMat.data));
    
    return rgbArray;
}

/**
 * Get processing time
 */
JNIEXPORT jdouble JNICALL
Java_com_flam_edgedetection_NativeLib_getProcessingTime(JNIEnv* env, jobject /* this */) {
    if (g_edgeProcessor == nullptr) {
        return 0.0;
    }
    return g_edgeProcessor->getProcessingTime();
}

} // extern "C"
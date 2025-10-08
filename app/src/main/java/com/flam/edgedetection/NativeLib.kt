package com.flam.edgedetection

/**
 * Native library interface for OpenCV processing via JNI
 */
object NativeLib {
    
    /**
     * Initialize OpenCV library
     */
    external fun initOpenCV(): Boolean
    
    /**
     * Process frame with edge detection or other filters
     * @param imageData RGB image data
     * @param width Image width
     * @param height Image height
     * @param applyEdgeDetection Whether to apply Canny edge detection
     * @return Processed image data
     */
    external fun processFrame(
        imageData: ByteArray,
        width: Int,
        height: Int,
        applyEdgeDetection: Boolean
    ): ByteArray
    
    /**
     * Convert YUV420 to RGB
     * @param yuvData YUV image data
     * @param width Image width
     * @param height Image height
     * @param rowStride Row stride
     * @return RGB image data
     */
    external fun yuv420ToRgb(
        yuvData: ByteArray,
        width: Int,
        height: Int,
        rowStride: Int
    ): ByteArray
    
    /**
     * Get processing time in milliseconds
     */
    external fun getProcessingTime(): Double
    
    /**
     * Set edge detection quality level
     * @param level 0=Fast (lower quality), 1=Normal (default), 2=High (slower but better)
     */
    external fun setQualityLevel(level: Int)
}
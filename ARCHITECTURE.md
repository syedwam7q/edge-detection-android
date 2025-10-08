# Architecture Document

## System Overview

This project implements a real-time edge detection system using:
- **Android**: UI and camera capture
- **C++/OpenCV**: Image processing
- **JNI**: Java-Native bridge
- **OpenGL ES**: Hardware-accelerated rendering
- **TypeScript**: Web-based viewer

## Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Android Application                      │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐       ┌──────────────┐                    │
│  │ MainActivity │◄──────┤  CameraX API │                    │
│  └───────┬──────┘       └──────────────┘                    │
│          │                                                    │
│          │ Frame Callback                                    │
│          ▼                                                    │
│  ┌──────────────────────────────────────────────┐           │
│  │           ImageAnalysis                       │           │
│  │  - YUV420 Image Data                         │           │
│  │  - Convert to RGB via JNI                    │           │
│  └────────────────┬─────────────────────────────┘           │
│                   │                                           │
│                   │ JNI Call                                 │
│                   ▼                                           │
├═══════════════════════════════════════════════════════════════┤
│                    JNI Bridge                                 │
│  ┌────────────────────────────────────────────┐              │
│  │  native-lib.cpp                            │              │
│  │  - yuv420ToRgb()                          │              │
│  │  - processFrame()                         │              │
│  │  - getProcessingTime()                    │              │
│  └───────────────┬────────────────────────────┘              │
│                  │                                            │
├══════════════════▼═══════════════════════════════════════════┤
│               Native C++ Layer                                │
│  ┌──────────────────────────────────────────┐                │
│  │  EdgeProcessor (edge_processor.cpp)      │                │
│  │                                           │                │
│  │  1. Convert RGB to Grayscale            │                │
│  │  2. Apply Gaussian Blur                 │                │
│  │  3. Canny Edge Detection                │                │
│  │  4. Convert back to RGB                 │                │
│  │                                           │                │
│  │  Uses: OpenCV C++ API                   │                │
│  └───────────────┬───────────────────────────┘                │
│                  │                                            │
│                  │ Return Processed Image                     │
│                  ▼                                            │
├═══════════════════════════════════════════════════════════════┤
│                Back to Java Layer                             │
│  ┌──────────────────────────────────────────┐                │
│  │  MainActivity receives ByteArray         │                │
│  │  - Converts to texture data              │                │
│  │  - Updates FPS counter                   │                │
│  └───────────────┬───────────────────────────┘                │
│                  │                                            │
│                  │ updateTexture()                           │
│                  ▼                                            │
│  ┌──────────────────────────────────────────┐                │
│  │  GLRenderer (OpenGL ES 2.0)              │                │
│  │                                           │                │
│  │  - Create texture from image data        │                │
│  │  - Vertex & Fragment Shaders             │                │
│  │  - Render to GLSurfaceView               │                │
│  └──────────────────────────────────────────┘                │
│                                                               │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                   TypeScript Web Viewer                       │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────┐                │
│  │  EdgeDetectionViewer (index.ts)          │                │
│  │                                           │                │
│  │  - Load images (upload or sample)        │                │
│  │  - Client-side Sobel edge detection      │                │
│  │  - Render to Canvas                      │                │
│  │  - Display statistics                    │                │
│  │  - Update frame info                     │                │
│  │                                           │                │
│  │  Processing Pipeline:                    │                │
│  │  1. Grayscale conversion                 │                │
│  │  2. Sobel operator (3x3 kernels)        │                │
│  │  3. Gradient magnitude calculation       │                │
│  │  4. Thresholding (value: 50)            │                │
│  │  5. Canvas rendering                     │                │
│  └──────────────────────────────────────────┘                │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

### Frame Processing Pipeline

1. **Camera Capture** (Android Java/Kotlin)
   ```
   Camera → YUV420 ImageProxy → Byte Array
   ```

2. **Color Conversion** (JNI → C++)
   ```
   YUV420 → OpenCV cvtColor() → RGB24
   ```

3. **Edge Detection** (OpenCV C++)
   ```
   RGB → Grayscale → Gaussian Blur → Canny → RGB
   ```
   
   Canny algorithm:
   - Low threshold: 50
   - High threshold: 150
   - Kernel size: 5×5

4. **Texture Upload** (Java → OpenGL)
   ```
   RGB ByteArray → ByteBuffer → GL Texture
   ```

5. **Rendering** (OpenGL ES)
   ```
   Texture → Vertex Shader → Fragment Shader → Screen
   ```

## Threading Model

```
┌─────────────────┐
│   Main Thread   │  UI updates, button clicks
└────────┬────────┘
         │
         ├──────────┐
         │          │
    ┌────▼────┐  ┌──▼────────────┐
    │ Camera  │  │  GL Thread    │
    │ Thread  │  │  (rendering)  │
    └────┬────┘  └───────────────┘
         │
         │ Frame callback
         │
    ┌────▼─────────┐
    │  Executor    │  Image processing
    │  Thread      │  (JNI calls)
    └──────────────┘
```

## Memory Management

### Java Side
- Camera frames: Managed by CameraX (auto-released)
- ByteArrays: Garbage collected
- OpenGL textures: Reused for each frame

### Native Side
- OpenCV Mat objects: RAII (automatic cleanup)
- ByteBuffer: Allocated once, reused
- No manual memory management needed

## Performance Optimization

1. **Buffer Reuse**
   - OpenGL texture reused across frames
   - No allocation per frame

2. **Backpressure Strategy**
   ```kotlin
   .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
   ```
   - Drop frames if processing is slow
   - Maintain real-time performance

3. **Native Processing**
   - All heavy computation in C++
   - OpenCV optimized operations
   - SIMD instructions on ARM

4. **Hardware Acceleration**
   - OpenGL ES for rendering
   - GPU texture operations
   - No CPU-based drawing

## Error Handling

```kotlin
// Kotlin side
try {
    processImage(imageProxy)
} catch (e: Exception) {
    e.printStackTrace()
} finally {
    imageProxy.close()
}
```

```cpp
// C++ side
try {
    cv::Canny(blurred, edges, lowThreshold, highThreshold);
} catch (const cv::Exception& e) {
    LOGE("OpenCV exception: %s", e.what());
    outputData = inputData; // Fallback to original
}
```

## Build System

### Gradle (Android)
```
app/build.gradle → CMake → Native Libraries (.so)
                 → Kotlin/Java → APK
```

### CMake (Native)
```
CMakeLists.txt → Find OpenCV
               → Compile C++ sources
               → Link shared library
               → libedgedetection.so
```

### npm (Web)
```
package.json → TypeScript Compiler (tsc)
             → ES2020 modules
             → dist/index.js
```

## Module Dependencies

```
MainActivity
    ├── CameraX (androidx.camera)
    ├── NativeLib (JNI bridge)
    └── GLRenderer (OpenGL ES)

NativeLib (JNI)
    └── EdgeProcessor (C++)
            └── OpenCV C++ API

GLRenderer
    └── OpenGL ES 2.0

Web Viewer
    ├── TypeScript
    └── Canvas API
```

## Configuration Points

1. **Edge Detection Parameters** (`edge_processor.cpp`)
   - Low threshold: 50.0
   - High threshold: 150.0
   - Kernel size: 5

2. **Camera Resolution** (`MainActivity.kt`)
   - Default: Auto (CameraX decides)
   - Can be set with `.setTargetResolution()`

3. **OpenGL Viewport** (`GLRenderer.kt`)
   - Matches GLSurfaceView size
   - Auto-scaled in shader

## Web Viewer Edge Detection

### Client-Side Processing Pipeline

The web viewer implements its own edge detection using pure TypeScript:

```
Upload Image → Canvas → getImageData()
                 ↓
         Grayscale Conversion
         (0.299R + 0.587G + 0.114B)
                 ↓
         Sobel Operator
         (3×3 convolution kernels)
                 ↓
         Gradient Magnitude
         √(Gx² + Gy²)
                 ↓
         Thresholding (50)
                 ↓
         putImageData() → Canvas
                 ↓
         Display with stats
```

### Sobel Kernels

```
Gx (Horizontal):        Gy (Vertical):
[-1  0  1]              [-1 -2 -1]
[-2  0  2]              [ 0  0  0]
[-1  0  1]              [ 1  2  1]
```

### Performance

- **Small images** (640×480): ~15-25ms
- **Medium images** (800×600): ~25-40ms
- **Large images** (1920×1080): ~80-120ms

All processing happens in the browser, no server required!

## Future Enhancements

1. **WebSocket Integration**
   - Real-time frame streaming from Android to web viewer
   - Bi-directional communication

2. **Additional Filters**
   - Laplacian edge detection
   - Prewitt operator
   - Custom GLSL shaders for GPU acceleration

3. **Advanced Web Features**
   - Adjustable threshold slider
   - Multiple algorithm comparison
   - OpenCV.js integration for Canny

4. **Performance Profiling**
   - Frame timing breakdown
   - Memory usage tracking
   - GPU utilization metrics

## Technologies Summary

| Component | Technology | Version |
|-----------|------------|---------|
| UI Framework | Android SDK | API 24+ |
| Language | Kotlin | 1.9.20 |
| Camera | CameraX | 1.3.1 |
| Native | C++ | C++17 |
| Build (Native) | CMake | 3.22.1 |
| CV Library | OpenCV | 4.8.0+ |
| Graphics | OpenGL ES | 2.0 |
| Build (Android) | Gradle | 8.2 |
| Web | TypeScript | 5.3+ |
| Web Build | tsc | ES2020 |

## Testing Strategy

1. **Unit Tests**: Native C++ functions
2. **Integration Tests**: JNI bridge
3. **UI Tests**: Camera permissions, button clicks
4. **Performance Tests**: FPS benchmarks

## Deployment

```bash
# Debug build
./gradlew assembleDebug

# Release build (requires signing)
./gradlew assembleRelease

# Install on device
./gradlew installDebug
```
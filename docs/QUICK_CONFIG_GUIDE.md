# ⚡ Quick Configuration Guide - Edge Detection

## 🎯 Quick Setup

The enhanced edge detection works automatically! No configuration needed for most cases.

---

## 🔧 Adjusting Quality (Optional)

### In Your Activity (e.g., MainActivity.kt):

```kotlin
// After initializing OpenCV
NativeLib.initOpenCV()

// Set quality level (optional, default is 1)
NativeLib.setQualityLevel(1)  // 0=Fast, 1=Normal, 2=High
```

---

## 📊 Quality Levels Explained

### Level 0: Fast ⚡
- **Speed**: Fastest (~25-35ms per frame)
- **Quality**: Medium
- **Best for**: Low-end devices, real-time processing
- **Features**: CLAHE + Bilateral filter + Canny + Morphology

### Level 1: Normal ⭐ (DEFAULT)
- **Speed**: Balanced (~30-45ms per frame)
- **Quality**: High
- **Best for**: Most devices and scenarios
- **Features**: CLAHE + Sharpening + Bilateral filter + Canny + Morphology

### Level 2: High 🎯
- **Speed**: Slower (~40-60ms per frame)
- **Quality**: Very High
- **Best for**: High-end devices, best quality needed
- **Features**: CLAHE + Sharpening + Strong bilateral filter + Canny + Morphology

---

## 🎨 Fine-Tuning Edge Detection

### Adjust Canny Thresholds (in edge_processor.cpp)

Current optimal settings:
```cpp
// Line 148 in edge_processor.cpp
applyCannyEdgeDetection(inputMat, outputMat, 50.0, 150.0);
```

**For More Edges** (noisier but captures finer details):
```cpp
applyCannyEdgeDetection(inputMat, outputMat, 30.0, 100.0);
```

**For Fewer, Stronger Edges** (cleaner but may miss details):
```cpp
applyCannyEdgeDetection(inputMat, outputMat, 70.0, 200.0);
```

**For Very Clean Scenes** (outdoor, good lighting):
```cpp
applyCannyEdgeDetection(inputMat, outputMat, 80.0, 240.0);
```

---

## 🔍 CLAHE Adjustment (in edge_processor.cpp)

Current settings:
```cpp
// Line 13 in edge_processor.cpp
clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
```

**For Low-Light Scenes** (more aggressive):
```cpp
clahe = cv::createCLAHE(3.0, cv::Size(8, 8));
```

**For Well-Lit Scenes** (less aggressive):
```cpp
clahe = cv::createCLAHE(1.5, cv::Size(8, 8));
```

**For Global Enhancement**:
```cpp
clahe = cv::createCLAHE(2.0, cv::Size(16, 16));  // Larger tiles
```

---

## ⚙️ Bilateral Filter Adjustment

### Normal/Fast Mode (Line 55):
```cpp
cv::bilateralFilter(sharpened, filtered, 5, 50, 50);
```

**For Cleaner Results** (more blur, slower):
```cpp
cv::bilateralFilter(sharpened, filtered, 7, 75, 75);
```

**For Faster Processing** (less blur):
```cpp
cv::bilateralFilter(sharpened, filtered, 3, 40, 40);
```

### High Quality Mode (Line 52):
```cpp
cv::bilateralFilter(sharpened, filtered, 9, 75, 75);
```

---

## 🎭 Sharpening Adjustment (in applySharpen function)

Current settings:
```cpp
// Line 109 in edge_processor.cpp
cv::addWeighted(inputImage, 1.5, blurred, -0.5, 0, outputImage);
```

**Stronger Sharpening**:
```cpp
cv::addWeighted(inputImage, 1.8, blurred, -0.8, 0, outputImage);
```

**Lighter Sharpening**:
```cpp
cv::addWeighted(inputImage, 1.3, blurred, -0.3, 0, outputImage);
```

**Disable Sharpening** (set quality to 0):
```kotlin
NativeLib.setQualityLevel(0)
```

---

## 🔨 Morphology Adjustment

### Dilation (Line 117):
```cpp
cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
```

**Thicker Edges**:
```cpp
cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(4, 4));
```

**Thinner Edges**:
```cpp
cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
```

### Closing (Line 122):
```cpp
cv::Mat closeKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
```

**More Aggressive Gap Closing**:
```cpp
cv::Mat closeKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
```

---

## 📱 Adding UI Controls (Optional)

### Add Quality Selector in MainActivity.kt:

```kotlin
class MainActivity : AppCompatActivity() {
    
    private var currentQuality = 1 // Default: Normal
    
    private fun setupQualityButton() {
        binding.qualityButton.setOnClickListener {
            currentQuality = (currentQuality + 1) % 3
            NativeLib.setQualityLevel(currentQuality)
            
            val qualityName = when(currentQuality) {
                0 -> "Fast ⚡"
                1 -> "Normal ⭐"
                2 -> "High 🎯"
                else -> "Unknown"
            }
            
            Toast.makeText(this, "Quality: $qualityName", Toast.LENGTH_SHORT).show()
            binding.qualityText.text = "Quality: $qualityName"
        }
    }
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        NativeLib.initOpenCV()
        NativeLib.setQualityLevel(currentQuality)
        
        setupQualityButton() // Add this
        // ... rest of your setup
    }
}
```

### Add Button to Layout (activity_main.xml):

```xml
<Button
    android:id="@+id/qualityButton"
    android:layout_width="wrap_content"
    android:layout_height="wrap_content"
    android:text="Quality"
    android:layout_margin="8dp"/>

<TextView
    android:id="@+id/qualityText"
    android:layout_width="wrap_content"
    android:layout_height="wrap_content"
    android:text="Quality: Normal ⭐"
    android:textSize="16sp"/>
```

---

## 🎯 Preset Configurations

### Configuration A: Maximum Quality (Slowest)
```cpp
// In edge_processor.cpp
clahe = cv::createCLAHE(3.0, cv::Size(8, 8));
cv::addWeighted(inputImage, 1.8, blurred, -0.8, 0, outputImage);
cv::bilateralFilter(sharpened, filtered, 9, 100, 100);
cv::Canny(filtered, edges, 50.0, 150.0, 3, true);
```

```kotlin
NativeLib.setQualityLevel(2)
```

### Configuration B: Balanced (Recommended) ⭐
```cpp
// Current default settings - no changes needed!
```

```kotlin
NativeLib.setQualityLevel(1)
```

### Configuration C: Maximum Speed (Lowest quality)
```cpp
// In edge_processor.cpp
clahe = cv::createCLAHE(1.5, cv::Size(8, 8));
// Disable sharpening via quality level
cv::bilateralFilter(sharpened, filtered, 3, 40, 40);
cv::Canny(filtered, edges, 60.0, 180.0, 3, true);
```

```kotlin
NativeLib.setQualityLevel(0)
```

### Configuration D: Low-Light Scenes
```cpp
// In edge_processor.cpp
clahe = cv::createCLAHE(3.5, cv::Size(6, 6));  // Strong enhancement
cv::bilateralFilter(sharpened, filtered, 7, 75, 75);  // More noise reduction
cv::Canny(filtered, edges, 40.0, 120.0, 3, true);  // Lower thresholds
```

### Configuration E: High-Contrast Scenes
```cpp
// In edge_processor.cpp
clahe = cv::createCLAHE(1.5, cv::Size(8, 8));  // Less enhancement needed
cv::Canny(filtered, edges, 70.0, 210.0, 3, true);  // Higher thresholds
```

---

## 🔍 Debugging & Testing

### Enable Verbose Logging

Logs are automatically enabled! Check with:
```bash
adb logcat | grep EdgeProcessor
```

Sample output:
```
EdgeProcessor: EdgeProcessor initialized with enhanced preprocessing
EdgeProcessor: Edge detection complete - thresholds: 50.0/150.0, quality: 1
EdgeProcessor: Frame processed in 35.24 ms (quality level: 1)
```

### Performance Monitoring

Check processing time:
```kotlin
val processingTime = NativeLib.getProcessingTime()
Log.d("Performance", "Edge detection: ${processingTime}ms")
```

---

## 📊 Comparison Chart

| Scenario | CLAHE clipLimit | Canny Low/High | Quality Level | Expected FPS |
|----------|----------------|----------------|---------------|--------------|
| Indoor, normal light | 2.0 | 50/150 | 1 | 20-25 |
| Outdoor, bright | 1.5 | 70/210 | 1 | 20-25 |
| Low light | 3.0 | 40/120 | 2 | 15-20 |
| Maximum speed | 1.5 | 60/180 | 0 | 25-30 |
| Maximum quality | 3.0 | 50/150 | 2 | 15-20 |

---

## ⚠️ Important Notes

1. **Always rebuild** after changing C++ code:
   ```bash
   ./gradlew clean build
   ```

2. **Test on actual device** for accurate performance metrics

3. **Quality level** can be changed at runtime without rebuild

4. **Default settings work well** for 90% of scenarios

5. **Profile before optimizing** - measure actual performance

---

## 🚀 Quick Commands

```bash
# Build and install
./gradlew clean assembleDebug installDebug

# Check logs
adb logcat | grep -E '(EdgeProcessor|OpenCV)'

# Monitor FPS
adb logcat | grep "Frame processed"

# Test on device
adb shell am start -n com.flam.edgedetection/.MainActivity
```

---

## 📞 Need Help?

If edges are still not satisfactory:

1. **Check lighting** - CLAHE helps but can't fix completely dark images
2. **Verify camera focus** - Blurry images won't have clear edges
3. **Try different quality levels** - Use level 2 for best results
4. **Adjust thresholds** - Lower for more edges, higher for cleaner
5. **Check resolution** - Higher resolution = better edge detail

---

*Last updated: 2024 - Enhanced Edge Detection v2.0*
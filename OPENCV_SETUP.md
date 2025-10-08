# OpenCV Setup Guide

The project requires OpenCV for Android. Here are detailed instructions for setting it up.

## Quick Setup (Recommended)

### Method 1: Gradle Dependency (Easiest)

This is the simplest method but results in a larger APK size (~50MB).

1. **Remove OpenCV module reference** from `settings.gradle`:
   ```gradle
   // Comment out or remove this line
   // include ':opencv'
   ```

2. **Remove OpenCV dependency** from `app/build.gradle`:
   ```gradle
   dependencies {
       // Remove this line
       // implementation project(':opencv')
   }
   ```

3. **Add OpenCV Maven dependency** to `app/build.gradle`:
   ```gradle
   dependencies {
       implementation 'com.quickbirdstudios:opencv:4.5.3.0'
   }
   ```

4. **Update CMakeLists.txt** to use the Gradle OpenCV:
   ```cmake
   # Comment out or remove the manual OpenCV path
   # set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv/sdk/native/jni)
   
   # OpenCV will be found automatically from Gradle
   find_package(OpenCV REQUIRED)
   ```

5. **Sync and build**:
   ```bash
   ./gradlew clean
   ./gradlew assembleDebug
   ```

## Method 2: Manual OpenCV SDK (Smaller APK)

This method requires manual download but gives you more control and smaller APK size.

### Step 1: Download OpenCV

1. Go to https://opencv.org/releases/
2. Download **OpenCV Android** package (opencv-4.8.0-android-sdk.zip or later)
3. Extract the ZIP file

### Step 2: Install OpenCV in Project

You have two sub-options:

#### Option A: Copy Native Libraries

```bash
# Create directory
mkdir -p app/src/main/cpp/opencv/sdk/native

# Copy from downloaded SDK
cp -r /path/to/OpenCV-android-sdk/sdk/native/* app/src/main/cpp/opencv/sdk/native/

# Verify structure
ls app/src/main/cpp/opencv/sdk/native/jni/
# Should see: abi-armeabi-v7a, abi-arm64-v8a, etc.
```

The `CMakeLists.txt` is already configured for this structure.

#### Option B: Import as Module

1. **Copy SDK to project**:
   ```bash
   cp -r /path/to/OpenCV-android-sdk/sdk opencv/
   ```

2. **Update `opencv/build.gradle`**:
   ```gradle
   plugins {
       id 'com.android.library'
   }

   android {
       namespace 'org.opencv'
       compileSdk 34

       defaultConfig {
           minSdk 24
           sourceSets {
               main {
                   jniLibs.srcDirs = ['sdk/native/libs']
                   java.srcDirs = ['sdk/java/src']
                   aidl.srcDirs = ['sdk/java/src']
                   res.srcDirs = ['sdk/java/res']
                   manifest.srcFile 'sdk/java/AndroidManifest.xml'
               }
           }
       }
   }
   ```

3. **Update app/build.gradle**:
   ```gradle
   dependencies {
       implementation project(':opencv')
   }
   ```

4. **Update CMakeLists.txt**:
   ```cmake
   set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../opencv/sdk/native/jni)
   find_package(OpenCV REQUIRED)
   ```

### Step 3: Verify Installation

Build the project:
```bash
./gradlew clean
./gradlew assembleDebug
```

If successful, you'll see:
```
BUILD SUCCESSFUL
```

Check that native libraries are included:
```bash
unzip -l app/build/outputs/apk/debug/app-debug.apk | grep libopencv
```

Should show:
```
lib/arm64-v8a/libopencv_java4.so
lib/armeabi-v7a/libopencv_java4.so
```

## Troubleshooting

### Error: "OpenCV not found"

**Symptoms**:
```
CMake Error: Could not find OpenCV
```

**Solutions**:
1. Verify OpenCV path in CMakeLists.txt:
   ```cmake
   set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv/sdk/native/jni)
   ```

2. Check directory structure:
   ```bash
   ls app/src/main/cpp/opencv/sdk/native/jni/
   ```
   Should contain `OpenCVConfig.cmake` or similar.

3. Try absolute path temporarily:
   ```cmake
   set(OpenCV_DIR "/absolute/path/to/opencv/sdk/native/jni")
   ```

### Error: "Undefined reference to cv::"

**Symptoms**:
```
undefined reference to `cv::Canny(...)`
```

**Solutions**:
1. Check library linking in CMakeLists.txt:
   ```cmake
   target_link_libraries(
       edgedetection
       ${OpenCV_LIBS}  # This line is crucial
       android
       log
       jnigraphics
   )
   ```

2. Verify OpenCV was found:
   Add to CMakeLists.txt:
   ```cmake
   message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
   message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
   ```

### Error: "UnsatisfiedLinkError"

**Symptoms**:
```
java.lang.UnsatisfiedLinkError: dlopen failed: library "libopencv_java4.so" not found
```

**Solutions**:
1. Add OpenCV libs to APK:
   ```gradle
   android {
       sourceSets {
           main {
               jniLibs.srcDirs += ['opencv/sdk/native/libs']
           }
       }
   }
   ```

2. Or ensure Gradle dependency includes native libs:
   ```gradle
   dependencies {
       implementation 'com.quickbirdstudios:opencv:4.5.3.0'
   }
   ```

### Build is Slow

**Solutions**:
1. Reduce target ABIs in `app/build.gradle`:
   ```gradle
   ndk {
       abiFilters 'arm64-v8a'  // Only 64-bit ARM
   }
   ```

2. Enable Gradle caching:
   ```bash
   ./gradlew assembleDebug --build-cache
   ```

## Testing OpenCV Installation

Create a simple test in MainActivity:
```kotlin
override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    
    // Test OpenCV
    if (NativeLib.initOpenCV()) {
        Log.i("OpenCV", "OpenCV initialized successfully!")
    } else {
        Log.e("OpenCV", "OpenCV initialization failed!")
    }
}
```

Run the app and check Logcat:
```bash
adb logcat | grep OpenCV
```

## Version Compatibility

| OpenCV Version | Min SDK | Recommended |
|----------------|---------|-------------|
| 4.8.0 | API 24 | ✅ |
| 4.5.3 | API 21 | ✅ |
| 4.x | API 21+ | ✅ |
| 3.x | API 15+ | ⚠️ (deprecated) |

## APK Size Impact

| Method | APK Size | Pros | Cons |
|--------|----------|------|------|
| Gradle (all ABIs) | ~50MB | Easy setup | Large size |
| Gradle (single ABI) | ~15MB | Easy setup | One architecture |
| Manual (all ABIs) | ~40MB | More control | Manual setup |
| Manual (single ABI) | ~12MB | Smallest size | Limited devices |

## Alternative: OpenCV Manager (Legacy)

**Not recommended** for new projects, but if you want to try:

1. User installs OpenCV Manager from Play Store
2. App uses shared OpenCV library
3. Smaller APK (~5MB)

Implementation:
```kotlin
if (!OpenCVLoader.initDebug()) {
    // Prompt user to install OpenCV Manager
}
```

## Recommended Configuration

For this project, we recommend:

```gradle
// app/build.gradle
android {
    ndk {
        // Target most common devices
        abiFilters 'arm64-v8a', 'armeabi-v7a'
    }
}

dependencies {
    // Use Gradle dependency for simplicity
    implementation 'com.quickbirdstudios:opencv:4.5.3.0'
}
```

This balances APK size (~25MB) with ease of setup and device compatibility.

## Resources

- [OpenCV Android Documentation](https://docs.opencv.org/4.x/d0/d6c/tutorial_android_dev_intro.html)
- [OpenCV Releases](https://opencv.org/releases/)
- [CMake with OpenCV](https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html)
- [Android NDK Guide](https://developer.android.com/ndk/guides)

## Need Help?

1. Check CMake output in Android Studio: View → Tool Windows → Build
2. Check Logcat for native errors: `adb logcat | grep -E '(OpenCV|FATAL)'`
3. Clean and rebuild: `./gradlew clean build`
4. Invalidate caches: Android Studio → File → Invalidate Caches / Restart
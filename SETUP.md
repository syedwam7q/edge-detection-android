# Setup Guide

## Quick Start

### 1. OpenCV Installation

The project requires OpenCV Android SDK. You have two options:

#### Option A: Download OpenCV SDK (Recommended)

1. Visit https://opencv.org/releases/
2. Download OpenCV Android SDK (4.8.0 or later)
3. Extract the archive
4. Create OpenCV module structure:

```bash
mkdir -p app/src/main/cpp/opencv/sdk/native
cp -r /path/to/OpenCV-android-sdk/sdk/native/* app/src/main/cpp/opencv/sdk/native/
```

#### Option B: Use Gradle Dependency (Simpler but larger APK)

Modify `app/build.gradle`:
```gradle
dependencies {
    // Add this line
    implementation 'org.opencv:opencv-android:4.8.0'
}
```

And update `app/build.gradle` to remove OpenCV configuration from CMake.

### 2. Build Android App

```bash
# Open in Android Studio
android-studio .

# Or build from command line
./gradlew clean
./gradlew assembleDebug
```

### 3. Install on Device

```bash
# Via USB
./gradlew installDebug

# Or drag APK to emulator
```

### 4. Setup Web Viewer

```bash
cd web
npm install
npm run build
npm run serve
```

Open http://localhost:8000

## Troubleshooting

### Issue: OpenCV not found

**Solution**: 
- Ensure OpenCV SDK path is correct in `app/src/main/cpp/CMakeLists.txt`
- Check that `OpenCV_DIR` points to the correct location
- Verify native libraries are in the right place

### Issue: CMake errors

**Solution**:
- Update CMake to 3.22.1 or later in Android Studio SDK Manager
- Clean and rebuild: `./gradlew clean build`

### Issue: Camera permission denied

**Solution**:
- Grant camera permission in device Settings → Apps → Edge Detection
- Or uninstall and reinstall the app

### Issue: Low FPS

**Solution**:
- Reduce camera resolution in MainActivity
- Test on a physical device instead of emulator
- Optimize edge detection parameters

### Issue: TypeScript compilation errors

**Solution**:
```bash
cd web
rm -rf node_modules dist
npm install
npm run build
```

## Device Requirements

- Android 7.0 (API 24) or higher
- Camera support
- OpenGL ES 2.0 support
- Recommended: 2GB+ RAM

## Build Variants

### Debug Build
```bash
./gradlew assembleDebug
```

### Release Build
```bash
./gradlew assembleRelease
```

Note: Release builds require signing configuration.

## Development Tips

1. **Hot Reload**: Use Android Studio's "Apply Changes" for faster iteration
2. **Logging**: Check Logcat for native crashes (filter by "EdgeProcessor" or "NativeLib")
3. **Performance**: Use Android Profiler to monitor CPU/Memory usage
4. **Debugging Native Code**: Attach LLDB debugger for C++ debugging

## Next Steps

After successful setup:
1. Grant camera permission
2. Toggle between raw and edge detection modes
3. Check FPS counter
4. Export frames and view in web viewer
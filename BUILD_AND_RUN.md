# 🔨 Build and Run Guide

Complete guide to build, run, and test the Edge Detection application.

---

## 📋 Prerequisites

Before building, ensure you have:

### 1. Java Development Kit (JDK) 17+

**Check if installed:**
```bash
java -version
```

**Install if needed:**

**macOS:**
```bash
# Using Homebrew
brew install openjdk@17

# Add to PATH
echo 'export PATH="/opt/homebrew/opt/openjdk@17/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

**Or download from:** https://adoptium.net/

### 2. Android Studio (Recommended)

**Download:** https://developer.android.com/studio

**Includes:**
- Android SDK
- Android NDK
- CMake
- Emulator
- All build tools

**Installation:**
1. Download Android Studio
2. Install and open
3. Go through setup wizard
4. Install Android SDK API 34
5. Install Android NDK (Tools → SDK Manager → SDK Tools → NDK)
6. Install CMake (same location)

### 3. Android Device or Emulator

**Physical Device:**
- Enable Developer Options
- Enable USB Debugging
- Connect via USB

**Emulator:**
- Create in Android Studio (Tools → Device Manager)
- Recommended: Pixel 6 with API 34

---

## 🚀 Method 1: Using Android Studio (Easiest)

### Step 1: Open Project

1. Launch Android Studio
2. Click **"Open"**
3. Navigate to project folder
4. Select the root directory containing `build.gradle`
5. Click **"OK"**

### Step 2: Sync Project

Android Studio will automatically:
- Download Gradle dependencies
- Download OpenCV library
- Configure NDK/CMake
- Index the project

**Wait for "Gradle Sync" to complete** (check bottom status bar)

### Step 3: Build Project

```
Build → Make Project
```

Or press: `⌘ + F9` (Mac) / `Ctrl + F9` (Windows/Linux)

**Expected output:**
```
BUILD SUCCESSFUL in 2m 30s
```

### Step 4: Run on Device

1. Connect device or start emulator
2. Select device from dropdown (top toolbar)
3. Click **Run** button (green play icon)

Or press: `⌃ + R` (Mac) / `Shift + F10` (Windows/Linux)

### Step 5: Grant Camera Permission

When app launches:
1. Tap **"Allow"** for camera permission
2. App will show camera feed
3. Use toggle button to switch modes

---

## 🖥️ Method 2: Command Line (Advanced)

### Step 1: Install Java

See Prerequisites section above.

**Verify:**
```bash
java -version
# Should show: openjdk version "17.0.x" or higher
```

### Step 2: Set ANDROID_HOME

**Find SDK location:**
```bash
# Default locations:
# macOS: ~/Library/Android/sdk
# Linux: ~/Android/Sdk
# Windows: C:\Users\<user>\AppData\Local\Android\Sdk
```

**Set environment variable:**

**macOS/Linux:**
```bash
export ANDROID_HOME=~/Library/Android/sdk
export PATH=$PATH:$ANDROID_HOME/emulator
export PATH=$PATH:$ANDROID_HOME/platform-tools
export PATH=$PATH:$ANDROID_HOME/cmdline-tools/latest/bin
```

Add to `~/.zshrc` or `~/.bashrc` to make permanent.

**Windows (PowerShell):**
```powershell
$env:ANDROID_HOME = "C:\Users\<your-user>\AppData\Local\Android\Sdk"
$env:PATH += ";$env:ANDROID_HOME\emulator"
$env:PATH += ";$env:ANDROID_HOME\platform-tools"
```

### Step 3: Install Android SDK Components

```bash
# Using sdkmanager
sdkmanager "platform-tools" "platforms;android-34" "build-tools;34.0.0" "ndk;25.2.9519653" "cmake;3.22.1"
```

### Step 4: Build with Gradle

Navigate to project directory:
```bash
cd "/path/to/edge-detection"
```

**Clean build:**
```bash
./gradlew clean
```

**Build debug APK:**
```bash
./gradlew assembleDebug
```

**Build release APK:**
```bash
./gradlew assembleRelease
```

**Expected output:**
```
BUILD SUCCESSFUL in 3m 15s
45 actionable tasks: 45 executed
```

**APK Location:**
```
app/build/outputs/apk/debug/app-debug.apk
```

### Step 5: Install on Device

**Connect device and verify:**
```bash
adb devices
# Should show your device
```

**Install APK:**
```bash
./gradlew installDebug
```

**Or manually:**
```bash
adb install app/build/outputs/apk/debug/app-debug.apk
```

### Step 6: Launch App

```bash
adb shell am start -n com.flam.edgedetection/.MainActivity
```

### Step 7: View Logs

```bash
adb logcat | grep -E "(EdgeDetection|OpenCV|EdgeProcessor)"
```

---

## 📸 Capturing Screenshots

### Method 1: Android Studio

1. Run app on device/emulator
2. Click **Camera** icon in left toolbar (or View → Tool Windows → Running Devices)
3. Click **Screenshot** button
4. Save to `screenshots/` folder

### Method 2: ADB Command

```bash
# Take screenshot
adb shell screencap -p /sdcard/screenshot.png

# Pull to computer
adb pull /sdcard/screenshot.png screenshots/app_screenshot.png

# Clean up
adb shell rm /sdcard/screenshot.png
```

### Method 3: Device Buttons

**Most devices:**
- Press `Power + Volume Down` simultaneously
- Screenshot saved to gallery
- Transfer via USB

### Method 4: Recording GIF

**Using scrcpy (recommended):**
```bash
# Install scrcpy
brew install scrcpy  # macOS
# or download from: https://github.com/Genymobile/scrcpy

# Record screen
scrcpy --record=demo.mp4

# Convert to GIF
ffmpeg -i demo.mp4 -vf "fps=10,scale=360:-1:flags=lanczos" demo.gif
```

---

## 🧪 Testing the App

### Test Checklist

- [ ] **App launches without crashes**
- [ ] **Camera permission dialog appears**
- [ ] **Camera feed displays** after permission granted
- [ ] **FPS counter visible** at top
- [ ] **Toggle button works** (switches between raw and edge)
- [ ] **Edge detection active** - see edges highlighted
- [ ] **Performance acceptable** - 15+ FPS shown
- [ ] **No memory leaks** - app doesn't slow down over time
- [ ] **Rotation works** - landscape and portrait modes
- [ ] **App resumes correctly** - after minimizing

### Performance Testing

**Check FPS:**
- Look at on-screen counter
- Target: 15-30 FPS
- Lower FPS on older devices is expected

**Check logs:**
```bash
adb logcat | grep "EdgeProcessor"
```

**Expected output:**
```
I/EdgeProcessor: Processing frame: 1920x1080, took 23ms
I/EdgeProcessor: Processing frame: 1920x1080, took 21ms
```

**Check memory usage:**
```bash
adb shell dumpsys meminfo com.flam.edgedetection
```

### Testing Edge Detection Quality

1. Point camera at objects with clear edges:
   - Books
   - Doors/windows
   - Computer screens
   - Printed text

2. Toggle between raw and edge modes
3. Verify edges are detected clearly
4. Adjust lighting if needed (brighter = better)

---

## 🎨 Building Web Viewer

### Step 1: Install Node.js

**Check if installed:**
```bash
node --version
npm --version
```

**Install if needed:**
```bash
# macOS
brew install node

# Or download from: https://nodejs.org/
```

### Step 2: Build Web Viewer

```bash
cd web
npm install
npm run build
```

**Expected output:**
```
> edge-detection-viewer@1.0.0 build
> tsc

✨  Done
```

**Output files:**
```
web/dist/index.js
web/dist/index.d.ts
```

### Step 3: Serve Web Viewer

```bash
npm run serve
```

**Or manually:**
```bash
# Using Python
python3 -m http.server 8000

# Using Node
npx http-server . -p 8000
```

**Open:** http://localhost:8000

### Step 4: Test Web Viewer

1. Click **"Generate Sample Frame"**
2. Should see a test frame with edge detection
3. Statistics should show at bottom:
   - FPS: 30
   - Resolution: 640x480
   - Processing Time: 25ms

---

## 🐛 Troubleshooting

### Issue: Gradle sync failed

**Error:**
```
Could not resolve all dependencies
```

**Solution:**
```bash
# Clear Gradle cache
./gradlew clean --refresh-dependencies

# Or delete cache manually
rm -rf ~/.gradle/caches/
```

### Issue: CMake can't find OpenCV

**Error:**
```
CMake Error: Could not find OpenCV
```

**Solution:**

The project is configured to use OpenCV from Gradle. Verify in `app/build.gradle`:
```gradle
dependencies {
    implementation 'com.quickbirdstudios:opencv:4.5.3.0'
}
```

If using manual OpenCV, see [OPENCV_SETUP.md](OPENCV_SETUP.md)

### Issue: Build fails with "NDK not found"

**Solution:**

Install NDK via Android Studio:
1. Tools → SDK Manager
2. SDK Tools tab
3. Check "NDK (Side by side)"
4. Click "Apply"

Or download manually: https://developer.android.com/ndk/downloads

### Issue: App crashes on launch

**Check logs:**
```bash
adb logcat | grep -E "(AndroidRuntime|FATAL)"
```

**Common causes:**
1. Camera permission not granted
2. OpenCV native lib not loaded
3. Device API level < 24

**Solution:**
- Ensure `minSdk 24` in `app/build.gradle`
- Grant camera permission in settings
- Check device Android version

### Issue: Camera permission denied

**Solution:**

**On device:**
```
Settings → Apps → Edge Detection → Permissions → Camera → Allow
```

**Or via ADB:**
```bash
adb shell pm grant com.flam.edgedetection android.permission.CAMERA
```

### Issue: Black screen instead of camera

**Possible causes:**
1. Permission not granted
2. Camera in use by another app
3. Emulator without camera support

**Solution:**
- Use physical device
- Or configure emulator camera: AVD Manager → Edit → Camera → Webcam

### Issue: Low FPS (<10)

**Causes:**
- Old device
- High resolution
- Debug build (not optimized)

**Solutions:**

1. Build release version:
   ```bash
   ./gradlew assembleRelease
   ```

2. Reduce ABIs to device-specific:
   ```gradle
   ndk {
       abiFilters 'arm64-v8a'  // Only your device architecture
   }
   ```

3. Lower camera resolution (edit in `MainActivity.kt`)

### Issue: "Execution failed for task ':app:externalNativeBuildDebug'"

**Solution:**

Ensure CMake and NDK are installed:
```bash
sdkmanager --list | grep -E "(ndk|cmake)"
```

Install if missing:
```bash
sdkmanager "cmake;3.22.1" "ndk;25.2.9519653"
```

---

## 📊 Expected Build Times

| Hardware | Clean Build | Incremental Build |
|----------|-------------|-------------------|
| M1/M2 Mac | 2-3 minutes | 15-30 seconds |
| Intel Mac | 3-5 minutes | 30-60 seconds |
| Windows (Modern) | 3-6 minutes | 30-90 seconds |
| Linux (Modern) | 2-4 minutes | 20-45 seconds |

**First build is slower** due to:
- Gradle dependency downloads (~200MB)
- OpenCV library download (~50MB)
- NDK toolchain setup
- CMake configuration

---

## 📦 APK Size

**Debug build:** ~50-60 MB
- Includes debugging symbols
- All ABIs (armeabi-v7a, arm64-v8a, x86, x86_64)
- OpenCV libraries

**Release build (optimized):** ~35-45 MB
- Stripped symbols
- ProGuard optimization
- Compressed resources

**To reduce size:**
1. Target single ABI (reduces ~15MB)
2. Enable ProGuard/R8 (reduces ~5-10MB)
3. Use WebP for images (if any)

---

## 🎯 Quick Start Summary

**Fastest path to running:**

1. **Install Android Studio** → https://developer.android.com/studio
2. **Open project** in Android Studio
3. **Wait for Gradle sync**
4. **Connect device** or start emulator
5. **Click Run** button (green play icon)
6. **Grant camera permission**
7. **App runs!**

**For screenshots:**
1. Run app
2. Use Android Studio screenshot tool
3. Save to `screenshots/` folder

**For web viewer:**
1. `cd web && npm install && npm run build`
2. `npm run serve`
3. Open http://localhost:8000

---

## 📚 Additional Resources

- **Android Developer Docs:** https://developer.android.com/docs
- **OpenCV Android Tutorial:** https://docs.opencv.org/4.x/d0/d6c/tutorial_android_dev_intro.html
- **Gradle Build Guide:** https://docs.gradle.org/current/userguide/userguide.html
- **ADB Reference:** https://developer.android.com/studio/command-line/adb

---

## 💡 Tips for Demo

1. **Test on multiple devices** if available (different performance characteristics)
2. **Capture video** - more impressive than screenshots
3. **Show toggle functionality** - demonstrates both raw and processed feeds
4. **Point out FPS counter** - shows real-time performance
5. **Have good lighting** - edge detection works best with proper lighting
6. **Prepare fallback** - have screenshots ready in case live demo has issues

---

## ✅ Verification Checklist

Before submission, verify:

- [ ] Project builds without errors
- [ ] App installs and launches
- [ ] Camera feed displays
- [ ] Edge detection works
- [ ] Toggle button functions
- [ ] FPS counter shows
- [ ] No crashes during normal use
- [ ] Web viewer builds and runs
- [ ] Screenshots captured
- [ ] README updated with actual screenshots

---

**Need help?** Check [SETUP.md](SETUP.md), [OPENCV_SETUP.md](OPENCV_SETUP.md), or [ARCHITECTURE.md](ARCHITECTURE.md)

**Ready to submit?** See [FINAL_SUMMARY.md](FINAL_SUMMARY.md)
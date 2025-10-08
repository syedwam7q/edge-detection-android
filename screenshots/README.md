# Screenshots

This folder should contain:

1. `demo.gif` - Animated demo of the app in action
2. `raw_feed.png` - Screenshot of raw camera feed
3. `edge_detection.png` - Screenshot of edge detection mode
4. `web_viewer.png` - Screenshot of the web viewer

## How to capture screenshots

### Android App
1. Run the app on a device
2. Press Volume Down + Power button simultaneously
3. Transfer screenshots from device to this folder

### Web Viewer
1. Open web viewer in browser
2. Use browser's screenshot tool or Cmd+Shift+4 (Mac) / PrtScn (Windows)
3. Save to this folder

## Creating Demo GIF

Use screen recording tools like:
- **Android**: ADB screen record
  ```bash
  adb shell screenrecord /sdcard/demo.mp4
  adb pull /sdcard/demo.mp4
  ```
- **Mac**: QuickTime Player or Kap
- **Windows**: Windows Game Bar (Win + G)

Then convert to GIF using:
- [ezgif.com](https://ezgif.com/)
- `ffmpeg` command-line tool
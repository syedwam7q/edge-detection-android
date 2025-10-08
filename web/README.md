# Edge Detection Web Viewer

A TypeScript-based web viewer for displaying edge-detected frames from the Android application.

## Features

- 📷 Display processed frames from Android app
- 📊 Real-time frame statistics (FPS, resolution, processing time)
- 📁 Upload and view custom images
- 🎨 Clean, responsive UI
- 🔄 Sample frame generator for testing

## Setup Instructions

1. Install dependencies:
```bash
npm install
```

2. Build TypeScript:
```bash
npm run build
```

3. Serve the web page:
```bash
npm run serve
```

4. Open browser and navigate to:
```
http://localhost:8000
```

## Development

- Watch mode (auto-rebuild on changes):
```bash
npm run watch
```

## Architecture

- **TypeScript**: Type-safe DOM manipulation and frame handling
- **Canvas API**: Hardware-accelerated frame rendering
- **Modular Design**: Clean separation of concerns

## Integration with Android

To receive frames from the Android app:

1. Export processed frames as images from the Android app
2. Use the "Upload Image" button to load them
3. Or implement WebSocket/HTTP endpoint for real-time streaming (bonus feature)

## Technologies

- TypeScript 5.3+
- HTML5 Canvas
- Modern ES2020 features
- CSS Grid & Flexbox
## QT Webcam

Elegant Qt 6 webcam viewer with real‑time GLSL filters, live previews, and one‑click capture. Designed with a polished dark UI, toolbar actions, and a focused workflow.

### Features
- **Live webcam preview** rendered via OpenGL (`VideoGLWidget`)
- **Real‑time filters** (GLSL fragment shaders in `assets/filters`)
- **Clickable filter previews** with horizontal scroll gallery
- **One‑click capture** and preview dialog
- **Modern toolbar** with Capture/ Quit and shortcuts
- **Dark theme** and status bar updates (current filter, capture feedback)

### Project layout
- `mainwindow.ui` – base layout (video area + side controls)
- `mainwindow.cpp/.h` – UI wiring, toolbar, theming, filter previews
- `video_glwidget.cpp/.h` – OpenGL video widget with shader filters
- `cameracapture.cpp/.h` – Qt Multimedia capture session
- `capturedialog.cpp/.h` – image preview after capture
- `assets/filters/*.frag` – GLSL fragment shaders (filters)
- `assets.qrc` – Qt resource file embedding assets
- `qt-webcam.pro` – qmake project file

### Requirements
- Qt 6.5+ (tested on Qt 6.9)
  - Modules: Core, Gui, Widgets, Multimedia, OpenGL
- C++17 compiler
- Windows: MinGW (or MSVC) via Qt, macOS: Clang, Linux: GCC/Clang

### Build (Qt Creator)
1. Open `qt-webcam.pro` in Qt Creator
2. Select a Qt 6 kit (e.g., MinGW 64‑bit on Windows)
3. Build and Run

### Build (qmake CLI)

Windows (PowerShell, Qt MinGW environment):
```powershell
qmake
mingw32-make -j
./build/Desktop_Qt_6_9_2_MinGW_64_bit-Debug/debug/qt-webcam.exe
```

Linux/macOS:
```bash
qmake
make -j
./build/Desktop_Qt_6_9_2-Debug/qt-webcam
```

Note: Output paths vary by kit and configuration.

### Run
On first launch, the default camera is opened automatically. If no camera is found, a message is logged to the console.

### Usage
- Click a filter preview to apply it to the main view
- Press the red round button or use the toolbar Capture to grab a frame
- A preview dialog will open with the captured image

### Keyboard shortcuts
- **Space**: Capture
- **Ctrl+Q** (or platform Quit shortcut): Quit

### Theming
A polished dark theme is applied in `MainWindow::applyElegantStyle()`. You can customize colors by editing that function in `mainwindow.cpp`.

### Adding new filters
1. Create a GLSL fragment shader in `assets/filters/` (e.g., `myfilter.frag`)
2. Add it to `assets.qrc`
3. Extend `VideoGLWidget::FilterType` and its loading logic
4. Add a preview item for it in `mainwindow.cpp`

### Troubleshooting
- If video is black: ensure a camera device is available in Qt (`QMediaDevices::videoInputs()`)
- On Windows, run from the Qt‑provided terminal so required DLLs are on PATH
- Verify your kit uses Qt 6 and includes the Multimedia module

### License
Add your preferred license here (e.g., MIT).



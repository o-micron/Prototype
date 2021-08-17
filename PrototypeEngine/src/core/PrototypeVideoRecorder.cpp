/// Copyright 2021 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "PrototypeVideoRecorder.h"

#if defined(PROTOTYPE_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int
myErrorHandler(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void*);

PrototypeVideoRecorder::PrototypeVideoRecorder()
  : _isRecording(PrototypeVideoRecordingType_None)
  , _desktopHwnd(nullptr)
  , _threadpool(1)
{}

PrototypeVideoRecorder::~PrototypeVideoRecorder()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    // block until writing is finished
    _threadpool.waitForWork();

    if (_isRecording == PrototypeVideoRecordingType_Framebuffer) {
        stopRecordingFramebuffer();
    } else if (_isRecording == PrototypeVideoRecordingType_Fullscreen) {
        stopRecordingDesktop();
    }
#endif
}

const PrototypeVideoRecordingType_&
PrototypeVideoRecorder::isRecording()
{
    return _isRecording;
}

void
PrototypeVideoRecorder::updateFrame(std::function<void(void** data)>&& fn)
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    if (_isRecording == PrototypeVideoRecordingType_Fullscreen) {
        // copy from the window device context to the bitmap device context
        StretchBlt((HDC)_hwindowCompatibleDC,
                   _outputVideoCoordinates.x,
                   _outputVideoCoordinates.y,
                   _outputVideoSize.x,
                   _outputVideoSize.y,
                   (HDC)_hwindowDC,
                   _screenSize.x,
                   _screenSize.y,
                   _outputVideoSize.x,
                   _outputVideoSize.y,
                   SRCCOPY); // change SRCCOPY to NOTSRCCOPY for wacky colors !
        GetDIBits((HDC)_hwindowCompatibleDC,
                  (HBITMAP)_hbwindow,
                  0,
                  _outputVideoSize.y,
                  _recordingFrame.data,
                  (BITMAPINFO*)&_bitmapInfoHeader,
                  DIB_RGB_COLORS); // copy from hwindowCompatibleDC to hbwindow

    } else if (_isRecording == PrototypeVideoRecordingType_Framebuffer) {
        fn((void**)&_recordingFrame2.data);
        cv::flip(_recordingFrame2, _recordingFrame, 0);
    }
#endif
}

void
PrototypeVideoRecorder::commitFrame()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    if (_isRecording == PrototypeVideoRecordingType_Fullscreen || _isRecording == PrototypeVideoRecordingType_Framebuffer) {
        _outputVideo.operator<<(_recordingFrame);
    }
#endif
}

void
PrototypeVideoRecorder::startRecordingFramebuffer(const char* filepath, i32 width, i32 height)
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    if (_isRecording != PrototypeVideoRecordingType_None) return;

    switchStderr();
    cv::setBreakOnError(false);
    cv::redirectError(myErrorHandler);
    _outputVideoSize = glm::vec2(width, height);
    _recordingFrame  = cv::Mat(_outputVideoSize.y, _outputVideoSize.x, CV_8UC3);
    _recordingFrame2 = cv::Mat(_outputVideoSize.y, _outputVideoSize.x, CV_8UC3);
    _isRecording     = PrototypeVideoRecordingType_Framebuffer;
    try {
        _outputVideo = cv::VideoWriter();
        _outputVideo.open(
          filepath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, cv::Size(_outputVideoSize.x, _outputVideoSize.y), true);
    } catch (...) {}
#endif
}

void
PrototypeVideoRecorder::stopRecordingFramebuffer()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    if (_isRecording != PrototypeVideoRecordingType_Framebuffer) return;

    // block until writing is finished
    _threadpool.waitForWork();

    _outputVideoBuffer.push_back(std::move(_outputVideo));
    _outputVideo = {};
    while (!_outputVideoBuffer.empty()) {
        try {
            auto video = std::move(_outputVideoBuffer.back());
            _outputVideoBuffer.pop_back();
            video.release();
        } catch (...) {}
    }
    revertStderr();
    cv::redirectError(NULL);
    _isRecording = PrototypeVideoRecordingType_None;
#endif
}

void
PrototypeVideoRecorder::startRecordingDesktop(const char* filepath, i32 x, i32 y, i32 width, i32 height)
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    if (_isRecording != PrototypeVideoRecordingType_None) return;

    // capture image
    _desktopHwnd = GetDesktopWindow();

    // get handles to a device context (DC)
    _hwindowDC           = GetDC((HWND)_desktopHwnd);
    _hwindowCompatibleDC = CreateCompatibleDC((HDC)_hwindowDC);
    SetStretchBltMode((HDC)_hwindowCompatibleDC, COLORONCOLOR);

    // define scale, height and width
    int scale     = 1;
    _screenSize.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    _screenSize.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    if (x <= -1) {
        _outputVideoCoordinates.x = 0;
    } else {
        _outputVideoCoordinates.x = x;
    }
    if (y <= -1) {
        _outputVideoCoordinates.y = 0;
    } else {
        _outputVideoCoordinates.y = y;
    }
    if (width <= -1) {
        _outputVideoSize.x = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    } else {
        _outputVideoSize.x = width;
    }
    if (height <= -1) {
        _outputVideoSize.y = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    } else {
        _outputVideoSize.y = height;
    }

    _recordingFrame  = cv::Mat(_outputVideoSize.y, _outputVideoSize.x, CV_8UC3);
    _recordingFrame2 = cv::Mat(_outputVideoSize.y, _outputVideoSize.x, CV_8UC3);
    _isRecording     = PrototypeVideoRecordingType_Fullscreen;

    // create a bitmap
    _hbwindow = CreateCompatibleBitmap((HDC)_hwindowDC, _outputVideoSize.x, _outputVideoSize.y);
    // create a bitmap
    _bitmapInfoHeader.biSize          = sizeof(BITMAPINFOHEADER);
    _bitmapInfoHeader.biWidth         = _outputVideoSize.x;
    _bitmapInfoHeader.biHeight        = -_outputVideoSize.y; // this is the line that makes it draw upside down or not
    _bitmapInfoHeader.biPlanes        = 1;
    _bitmapInfoHeader.biBitCount      = 24;
    _bitmapInfoHeader.biCompression   = BI_RGB;
    _bitmapInfoHeader.biSizeImage     = 0;
    _bitmapInfoHeader.biXPelsPerMeter = 0;
    _bitmapInfoHeader.biYPelsPerMeter = 0;
    _bitmapInfoHeader.biClrUsed       = 0;
    _bitmapInfoHeader.biClrImportant  = 0;

    // use the previously created device context with the bitmap
    SelectObject((HDC)_hwindowCompatibleDC, (HBITMAP)_hbwindow);

    try {
        _outputVideo = cv::VideoWriter();
        _outputVideo.open(
          filepath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, cv::Size(_outputVideoSize.x, _outputVideoSize.y), true);
    } catch (...) {}
#endif
}

void
PrototypeVideoRecorder::stopRecordingDesktop()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    if (_isRecording != PrototypeVideoRecordingType_Fullscreen) return;

    // block until writing is finished
    _threadpool.waitForWork();

    _outputVideoBuffer.push_back(std::move(_outputVideo));
    _outputVideo = {};
    while (!_outputVideoBuffer.empty()) {
        try {
            auto video = std::move(_outputVideoBuffer.back());
            _outputVideoBuffer.pop_back();
            video.release();
        } catch (...) {}
    }

    // avoid memory leak
    DeleteObject((HBITMAP)_hbwindow);
    DeleteDC((HDC)_hwindowCompatibleDC);
    ReleaseDC((HWND)_desktopHwnd, (HDC)_hwindowDC);

    revertStderr();
    cv::redirectError(NULL);
    _isRecording = PrototypeVideoRecordingType_None;
#endif
}

int
myErrorHandler(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void*)
{
    // handle opencv errors
    return 0;
}

void
PrototypeVideoRecorder::switchStderr()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    {
        _iferr = std::ifstream(PROTOTYPE_VIDEO_PATH("logserr.txt"));
        if (!_iferr.is_open()) {
            std::ofstream errfile(PROTOTYPE_VIDEO_PATH("logserr.txt"));
            errfile.close();
            _iferr = std::ifstream(PROTOTYPE_VIDEO_PATH("logserr.txt"));
        }
        _cerrbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(_iferr.rdbuf());
    }
    {
        _ifin = std::ifstream(PROTOTYPE_VIDEO_PATH("logsin.txt"));
        if (!_ifin.is_open()) {
            std::ofstream infile(PROTOTYPE_VIDEO_PATH("logsin.txt"));
            infile.close();
            _ifin = std::ifstream(PROTOTYPE_VIDEO_PATH("logsin.txt"));
        }
        _cinbuf = std::cin.rdbuf();
        std::cin.rdbuf(_ifin.rdbuf());
    }
    {
        _ifout = std::ifstream(PROTOTYPE_VIDEO_PATH("logsout.txt"));
        if (!_ifout.is_open()) {
            std::ofstream outfile(PROTOTYPE_VIDEO_PATH("logsout.txt"));
            outfile.close();
            _iferr = std::ifstream(PROTOTYPE_VIDEO_PATH("logsout.txt"));
        }
        _coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(_ifout.rdbuf());
    }
#endif
}

void
PrototypeVideoRecorder::revertStderr()
{
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    {
        _iferr.close();
        std::cerr.rdbuf(_cerrbuf);
    }
    {
        _ifin.close();
        std::cin.rdbuf(_cerrbuf);
    }
    {
        _ifout.close();
        std::cout.rdbuf(_coutbuf);
    }
#endif
}
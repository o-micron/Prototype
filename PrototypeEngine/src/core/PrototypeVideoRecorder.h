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

#pragma once

#include "../../include/PrototypeEngine/PrototypeEngineApi.h"

#include "PrototypeThreadpool.h"

#include <deque>
#include <fstream>
#include <functional>
#include <memory>

#if defined(PROTOTYPE_PLATFORM_WINDOWS)
#include <opencv2/opencv.hpp>
#endif

#include <PrototypeCommon/Maths.h>

typedef unsigned long  PTDWORD;
typedef long           PTLONG;
typedef unsigned short PTWORD;

namespace cv {
class Mat;
class VideoWriter;
}

enum PrototypeVideoRecordingType_
{
    PrototypeVideoRecordingType_None = 0,
    PrototypeVideoRecordingType_Framebuffer,
    PrototypeVideoRecordingType_Fullscreen,

    PrototypeVideoRecordingType_Count
};

struct PrototypeBitmapInfoHeader
{
    PTDWORD biSize;
    PTLONG  biWidth;
    PTLONG  biHeight;
    PTWORD  biPlanes;
    PTWORD  biBitCount;
    PTDWORD biCompression;
    PTDWORD biSizeImage;
    PTLONG  biXPelsPerMeter;
    PTLONG  biYPelsPerMeter;
    PTDWORD biClrUsed;
    PTDWORD biClrImportant;
};

struct PrototypeVideoRecorder
{
    PrototypeVideoRecorder();
    ~PrototypeVideoRecorder();

    const PrototypeVideoRecordingType_& isRecording();

    void updateFrame(std::function<void(void** data)>&& fn);
    void commitFrame();

    void startRecordingFramebuffer(const char* filepath, i32 width, i32 height);
    void stopRecordingFramebuffer();
    void startRecordingDesktop(const char* filepath, i32 x = -1, i32 y = -1, i32 width = -1, i32 height = -1);
    void stopRecordingDesktop();

  private:
    void switchStderr();
    void revertStderr();
    PrototypeVideoRecordingType_ _isRecording;
    void*                        _desktopHwnd;
    PrototypeThreadpool          _threadpool;
#if defined(PROTOTYPE_PLATFORM_WINDOWS)
    cv::VideoWriter              _outputVideo;
    glm::ivec2                   _screenSize;
    glm::ivec2                   _outputVideoCoordinates;
    glm::ivec2                   _outputVideoSize;
    std::deque<cv::VideoWriter>  _outputVideoBuffer;
    cv::Mat                      _recordingFrame;
    cv::Mat                      _recordingFrame2;
    std::streambuf*              _cerrbuf;
    std::ifstream                _iferr;
    std::streambuf*              _cinbuf;
    std::ifstream                _ifin;
    std::streambuf*              _coutbuf;
    std::ifstream                _ifout;
    void*                        _hwindowDC;
    void*                        _hwindowCompatibleDC;
    void*                        _hbwindow;
    PrototypeBitmapInfoHeader    _bitmapInfoHeader;
#endif
};

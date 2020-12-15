// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <chrono>

const std::string pipeline("rtspsrc location=rtsp://192.168.100.19:8554/live latency=0 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink");

int main(int argc, char const* argv[]) {
  
  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    
  if (!cap.isOpened()) {
        std::cerr <<"VideoCapture not opened"<< std::endl;
        exit(-1);
    }
    
    auto tp_1 = std::chrono::steady_clock::now();
    auto tp_2 = std::chrono::steady_clock::now();
    const int xl = 0;
    const int yl = 0;
    
    const int width = 640;
    const int height = 480;

    const int xr = width;
    const int yr = 0;

    const cv::Rect rl(xl, yl, width, height);
    const cv::Rect rr(xr, yr, width, height);

    while (true) {

        cv::Mat frame;

        if (cap.read(frame))
        {
          tp_1 = tp_2;
          tp_2 = std::chrono::steady_clock::now();
          const auto time = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
          std::cout << "Time to prev image: " << time << "s" << std::endl;
        }
        cv::Mat left(frame, rl);
        cv::Mat right(frame, rr);
        cv::imshow("receiver", left);
        cv::imshow("receiverr", right);

        char key = static_cast<char>(cv::waitKey(1));
        if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
          break;
        }
    }

    return 0;
}

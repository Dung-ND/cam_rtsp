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
#include <numeric>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include "mynteyed/camera.h"
#include "mynteyed/utils.h"

#include "util/cam_utils.h"
#include "util/counter.h"
#include "util/cv_painter.h"

MYNTEYE_USE_NAMESPACE

// const std::string pipeline("appsrc is-live=true ! autovideoconvert ! x264enc ! rtph264pay ! udpsink host=127.0.0.1/live/test");
//"appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay ! udpsink host=127.0.0.1 port=5000"
// const std::string pipeline("appsrc is-live=true ! jpegdec ! videoconvert ! x264enc ! rtph264pay ! udpsink");
// const std::string pipeline("appsrc is-live=true ! videoconvert ! video/x-raw,width=1280,height=480,framerate=30/1 ! x264enc tune=zerolatency pass=quant ! rtph264pay ! h264parse ! rtspclientsink latency=15 location=rtsp://127.0.0.1:8554/live/test");14.232.245.24
const std::string pipeline("appsrc is-live=true ! videoconvert ! video/x-raw,width=1280,height=480 ! x264enc tune=zerolatency pass=qual ! queue max-size-buffers=1  silent=true ! h264parse ! queue max-size-buffers=1 silent=true ! rtspclientsink latency=55 location=rtsp://127.0.0.1:8554/live");


int main(int argc, char const* argv[]) {
  Camera cam;
  DeviceInfo dev_info;
  if (!util::select(cam, &dev_info)) {
    return 1;
  }
  util::print_stream_infos(cam, dev_info.index);

  std::cout << "Open device: " << dev_info.index << ", "
      << dev_info.name << std::endl << std::endl;

  OpenParams params(dev_info.index);
  {
    // Framerate: 30(default), [0,60], [30](STREAM_2560x720)
    params.framerate = 30;

    // Device mode, default DEVICE_ALL
    //   DEVICE_COLOR: IMAGE_LEFT_COLOR ✓ IMAGE_RIGHT_COLOR ? IMAGE_DEPTH x
    //   DEVICE_DEPTH: IMAGE_LEFT_COLOR x IMAGE_RIGHT_COLOR x IMAGE_DEPTH ✓
    //   DEVICE_ALL:   IMAGE_LEFT_COLOR ✓ IMAGE_RIGHT_COLOR ? IMAGE_DEPTH ✓
    // Note: ✓: available, x: unavailable, ?: depends on #stream_mode
    params.dev_mode = DeviceMode::DEVICE_COLOR;

    // Color mode: raw(default), rectified
    // params.color_mode = ColorMode::COLOR_RECTIFIED;

    // Stream mode: left color only
    // params.stream_mode = StreamMode::STREAM_640x480;  // vga
    // params.stream_mode = StreamMode::STREAM_1280x720;  // hd
    // Stream mode: left+right color
    // params.stream_mode = StreamMode::STREAM_1280x480;  // vga
    params.stream_mode = StreamMode::STREAM_1280x480;  // hd

    // Auto-exposure: true(default), false
    // params.state_ae = false;

    // Auto-white balance: true(default), false
    // params.state_awb = false;

    // IR Depth Only: true, false(default)
    // Note: IR Depth Only mode support frame rate between 15fps and 30fps.
    //     When dev_mode != DeviceMode::DEVICE_ALL,
    //       IR Depth Only mode not be supported.
    //     When stream_mode == StreamMode::STREAM_2560x720,
    //       frame rate only be 15fps in this mode.
    //     When frame rate less than 15fps or greater than 30fps,
    //       IR Depth Only mode will be not available.
    // params.ir_depth_only = true;

    // Infrared intensity: 0(default), [0,10]
    params.ir_intensity = 0;

    // Colour depth image, default 5000. [0, 16384]
    // params.colour_depth_value = 5000;
  }

  // Enable what process logics
  // cam.EnableProcessMode(ProcessMode::PROC_IMU_ALL);

  // Enable image infos
  cam.EnableImageInfo(true);

  cam.Open(params);

  std::cout << std::endl;
  if (!cam.IsOpened()) {
    std::cerr << "Error: Open camera failed" << std::endl;
    return 1;
  }
  std::cout << "Open device success" << std::endl << std::endl;

  std::cout << "Press ESC/Q on Windows to terminate" << std::endl;

  bool is_left_ok = cam.IsStreamDataEnabled(ImageType::IMAGE_LEFT_COLOR);
  bool is_right_ok = cam.IsStreamDataEnabled(ImageType::IMAGE_RIGHT_COLOR);
  // bool is_depth_ok = cam.IsStreamDataEnabled(ImageType::IMAGE_DEPTH);

  cv::VideoWriter writer(pipeline, cv::CAP_GSTREAMER, 0, 30, cv::Size(1280, 480), true);
  if (!writer.isOpened()) {
        std::cerr <<"VideoWriter not opened"<< std::endl;
        exit(-1);
    }
  // if (is_left_ok) cv::namedWindow("left color");
  // if (is_right_ok) cv::namedWindow("before");
  // cv::namedWindow("after");
  // if (is_depth_ok) cv::namedWindow("depth");

  CVPainter painter;
  util::Counter counter(params.framerate);
  std::vector<double> track_times;
  for (;;) {
    cam.WaitForStream();
    auto allow_count = false;
    cv::Mat left, right;
    if (is_left_ok) {
      auto left_color = cam.GetStreamData(ImageType::IMAGE_LEFT_COLOR);
      if (left_color.img) {
        allow_count = true;
        left = left_color.img->To(ImageFormat::COLOR_BGR)->ToMat();
        // painter.DrawSize(left, CVPainter::TOP_LEFT);
        // painter.DrawStreamData(left, left_color, CVPainter::TOP_RIGHT);
        // painter.DrawInformation(left, util::to_string(counter.fps()),
            // CVPainter::BOTTOM_RIGHT);
        // cv::imshow("left color", left);
      }
    }

    if (is_right_ok) {
      auto right_color = cam.GetStreamData(ImageType::IMAGE_RIGHT_COLOR);
      if (right_color.img) {
        allow_count = true;
        right = right_color.img->To(ImageFormat::COLOR_BGR)->ToMat();
        // painter.DrawSize(right, CVPainter::TOP_LEFT);
        // painter.DrawStreamData(right, right_color, CVPainter::TOP_RIGHT);
        // cv::imshow("right color", right);
      }
    }
    cv::Mat both;
    cv::hconcat(left, right, both);
    writer.write(both);
    
    // const auto tp_1 = std::chrono::steady_clock::now();
    // cv::cvtColor(both, both, CV_BGR2GRAY);
    // const auto tp_2 = std::chrono::steady_clock::now();
    // cv::imshow("before", both);
    // cv::equalizeHist(both,both);
    // const auto tp_3 = std::chrono::steady_clock::now();
    // const auto cvt = std::chrono::duration_cast<std::chrono::duration<double>>(tp_2 - tp_1).count();
    // // std::cout << "cvt time: " << cvt << std::endl;
    // const auto equalH = std::chrono::duration_cast<std::chrono::duration<double>>(tp_3 - tp_2).count();
    // track_times.push_back(equalH);
    // // std::cout << "equalH time: " << equalH << std::endl;
    // cv::imshow("after", both);
    // if (is_depth_ok) {
    //   auto image_depth = cam.GetStreamData(ImageType::IMAGE_DEPTH);
    //   if (image_depth.img) {
    //     allow_count = true;
    //     cv::Mat depth;
    //     depth = image_depth.img->ToMat();
    //     painter.DrawSize(depth, CVPainter::TOP_LEFT);
    //     painter.DrawStreamData(depth, image_depth, CVPainter::TOP_RIGHT);
    //     cv::imshow("depth", depth);
    //   }
    // }

    if (allow_count == true) {
      counter.Update();
    }

    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
      // std::sort(track_times.begin(), track_times.end());
      //   const auto total_he_time = std::accumulate(track_times.begin(), track_times.end(), 0.0);
      //   std::cout << "median he time: " << track_times.at(track_times.size() / 2) << "[s]" << std::endl;
      //   std::cout << "mean he time: " << total_he_time / track_times.size() << "[s]" << std::endl;
      break;
    }
  }

  cam.Close();

  return 0;
}

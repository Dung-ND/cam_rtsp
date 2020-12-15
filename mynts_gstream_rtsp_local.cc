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
#include <string>
#include <chrono>
#include <iostream>
#include <numeric>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mynteye/api/api.h"
#include "mynteye/util/times.h"

#define M_PIPELINE "appsrc is-live=true ! autovideoconvert ! queue max-size-buffers=2  silent=true  ! x264enc frame-packing=side-by-side speed-preset=veryfast tune=zerolatency pass=qual ! queue max-size-buffers=2  silent=true ! h264parse ! queue max-size-buffers=2  silent=true ! rtspclientsink latency=0 location=rtsp://localhost:8554/live"
// #define M_PIPELINE "appsrc is-live=true ! autovideoconvert ! video/x-raw,width=1504,height=480 ! x264enc tune=zerolatency pass=qual ! queue max-size-buffers=1  silent=true ! h264parse ! queue max-size-buffers=1  silent=true ! rtspclientsink latency=0 location=rtsp://localhost:8554/live"

MYNTEYE_USE_NAMESPACE

int main(int argc, char *argv[]) {
  auto &&api = API::Create(argc, argv);
  if (!api) return 1;

  auto request = api->GetStreamRequest();

//   struct StreamRequest {
//   /** Stream width in pixels */
//   std::uint16_t width;
//   /** Stream height in pixels */
//   std::uint16_t height;
//   /** Stream pixel format */
//   Format format;
//   /** Stream frames per second */
//   std::uint16_t fps;
//   }

  // request.fps = 20;
  api->ConfigStreamRequest(request);
  api->SetOptionValue(Option::FRAME_RATE, 20);
  api->SetOptionValue(Option::EXPOSURE_MODE, 0);

  // max_gain: range [0,48], default 48
  api->SetOptionValue(Option::MAX_GAIN, 48);
  // max_exposure_time: range [0,240], default 240
  api->SetOptionValue(Option::MAX_EXPOSURE_TIME, 240);
  // desired_brightness: range [0,255], default 192
  api->SetOptionValue(Option::DESIRED_BRIGHTNESS, 192);
  api->SetOptionValue(Option::IMU_FREQUENCY, 200);
  int frame_rate = api->GetOptionValue(Option::FRAME_RATE);
  std::cout << std::endl << "fps:" << frame_rate << std::endl;
  // api->SetOptionValue(Option::IMU_FREQUENCY, 500);
  api->Start(Source::VIDEO_STREAMING);

//  cv::namedWindow("frame");
  
  cv::VideoWriter writer(M_PIPELINE, cv::CAP_GSTREAMER, 0, 20, cv::Size(1504, 480), true);
  if (!writer.isOpened()) {
      std::cerr << "VideoWriter not opened \n";
  }

  unsigned int prev_num_frame = 0;
  unsigned int num_frame = 0;
  double total_run_time = 0.0;
  double period_time = 1.0;

  std::vector<double> track_times;
  auto t1 = std::chrono::steady_clock::now();
  auto t2 = std::chrono::steady_clock::now();

  while (true) {
    // t1 = std::chrono::steady_clock::now();

    api->WaitForStreams();

    auto &&left_data = api->GetStreamData(Stream::LEFT);
    auto &&right_data = api->GetStreamData(Stream::RIGHT);

    if (!left_data.frame.empty() && !right_data.frame.empty()) {
      cv::Mat img;

      cv::hconcat(left_data.frame, right_data.frame, img);

      cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);

      writer.write(img);
      // t2 = std::chrono::steady_clock::now();

      // auto time = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
      // track_times.push_back(time);

      // num_frame++;
      // total_run_time += time;

      // if (total_run_time - period_time > 0.0) {
      //   int diff_num_frame = num_frame - prev_num_frame;
        //std::cout << "At " << total_run_time << ", " << diff_num_frame << " has been sent \n";

      //   period_time++;
      //   prev_num_frame = num_frame;
      // }
    }

    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
      break;
    }
  }

  api->Stop(Source::VIDEO_STREAMING);

  // std::sort(track_times.begin(), track_times.end());
  // const auto total_track_time = std::accumulate(track_times.begin(), track_times.end(), 0.0);
  // std::cout << num_frame << " in " << total_track_time << " [s] \n";
  // std::cout << "run in " << num_frame / total_track_time << " fps \n";
  // std::cout << "median time: " << track_times.at(track_times.size() / 2) << "[s] \n";
  // std::cout << "mean time: " << total_track_time / track_times.size() << "[s] \n";

  return 0;
}


# cam_rtsp
opencv - rtsp

const std::string pipeline("appsrc is-live=true ! videoconvert ! video/x-raw,width=1280,height=480 ! x264enc tune=zerolatency pass=qual ! queue max-size-buffers=1  silent=true ! h264parse ! queue max-size-buffers=1 silent=true ! rtspclientsink latency=55 location=rtsp://127.0.0.1:8554/live");

const std::string pipeline("rtspsrc location=rtsp://192.168.100.19:8554/live latency=0 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink");

"appsrc is-live=true ! videoconvert ! video/x-raw,width=1280,height=480,framerate=30/1 ! x264enc tune=zerolatency ! rtph264pay ! udpsink host=127.0.0.1 port=5000"

cap:

gst-launch-1.0 udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=H264 ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink

Build OpenCV with Gstreamer:

cmake     -DCMAKE_BUILD_TYPE=Release     -DCMAKE_INSTALL_PREFIX=/mnt/F/MYNT-EYE-D-SDK/ocv_340_gstreamer     -DENABLE_CXX11=ON     -DBUILD_DOCS=OFF     -DBUILD_EXAMPLES=OFF     -DBUILD_JASPER=OFF     -DBUILD_OPENEXR=OFF     -DBUILD_PERF_TESTS=OFF     -DBUILD_TESTS=OFF     -DWITH_EIGEN=ON     -DWITH_FFMPEG=ON     -DWITH_OPENMP=ON -DWITH_GSTREAMER=ON ..

sudo apt install gstreamer1.0-rtsp

also gstreamer bad and good

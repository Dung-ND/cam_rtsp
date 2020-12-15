# cam_rtsp
opencv - rtsp

const std::string pipeline("appsrc is-live=true ! videoconvert ! video/x-raw,width=1280,height=480 ! x264enc tune=zerolatency pass=qual ! queue max-size-buffers=1  silent=true ! h264parse ! queue max-size-buffers=1 silent=true ! rtspclientsink latency=55 location=rtsp://127.0.0.1:8554/live");

const std::string pipeline("rtspsrc location=rtsp://192.168.100.19:8554/live latency=0 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink");

"appsrc is-live=true ! videoconvert ! video/x-raw,width=1280,height=480,framerate=30/1 ! x264enc tune=zerolatency ! rtph264pay ! udpsink host=127.0.0.1 port=5000"

cap:

gst-launch-1.0 udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=H264 ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink

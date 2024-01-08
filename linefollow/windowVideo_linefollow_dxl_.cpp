//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <unistd.h>
//#include <sys/time.h>
//#include <signal.h>
//#include "dxl.hpp"
//using namespace cv;
//using namespace std;
//bool ctrl_c_pressed = false;
//void ctrlc_handler(int) { ctrl_c_pressed = true; }
//bool mode = false;
//int main() {
//    // Open the video capture object (use 0 for default camera)
//    VideoCapture cap("line11.mp4");
//    string dst = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
//         nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
//         h264parse ! rtph264pay pt=96 ! \
//         udpsink host=192.168.0.47 port=8001 sync=false";
//
//    VideoWriter writer(dst, 0, (double)30, Size(640, 90), true);
//    if (!writer.isOpened()) { cerr << "Writer open failed!" << endl; return -1; }
//
//    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
//         nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
//         h264parse ! rtph264pay pt=96 ! \
//         udpsink host=192.168.0.47 port=8002 sync=false";
//
//    VideoWriter writer2(dst2, 0, (double)30, Size(640, 360), true);
//    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1; }
//    // Check if the camera opened successfully
//    if (!cap.isOpened()) {
//        cerr << "Error opening camera." << endl;
//        return -1;
//    }
//
//    Dxl mx;
//    struct timeval start, end1;
//    double time1;
//    int vel1 = 0, vel2 = 0, error;
//    signal(SIGINT, ctrlc_handler);
//    if (!mx.open()) { cout << "dynamixel open error" << endl; return -1; }
//
//    Mat frame, gray, noise, result;
//    Mat labels, stats, centroids;
//
//    while (true) {
//        gettimeofday(&start, NULL);
//        //cap의 영상을 frame에 삽입
//        cap >> frame;
//
//        cvtColor(frame, gray, COLOR_BGR2GRAY);
//        // 크기 조절한 로드 맵
//
//        noise = gray(Rect(0, 270, 640, 90));
//        GaussianBlur(noise, noise, Size(5, 5), 0, 0);
//
//        // 밝기 보정
//        double desiredMean = 70;
//        Scalar meanValue = mean(noise);
//        double b = desiredMean - meanValue.val[0];
//        result = noise + b;
//        result = max(0, min(255, result));
//
//        // 이진화
//        threshold(result, result, 128, 255, THRESH_BINARY);
//
//        int p_width = result.cols;
//        int p_height = result.rows;
//
//        // 레이블링        
//        int numLabels = connectedComponentsWithStats(result, labels, stats, centroids);
//        cvtColor(result, result, COLOR_GRAY2BGR);
//
//        // 주 로드를 잡을 왼쪽 기준점
//        static Point pos(1 * (p_width) / 4, p_height / 2);
//        circle(result, pos, 3, Scalar(0, 255, 0), -1);
//
//        // 주 로드를 잡을 오른쪽 기준점
//        static Point posco(3 * (p_width) / 4, p_height / 2);
//        circle(result, posco, 3, Scalar(0, 255, 0), -1);
//
//        // 에러값을 검출하기 위해 사용할 고정점
//        static Point impot((p_width) / 2, p_height / 2);
//        circle(result, impot, 3, Scalar(0, 255, 0), -1);
//
//        // 각각의 좌표점과 박스 색깔
//        for (int i = 1; i < numLabels; i++) {
//            int left = stats.at<int>(i, CC_STAT_LEFT);
//            int top = stats.at<int>(i, CC_STAT_TOP);
//            int width = stats.at<int>(i, CC_STAT_WIDTH);
//            int height = stats.at<int>(i, CC_STAT_HEIGHT);
//
//            // 객체의 중심점
//            double x = centroids.at<double>(i, 0);
//            double y = centroids.at<double>(i, 1);
//
//            //각 주로드를 잡기위한 조건
//            Point integer = pos - Point(x, y);
//            Point integer2 = posco - Point(x, y);
//
//            // 레이블 기준으로 객체 구분 (작은 픽셀들은 노이즈로 취급)
//            int label = (stats.at<int>(i, CC_STAT_AREA));
//            if (label < 80)continue;
//
//            //x축과 y축의 범위를 특정하여 점 따라가기
//            if ((integer.x <= 100 && integer.x >= -100) && ((integer.y >= -80) && (integer.y <= 80)))
//            {
//                // 객체의 중심점으로 좌표값 초기화~
//                pos = Point(x, y);
//                // 사각형            
//                rectangle(result, Point(left, top), Point(left + width, top + height), Scalar(0, 255, 0), 2);
//
//                // 원 (중심 좌표에 점 찍기)
//                circle(result, Point(static_cast<int>(x), static_cast<int>(y)), 3, Scalar(0, 0, 255), -1);
//            }
//            else if ((integer2.x <= 80 && integer2.x >= -80) && ((integer2.y <= 80) && (integer2.y >= -85)))
//            {
//                posco = Point(x, y);
//                // 사각형            
//                rectangle(result, Point(left, top), Point(left + width, top + height), Scalar(0, 255, 0), 2);
//
//                // 원 (중심 좌표에 점 찍기)
//                circle(result, Point(static_cast<int>(x), static_cast<int>(y)), 3, Scalar(0, 0, 255), -1);
//            }
//            // integer.y 차이가 150이상이되면 객체의
//            else if (integer.y >= 150)pos = Point(x, y);
//            else if (integer2.y >= 150)posco = Point(x, y);
//
//            else
//            {
//                // 사각형            
//                rectangle(result, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 2);
//                // 원 (중심 좌표에 점 찍기)
//                circle(result, Point(static_cast<int>(x), static_cast<int>(y)), 3, Scalar(255, 0, 0), -1);
//            }
//
//        }
//        error = impot.x - (posco.x + pos.x) / 2;
//        line(result, pos, posco, Scalar(0, 0, 255), 1);
//        vel1 = 100 - (0.3) * error;
//        vel2 = -(100 + (0.3) * error);
//        // 키보드 감지!
//        if (mx.kbhit())
//        {
//            char ch = mx.getch();
//            if (ch == 'q') break;
//            else if (ch == 's') mode = true;
//        }
//
//        if (ctrl_c_pressed) break; //Ctrl+c입력시 탈출
//        if (mode) mx.setVelocity(vel1, vel2);   //다이나믹셀 동작
//        usleep(10 * 1000);
//        // 종료 시간 기록
//        gettimeofday(&end1, NULL);
//        time1 = end1.tv_sec - start.tv_sec + (end1.tv_usec - start.tv_usec) / 1000000.0;
//
//        // 기본 영상과 로드 영상   
//        writer << result;
//        writer2 << frame;
//        waitKey(1);
//
//        // 결과 출력
//        cout << "err : " << error << ", lvel:" << vel1 << ',' << "rvel:" << vel2 <<
//            ", time:" << time1 << endl;
//    }
//    // 영상의 재생이 끝나면 종료
//    mx.close();
//    cap.release();
//    return 0;
//}

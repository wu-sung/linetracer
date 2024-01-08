//#include "opencv2/opencv.hpp"
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
//int main()
//{
//    string src = "nvarguscamerasrc sensor-id=0 ! \
//    video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
//    format=(string)NV12 ! nvvidconv flip-method=0 ! video/x-raw, \
//    width=(int)640, height=(int)360, format=(string)BGRx ! \
//    videoconvert ! video/x-raw, format=(string)BGR ! appsink";
//
//    VideoCapture cap(src, CAP_GSTREAMER);
//
//    string dst = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
//        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
//        h264parse ! rtph264pay pt=96 ! \
//        udpsink host=192.168.0.47 port=8001 sync=false";
//
//    VideoWriter writer(dst, 0, (double)30, Size(640, 90), true);
//    if (!writer.isOpened()) { cerr << "Writer open failed!" << endl; return -1; }
//
//    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
//        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
//        h264parse ! rtph264pay pt=96 ! \
//        udpsink host=192.168.0.47 port=8002 sync=false";
//
//    VideoWriter writer2(dst2, 0, (double)30, Size(640, 360), true);
//    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1; }
//
//    //ī�޶� ����
//    if (!cap.isOpened()) {
//        cout << "Camera error" << endl; return -1;
//    }
//
//    Mat frame, gray, noise, result;
//    Mat labels, stats, centroids;
//
//    //���̳��ͻ� ����
//    Dxl mx;
//    struct timeval start, end1;
//    double time1;
//    static int vel1 = 0, vel2 = 0, error;
//    int key = waitKey(30);
//    signal(SIGINT, ctrlc_handler);
//    if (!mx.open()) { cout << "dynamixel open error" << endl; return -1; }
//
//    while (true) {
//        gettimeofday(&start, NULL);
//        //cap�� ������ frame�� ����
//        cap >> frame;
//
//        //c�� ������ ����
//        if (key == 'c') break;
//
//        // 1ä�η� �ٲٱ�
//        cvtColor(frame, gray, COLOR_BGR2GRAY);
//
//        // ũ�� ������ �ε� ��        
//        noise = gray(Rect(0, 270, 640, 90));
//        GaussianBlur(noise, noise, Size(5, 5), 0, 0);
//
//        // ��� ����
//        double desiredMean = 70;
//        Scalar meanValue = mean(noise);
//        double b = desiredMean - meanValue.val[0];
//        result = noise + b;
//        result = max(0, min(255, result));
//
//        // ����ȭ
//        threshold(result, result, 128, 255, THRESH_BINARY);
//
//        int p_width = result.cols;
//        int p_height = result.rows;
//
//        // ���̺�        
//        int numLabels = connectedComponentsWithStats(result, labels, stats, centroids);
//        cvtColor(result, result, COLOR_GRAY2BGR);
//
//        // �� �ε带 ���� ���� ������
//        static Point pos(1 * (p_width) / 4, p_height / 2);
//        circle(result, pos, 3, Scalar(0, 255, 0), -1);
//
//        // �� �ε带 ���� ������ ������
//        static Point posco(3 * (p_width) / 4, p_height / 2);
//        circle(result, posco, 3, Scalar(0, 255, 0), -1);
//
//        static Point impot((p_width) / 2, p_height / 2);
//        circle(result, impot, 3, Scalar(0, 255, 0), -1);
//
//        // ������ ��ǥ���� �ڽ� ����
//        for (int i = 1; i < numLabels; i++) {
//            int left = stats.at<int>(i, CC_STAT_LEFT);
//            int top = stats.at<int>(i, CC_STAT_TOP);
//            int width = stats.at<int>(i, CC_STAT_WIDTH);
//            int height = stats.at<int>(i, CC_STAT_HEIGHT);
//
//            // ��ü�� �߽���
//            double x = centroids.at<double>(i, 0);
//            double y = centroids.at<double>(i, 1);
//
//            // �������� �߽������κ��� ���� �� ������ �߽��� �Ÿ�����
//            Point integer = pos - Point(x, y);
//            Point integer2 = posco - Point(x, y);
//
//            // ���̺� �������� ��ü ���� (���� �ȼ����� ������� ���)
//            int label = (stats.at<int>(i, CC_STAT_AREA));
//            if (label < 80)continue;
//
//            // x��� y���� �Ÿ����� ���̸� �̿��Ͽ� �� ���� ����        
//            if ((integer.x <= 150 && integer.x >= -150) && ((integer.y >= -80) && (integer.y <= 80)))
//            {
//                pos = Point(x, y);
//                // �簢��            
//                rectangle(result, Point(left, top), Point(left + width, top + height), Scalar(0, 255, 0), 2);
//
//                // �� (�߽� ��ǥ�� �� ���)
//                circle(result, Point(static_cast<int>(x), static_cast<int>(y)), 3, Scalar(0, 0, 255), -1);
//            }
//            else if ((integer2.x <= 150 && integer2.x >= -150) && ((integer2.y <= 80) && (integer2.y >= -80)))
//            {
//                posco = Point(x, y);
//                // �簢��            
//                rectangle(result, Point(left, top), Point(left + width, top + height), Scalar(0, 255, 0), 2);
//
//                // �� (�߽� ��ǥ�� �� ���)
//                circle(result, Point(static_cast<int>(x), static_cast<int>(y)), 3, Scalar(0, 0, 255), -1);
//            }
//            else if (integer.y >= 150)pos = Point(x, y);
//            else if (integer2.y >= 150)posco = Point(x, y);
//            else
//            {
//                // �簢��            
//                rectangle(result, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 2);
//
//                // �� (�߽� ��ǥ�� �� ���)
//                circle(result, Point(static_cast<int>(x), static_cast<int>(y)), 3, Scalar(255, 0, 0), -1);
//            }
//            error = impot.x - (posco.x + pos.x) / 2;
//            circle(result, Point(impot.x, impot.y), 3, Scalar(255, 0, 0), -1);
//        }
//        vel1 = 200 - (0.6) * error;
//        vel2 = -(200 + (0.6) * error);
//        if (mx.kbhit())
//        {
//            char ch = mx.getch();
//            if (ch == 'q') break;
//            else if (ch == 's') mode = true;
//        }
//        line(result, pos, posco, Scalar(0, 0, 255), 1);
//
//        if (ctrl_c_pressed) break; //Ctrl+c�Է½� Ż��
//        if (mode) mx.setVelocity(vel1, vel2);
//        usleep(10 * 1000);
//        gettimeofday(&end1, NULL);
//        time1 = end1.tv_sec - start.tv_sec + (end1.tv_usec - start.tv_usec) / 1000000.0;
//
//        cout << "err : " << error << ", lvel:" << vel1 << ',' << "rvel:" << vel2 <<
//            ", time:" << time1 << endl;
//
//        // �⺻ ����� �ε� ����   
//        writer << result;
//        writer2 << frame;
//        waitKey(1);
//    }
//    // ������ ����� ������ ����
//    mx.close();
//    cap.release();
//    return 0;
//}
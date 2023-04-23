#include <iostream>
#include <qrencode.h>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <fstream>
#include <mutex>
#include <thread>
#include <filesystem>

#define MAX_DATA_LENGTH 1024 //单次发送长度
#define QR_DELAY_US 100 //二维码生成延迟

using namespace cv;
using namespace std;

std::mutex  resendMutex; // 接收重传信息锁
bool resendFlag; //重传标志位
/*
 * 获取文件大小
 * */
std::uintmax_t getFileSize(const string &name){
    filesystem::path p = filesystem::current_path() / name;
    return filesystem::file_size(p);
}

/**
 * 用于拆分文件并生成二维码
 * */
void generateQRCode(string name){
    ifstream ifs;
    ifs.open(name,ios::binary);
    if(!ifs.is_open()){
        cout<< "Cannot open file " << name << endl;
        return;
    }
    std::uintmax_t fileSize = getFileSize(name);//获取文件大小来判断数据包数量
    int currentPackageNumber;
    int totalPackageNumber;
    int n = 3;
    vector<string> srcs;
    srcs.emplace_back("1.Test generator and detector");
    srcs.emplace_back("2.Test generator and detector");
    srcs.emplace_back("3.Test generator and detector");
    int i = 0;
    while (i < n){
        // 使用qrencode进行字符串编码
        QRcode *code = QRcode_encodeString(srcs[i].c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
        if (code == NULL) {
            std::cout << "code = NULL" << std::endl;
            return;
        }

        cv::Mat img = cv::Mat(code->width, code->width, CV_8U);

        for (int i = 0; i < code->width; ++i) {
            for (int j = 0; j < code->width; ++j) {
                img.at<uchar>(i, j) = (code->data[i * code->width + j] & 0x01) == 0x01 ? 0 : 255;
            }
        }
        cv::resize(img, img, cv::Size(img.rows * 10, img.cols * 10), 0, 0, cv::INTER_NEAREST);

        cv::Mat result = cv::Mat::zeros(img.rows + 30, img.cols + 30, CV_8U);
        //白底
        result = 255 - result;
        //转换成彩色
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
        //建立roi
        cv::Rect roi_rect = cv::Rect((result.rows - img.rows) / 2, (result.cols - img.rows) / 2, img.cols, img.rows);
        //roi关联到目标图像，并把源图像复制到指定roi
        img.copyTo(result(roi_rect));

        cv::imshow("code", result);
        cv::waitKey(1000);
        i++;
    }

    cv::waitKey(0);
    system("pause");
}

void listenSendback(){

}

int main() {
    //传入文件名
    string fileName = "TestSend.txt";
    //创建监听线程
    std::thread listenThread(listenSendback);
    listenThread.detach();
    //创建传输线程
    std::thread sendThread(generateQRCode, fileName);
    sendThread.join();

    return 0;
}

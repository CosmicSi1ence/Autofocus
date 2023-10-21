#include <windows.h>
#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "INIReader.h"

#include <opencv2/ximgproc/edge_drawing.hpp>

using namespace std;
using namespace cv;

Mat img;
string path = "";
int img_num = -1;
int frames_cnt = 0;

Mat roi_img;
Point roi_loc = Point();
Size roi_size = Size(0, 0);


//  ФУНКЦИЯ ЧТЕНИЯ INI-ФАЙЛА:
bool get_ini_params(const string& config) {

    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);       //  Создаем объект Ридера

    if (reader.ParseError() < 0) {
        cout << "Can't load '" << config << "'\n";
        return 0;
    }

    // ------------------------------ [images]: ----------------------------

    path = reader.Get("images", "path", "");
    if (path == "") {
        cout << "path not declared" << endl;
        return 0;
    }
    else {
        cout << "img_path = " << path << endl;
    }

    img_num = reader.GetInteger("images", "img_num", -1);
    if (img_num == -1) {
        cout << "img_num not declared" << endl;
        return 0;
    }
    else {
        cout << "img_num = " << img_num << endl;
    }

    frames_cnt = reader.GetInteger("images", "img_cnt", -1);
    if (frames_cnt == -1) {
        cout << "img_cnt not declared" << endl;
        return 0;
    }
    else {
        cout << "img_cnt = " << frames_cnt << endl;
    }

    // --------------------------------- [roi]: --------------------------------

    roi_loc.x = reader.GetInteger("roi", "roi_x", -1);
    roi_loc.y = reader.GetInteger("roi", "roi_y", -1);
    if (roi_loc.x == -1 || roi_loc.y == -1) {
        cout << "roi_loc not declared" << endl;
        return 0;
    }
    else {
        cout << "roi_loc = (" << roi_loc.x << ", " << roi_loc.y << ")" << endl;
    }

    roi_size.width = roi_size.height = reader.GetInteger("roi", "roi_size", -1);
    if (roi_size.width == -1) {
        cout << "roi_size not declared" << endl;
        return 0;
    }
    else {
        cout << "roi_size = " << roi_size.width << " x " << roi_size.height << endl; 
    }

    return 1;
}

int main() {

    //  Читаем INI-файл:
    bool is_ok = get_ini_params("img_data.ini");
    if (!is_ok) {
        cout << "INI-file error" << endl;
        return 0;
    }

    Mat dst = Mat(roi_size, CV_8UC1);
    Mat dst_x = Mat(roi_size, CV_8UC1);
    Mat dst_y = Mat(roi_size, CV_8UC1);

    int sf_max = 0;
    int sf_max_frame_num = 0;

    for (int i = 0; i < 149 - 10; i += 10) {
        img = imread(path + "\\" + to_string(img_num + i) + ".jpg", IMREAD_GRAYSCALE);
        roi_img = img(Rect(roi_loc, roi_size)).clone();

        Sobel(roi_img, dst_x, 0, 1, 0);
        Sobel(roi_img, dst_y, 0, 0, 1);
        addWeighted(dst_x, 0.5, dst_y, 0.5, 0, dst);

        int s = sum(dst)[0];
        if (sf_max < s) {
            sf_max = s;
            sf_max_frame_num = i;
        }
    }
    for (int i = sf_max_frame_num - 9; i < sf_max_frame_num + 9; ++i) {
        img = imread(path + "\\" + to_string(img_num + i) + ".jpg", IMREAD_GRAYSCALE);
        roi_img = img(Rect(roi_loc, roi_size)).clone();

        Sobel(roi_img, dst_x, 0, 1, 0);
        Sobel(roi_img, dst_y, 0, 0, 1);
        addWeighted(dst_x, 0.5, dst_y, 0.5, 0, dst);

        int s = sum(dst)[0];
        if (sf_max < s) {
            sf_max = s;
            sf_max_frame_num = i;
        }
    }

    cout << endl << "Most focused frame is " << img_num + sf_max_frame_num << ".jpg" << endl << endl;
    img = imread(path + "\\" + to_string(img_num + sf_max_frame_num) + ".jpg", IMREAD_GRAYSCALE);
    namedWindow("img", WINDOW_AUTOSIZE);
    moveWindow("img", 0, 65);
    imshow("img", img);
    waitKey();

    return 0;
}

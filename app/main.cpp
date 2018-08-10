#include "libsv/sv_image.h"
#include "libsv/sv_codec.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void sv_imshow(const char* name, sv_image_t* image)
{
    int components = SV_GET_FMT_YBPP(image->format) / 8;
    cv::Mat show(image->height, image->width, CV_8UC(components), image->data);
    cv::imshow(name, show);
}

int main(int argc, char** argv)
{
    sv_image_t *image = sv_imread("images/image.jpg");

    sv_image_t *image2 = sv_image_create();

    sv_image_copyTo(image, image2);
    sv_image_copyTo(image2, image);
    int components = SV_GET_FMT_YBPP(image->format) / 8;
    cv::Mat cvimage(image->height, image->width, CV_8UC(components), image->data);

    cv::Mat cvgray;

    cv::cvtColor(cvimage, cvgray, CV_BGR2GRAY);

    sv_image_t* gray = sv_image_from_gray(cvgray.cols, cvgray.rows, cvgray.data, cvgray.cols);

    sv_image_t * graytorgb = sv_image_convert(gray, SV_PIX_FMT_RGB888);
    sv_image_t * graytonv  = sv_image_convert(gray, SV_PIX_FMT_NV12);
    cv::Mat cvrgb(image->height, image->width, CV_8UC(components), graytorgb->data);

    cv::Mat bgr;
    cv::cvtColor(cvrgb, bgr, CV_RGB2BGR);

    sv_image_t * svbgr = sv_image_from_bgr(bgr.cols, bgr.rows, bgr.data, bgr.step);

    sv_image_t * svgray = sv_image_convert(svbgr, SV_PIX_FMT_GRAY8);
    sv_imshow("svgray", svgray);
    cv::imshow("debug", cvrgb);
    cv::waitKey();

    return 0;
}
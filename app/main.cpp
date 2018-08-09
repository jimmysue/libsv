#include "libsv/sv_image.h"
#include "libsv/sv_codec.h"

#include "opencv2/highgui/highgui.hpp"

int main(int argc, char** argv)
{
    sv_image_t *image = sv_imread("images/image.jpg");

    int components = SV_GET_FMT_YBPP(image->format) / 8;
    cv::Mat cvimage(image->height, image->width, CV_8UC(components), image->data);

    cv::imshow("debug", cvimage);
    cv::waitKey();

    return 0;
}

#include "base.h"
#include <libsv/sv_codec.h>
#include <libsv/sv_image.h>

#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void TestImage()
{
    std::string path = CMAKE_SOURCE_DIR + std::string("/test/input/image.jpg");

    sv_image_t* image = sv_imread(path.c_str());
    LIBSV_ASSERT_THROW( image != nullptr);

    sv_image_t * image2 = sv_image_clone(image);
    

    LIBSV_ASSERT_THROW( sv_image_isEqual(image, image2));

    // cv::Mat cvimg = cv::imread(path);

    // cv::Mat cvrgb;

    // cv::cvtColor(cvimg, cvrgb, CV_BGR2RGB);

    // sv_image_t *img2 = sv_image_from_rgb(cvrgb.cols, cvrgb.rows, cvrgb.data, cvrgb.step);

    // LIBSV_ASSERT_THROW(sv_image_isEqual(image, img2));

}


int main()
{
    TestImage();
}
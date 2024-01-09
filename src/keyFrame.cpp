
#include "keyFrame.hpp"

class keyFrame
{

    public:

    static inline int id_gen = 0;
    int id;
    cv::Mat img_grey;
    cv::Matx33d K;
    cv::Mat dist;
    std::vector<cv::KeyPoint> kp;
    cv::Mat des;
    cv::Matx33d rotation;
    cv::Matx31d translation;

    public:

    keyFrame(){}

    keyFrame(const cv::Mat& img_grey, const cv::Matx33d& K, const cv::Mat& dist)
    {
        static cv::Ptr<cv::ORB> extractor = cv::ORB::create();
        this->id = keyFrame::id_gen++;
        this->img_grey = img_grey;
        this->K = K;
        this->dist = dist;
        std::vector<cv::Point2f> corners;
        cv::goodFeaturesToTrack(img_grey,corners,3000,0.01,7);
        this->kp.reserve(corners.size());
        for (const cv::Point2f& corner : corners)
        {
            this->kp.push_back(cv::KeyPoint(corner,20));

        }
        extractor->compute(img_grey,this->kp,des);
        this->rotation = cv::Matx33d::eye();
        this->translation = cv::Matx31d::zeros();
        std::cout<<" Detected " << this->kp.size() << " features"<<std::endl;
    }


};
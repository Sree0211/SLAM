#include "slam_application.hpp"

class Slam_application
{

public:
    Map map_gen;

    void process_frame(const cv::Matx33d& K, const cv::Mat& img_grey)
    {
        keyFrame frame(img_grey, K, cv::Mat::zeros(1,4,CV_64F));
        this-> map_gen.add_frame(frame);

        keyFrame& current_frame = map_gen.frames.at(keyFrame::id_gen - 1);
        keyFrame& previous_frame = map_gen.frames.at(keyFrame::id_gen - 2);

        constexpr static const auto ratio_test = [](std::vector<std::vector<cv::DMatch>>& matches) {
            matches.erase(std::remove_if(matches.begin(), matches.end(), [](const std::vector<cv::DMatch>& options){
                return ((options.size() <= 1) || ((options[0].distance / options[1].distance) > 0.75));
            }), matches.end());
        };

        constexpr static const auto symmetry_test = [](const std::vector<std::vector<cv::DMatch>> &matches1, const std::vector<std::vector<cv::DMatch>> &matches2, std::vector<std::vector<cv::DMatch>>& symMatches) {
            symMatches.clear();
            for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator1 = matches1.begin(); matchIterator1!= matches1.end(); ++matchIterator1) {
                for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator2 = matches2.begin(); matchIterator2!= matches2.end();++matchIterator2) {
                    if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx &&(*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx) {
                        symMatches.push_back({cv::DMatch((*matchIterator1)[0].queryIdx,(*matchIterator1)[0].trainIdx,(*matchIterator1)[0].distance)});
                        break;
                    }
                }
            }
        };

    }

    // Feature matching
    // pose estimation

    // Landmark triangluation

    // Map optimization


};
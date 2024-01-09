#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>

#include "slam_application.h"
#include "viewer.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " video and focal length not foound" << std::endl;
        return 1;
    }   

    // Load the Traffic video
    std::printf(" The loading video - %s\n", argv[1]);
    std::string video_path = argv[1]; 
    double F = std::atof(argv[2]); // Focal length

    // Load the video
    cv::VideoCapture cap(video_path);

    // Get the width, height and frames
    int w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int cnt = cap.get(cv::CAP_PROP_FRAME_COUNT);

    // Camera parameters
    cv::Matx33d K = {
        F, 0, (double)w/2,
        0, F, (double)h/2,
        0, 0, 1
    };
    
    // Create a window using OpenGL
    Viewer viewer(w,h);

    // SLAM 
    slam_application slam;
    int i=0;
    std::cout<< "Process the mapping frame by frame "<<std::endl;

    while(cap.isOpened())
    {
        cv::Mat img;

        if (!cap.isOpened())
        {
            std::cerr << "Error: Couldn't open video file: "<< video_path << std::endl;
            return 1;
        }
        cv::Mat img_copy = img.clone();

        cv::cvtColor(img,img, cv::COLOR_BGR2GRAY); // Convert the frame to gray scale
        cv::resize(img,img, cv::Size(w,h));

        auto start = std::chrono::steady_clock::now();
        slam.process_frame(K, image);  // Perform slam operation
        auto frame_duration = std::chrono::steady_clock::now() - start;
        std::cout << "Processing time: " << frame_duration.count() << " seconds\n";

        start = std::chrono::steady_clock::now();
        display.render(slam.getMap());  // Replace with the actual method to get the map from your SLAM class
        frame_duration = std::chrono::steady_clock::now() - start;
        std::cout << "Rendering time: " << frame_duration.count() << " seconds\n";


    }

    return 0;
}


#include <iostream>
#include <opencv2/opencv.hpp>

#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages

#define GL_GLEXT_PROTOTYPES
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#endif



class gl
{

};

class slam
{

};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <video_file_path>" << std::endl;
        return 1;
    }   

    // Load the Traffic video
    std::printf(" The loading video - %s\n", argv[1]);
    std::string video_path = argv[1];

    if(!glfwInit())
    {
        std::cerr<< "Error: GLFW initialization failed" << std::endl;
        return -1;
    }

    // Set OpenGL context version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create an OpenGL window
    GLFWwindow* window = glfwCreateWindow(800, 600, "DashCam_Traffic_Analysis", NULL, NULL);
    
    if (!window)
    {
        std::cerr << "Error: GLFW window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    // GLenum err = glewInit();
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error: GLEW initialization failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Start capturing video using OpenCV
    cv::VideoCapture cap(video_path);

    int cnt = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int i = 0;

    if (!cap.isOpened())
    {
        std::cerr << "Error: Couldn't open video file: "<< video_path << std::endl;
        return 1;
    }

    // Enable texture
    glEnable(GL_TEXTURE_2D);

    // Set up OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters (modify according to your video dimensions)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    while(!glfwWindowShouldClose(window))
    {
        
        /// Read a frame from the video
        cv::Mat frame;
        cap >> frame;

        std::cout<< "Frame "<< ++i<<" of "<< cnt <<std::endl;

        if (frame.empty()) {
            // Video ended or error occurred
            break;
        }

        // Update the texture with the new frame
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a textured quad
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();

        
        // Display the image using OpenGL or OpenCV's imshow

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    // cap.release();
    // cv::destroyAllWindows();

    // Clean up
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


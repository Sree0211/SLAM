
#include "viewer.h"


class Viewer
{
    private:
    // Window dimensions  
    GLFWwindow* window = nullptr;
    GLuint img_texture;
    int frame_w, frame_h;
    frame_w = 0.5;
    frame_h = 0.5;

    int frame_vertex[2] = {-1,1};
    
    public:
    ~Viewer()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    Viewer(int width,int height)
    {
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

        this->window = glfwCreateWindow(width, height, "DashCam_Traffic-SLAM", nullptr, nullptr);

        // Set OpenGL callbacks for window actions
        glfwSetWindowCloseCallback(this->windowm [](GLFWwindow *window) {glfwSetWindowShouldClose(window, GL_TRUE);});
        glfwSetKeyCallback(this->window, 
            [](GLFWwindow* window, int key, int scancode, int action, int mods) 
            {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
            });
        glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow*, int width, int height){ glViewport(0,0,width,height);});
        glfwMakeContextCurrent(this->window);
        glGenTextures(1, &textureID);

    }

    cv::Mat capture_video()
    {
        // Function to capture current frame from OpenGL window
        glfwMakeContextCurrent(this->window);
        
        // Read the frame buffer size
        int width, height;

        glfwGetFramebufferSize(this->window,&width,&height);
        width = (width / 16) * 16;
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        
        // OpenCV variable to get the pixels from frames
        cv::Mat pixels(height,width, CV_8UC3);
        glReadPixels(0,0,width,height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data);

        return pixels;
    }

    void image_rendering(const Map& map_gen)
    {   
        glfwMakeContextCurrent(this->window);

        // Read the frame buffer size
        int width, height;
        glfwGetFramebufferSize(this->window, &width, &height);

        glViewport(0, 0, width, height);

        const keyFrame& frame = map_gen.frames.at(keyFrame::id_gen - 1);
        const float ratio_image = ((float)frame.image_grey.cols /(float)float.image_grey.rows);
        const float ratio_screen = ((float)width / (float)height);
        float image_width = 0;
        float image_height = 0;
        if(width > height)
        {
            image_width = this->frame_w;
            image_height = ratio_screen * this->frame_h / ratio_image;
        }
        else
        {
            image_width = ratio_image * this->frame_w / ratio_screen ;
            image_height = this->frame_h;
        }

        // OpenGL configurations

        glDepthRange(0,1);
        glClearColor(1.0,1.0,1.0,1.0);
        glClearDepth(1.0);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glEnable(GL_BLEND);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3b(1,1,1);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Render the image
        cv::Mat img;
        cv::drawKeypoints(frame.image_grey, frame.kps, img);
        glBindTexture(GL_TEXTURE_2D, this->img_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB,GL_UNSIGNED_BYTE, img.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);


        glBindTexture(GL_TEXTURE_2D, this->image_texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(this->frame_vertex[0],this->frame_vertex[1] );
        glTexCoord2f(0,1);
        glVertex2f(this->frame_vertex[0], this->frame_vertex[1] - image_height);
        glTexCoord2f(1, 1);
        glVertex2f(this->frame_vertex[0] + image_width, this->frame_vertex[1] - image_height);
        glTexCoord2f(1, 0);
        glVertex2f(this->frame_vertex[0] + image_width, this->frame_vertex[1]);
        glEnd();

        // Configuration for 3D rendering
        glColor3f(0,0,0);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, ((float)width / (float)height), 0.001, 1000.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Helper functions.

        // To combine a 3x3 rotation matrix and a 3x1 translation vector into a 4x4 transformation matrix
        constexpr static const auto rt_to_transform = [](const cv::Matx33f& R, const cv::Matx31f& t){
            cv::Matx44f transform = {
                R(0,0), R(0,1), R(0,2), t(0),
                R(1,0), R(1,1), R(1,2), t(1),
                R(2,0), R(2,1), R(2,2), t(2),
                0.0f,  0.0f,  0.0f, 1.0f
            };
            return transform;
        };

        
        constexpr static const auto opengl_coordinate_system = [](const cv::Matx44f& mat){
            cv::Matx44f swap = {
                1.0f,  0.0f,  0.0f, 0.0f,
                0.0f, -1.0f,  0.0f, 0.0f,
                0.0f,  0.0f, -1.0f, 0.0f,
                0.0f,  0.0f,  0.0f, 1.0f
            };
            return swap * mat * swap.inv();
        };

        glMultMatrixf(opengl_coordinate_system(
            rt_to_transform(map_gen.frames.at(keyFrame::id_gen-1).rotation,
            map_gen.frames.at(keyFrame::id_gen-1).translation
        ).t()
        ).val()
        );

        glTranslatef(0,0,-5);

        const int edges[12][2] = {{0, 1}, {0, 3}, {0, 4}, {2, 1}, {2, 3}, {2, 7}, {6, 3}, {6, 4}, {6, 7}, {5, 1}, {5, 4}, {5, 7}};
        const float verticies[8][3] = {{1, -1, -0.9}, {1, 1, -0.9}, {-1, 1, -0.9}, {-1, -1, -0.9}, {0.1, -0.1, -0.1}, {0.1, 0.1, -0.1}, {-0.1, -0.1, -0.1}, {-0.1, 0.1, -0.1}};
        
        // Render current camera
        const float scale_pose = 0.2;
        glLineWidth(4);
        glBegin(GL_LINES);
        glColor3f(0.5, 0.5, 0.0);

        for (int e = 0; e < 12; ++e) {
            for (int v = 0; v < 2; ++v) {
                cv::Matx41f vertex = {
                    verticies[edges[e][v]][0] * scale_pose,
                    verticies[edges[e][v]][1] * scale_pose,
                    verticies[edges[e][v]][2] * scale_pose,
                    1
                };
                cv::Matx44f pose = opengl_coordinate_system(
                    rt_to_transform(
                        map_gen.frames.at(keyFrame::id_gen-1).rotation, 
                        map_gen.frames.at(keyFrame::id_gen-1).translation
                    ).inv()
                );
                vertex = pose * vertex;
                glVertex3f(vertex(0), vertex(1), vertex(2));
            }
        }
        glEnd();

        // Render other cameras.
        float scale_camera = 0.2;
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        for (const auto& [frame_id, frame] : map_gen.frames) {
            for (int e = 0; e < 12; ++e) {
                for (int v = 0; v < 2; ++v) {
                    cv::Matx41f vertex = {
                        verticies[edges[e][v]][0] * scale_camera,
                        verticies[edges[e][v]][1] * scale_camera,
                        verticies[edges[e][v]][2] * scale_camera,
                        1
                    };
                    cv::Matx44f pose = opengl_coordinate_system(
                        rt_to_transform(
                            frame.rotation, 
                            frame.translation
                        ).inv()
                    );
                    vertex = pose * vertex;
                    glVertex3f(vertex(0), vertex(1), vertex(2));
                }
            }
        }
        glEnd();

        // Render trajectory.
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        for (size_t index = 1; index < map_gen.frames.size(); ++index) {
            for (int offset = -1; offset < 1; ++offset) {
                cv::Matx41f vertex = { 0, 0, 0, 1 };
                const keyFrame& frame = map_gen.frames.at(index + offset);
                cv::Matx44f pose = opengl_coordinate_system(
                    rt_to_transform(
                        frame.rotation, 
                        frame.translation
                    ).inv()
                );
                vertex = pose * vertex;
                glVertex3f(vertex(0), vertex(1), vertex(2));
            }
        }
        glEnd();
        // Render landmarks.
        glPointSize(10);
        glBegin(GL_POINTS);
        for (const auto& [landmark_id, landmark] : map_gen.landmarks) {
            glColor3f(landmark.colour(0), landmark.colour(1), landmark.colour(2));
            glVertex3f(landmark.location(0), -landmark.location(1), -landmark.location(2));
        }
        glEnd();
        // Swap the buffers of the window.
        glfwSwapBuffers(this->window);
        glfwPollEvents();
        if (glfwWindowShouldClose(this->window)) {
            exit(0);
        }

    }



}
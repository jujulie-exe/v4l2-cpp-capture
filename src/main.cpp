#include "../include/Camera.hpp"
#include <iostream>

int main() {
     std::srand(std::time(NULL));
    try {
        Camera cam("/dev/video0", 640, 480);
        std::cout << "Camera initialized" << std::endl;
        
        if (cam.initV4L2() == OK) {
            std::cout << "V4L2 initialized" << std::endl;
            if (cam.takeAFrame() == OK) {
                std::cout << "Frame taken" << std::endl;
            } else {
                std::cerr << "Failed to take frame" << std::endl;
            }
        } else {
            std::cerr << "Failed to init V4L2" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}

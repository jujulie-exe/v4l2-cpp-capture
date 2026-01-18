#include "../include/Camera.hpp"
#include <iostream>

int main() {
     std::srand(std::time(NULL));
    try {
        Camera cam("/dev/video0", 2688, 1520);
        std::cout << "Camera initialized" << std::endl;
        cam.setParameters(V4L2_CID_FOCUS_AUTO, ENABLE);
        cam.setParameters(V4L2_CID_POWER_LINE_FREQUENCY, 1);
        cam.setParameters(V4L2_CID_EXPOSURE_AUTO  , DISABLE); 
        cam.setParameters(V4L2_CID_EXPOSURE_ABSOLUTE, 2);
        
        cam.setParameters(0x009a090c , 0); // focus_automatic_continuous
         
        
       // cam.setParameters(V4L2_CID_EXPOSURE_AUTO , 3);
 
        
        if (cam.initV4L2() == OK) {
            
            for (int i = 0; i < 8; ++i){
            if (cam.takeAFrame() == OK) {
                std::cout << "Frame taken" << std::endl;
            } else {
                std::cerr << "Failed to take frame" << std::endl;
                break ;
            }
               // usleep(20000);
        }
        } else {
            std::cerr << "Failed to init V4L2" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}

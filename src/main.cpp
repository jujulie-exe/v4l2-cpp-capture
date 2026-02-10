#include "CameraV4L2.hpp"
#include "ICamera.hpp"
#include <iostream>

int main() {
     std::srand(std::time(NULL));
     ICamera *cam = NULL;
    try {
        cam = new CameraV4L2 ("/dev/video0", 2688, 1520);
        std::cout << "CameraV4L2 initialized" << std::endl;
        cam->setParameters(V4L2_CID_FOCUS_AUTO, ENABLE);
        cam->setParameters(V4L2_CID_POWER_LINE_FREQUENCY, 1);
        cam->setParameters(V4L2_CID_EXPOSURE_AUTO  , DISABLE); 
        cam->setParameters(V4L2_CID_EXPOSURE_ABSOLUTE, 2);
        
        cam->setParameters(0x009a090c , 0); // focus_automatic_continuous
         
        
       // cam->setParameters(V4L2_CID_EXPOSURE_AUTO , 3);
 
        
        if (cam->initCamera() == OK) {
            
            for (int i = 0; i < 8; ++i){
            if (cam->takeAFrame() == OK) {
                std::cout << "Frame taken" << std::endl;
                sleep(200000);
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

#ifndef CAMERAV4L2_H
#define CAMERAVAL2_H

#include <string>
#include <vector>
#include <exception>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <linux/videodev2.h>
#include <fstream>
#include <ctime>
#include <iostream>
#include "ICamera.hpp"
// Constants
#define OK 0
#define ERROR_SET_FMT -1
#define ERROR_REQ_BUFFER -2
#define ERROR_QBUF -3
#define ERROR_DQBUF -4
#define ERROR_EPOLL_CREATE -5
#define ERROR_CTL_EPOLL -6
#define NUMBER_REQ_BUFFER 4
#define ERROR_OFSTREM_NON_OPEN -7
#define ERROR_NO_SET_CTRL -8
#define TIME_OUT -9
#define DISABLE 1
#define ENABLE  0

#define DONT_SAVE 0
#define WATIN_TIME_MAX 3000

class CameraV4L2 : public ICamera
{
   public:
       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       CameraV4L2(const std::string& pathDevice, size_t height, size_t width);
       CameraV4L2(CameraV4L2 const & src) = delete;
      
    

       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       const std::string getNameCamera(void) const override;
       const std::vector<std::string> getFormatType(void) const override;

       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual int setParameters(uint32_t flag, int32_t value) override;
       int takeAFrame(int flag = SAVE_LOCAL, const std::string& prefix = "frame", const std::string& directory = "./") override;
       int initCamera(void) override;

       /*♡♡♡♡♡♡♡♡♡♡♡OPERATOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       CameraV4L2& operator=(CameraV4L2 const & rsh) = delete;    //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual ~CameraV4L2(); //Cannon
       class ErrorOpen : public std::exception
       {
           public:
               const char * what () const noexcept override { return "Failed to open FD path"; }
       };

       class IsNotACamera : public std::exception
       {
           public:
               const char * what () const noexcept override { return "Is not a camera"; }
       };

   private:
       bool ft_ioctl(const int fd, const int flags, const void *args) const;
       
       int _SetFormat(void);
       int _reqBuffer(void);
       int _mmapBuffer(int index);
       int _epollStart(void);
       int _saveFrame(const std::vector<uint8_t>& frame, const struct v4l2_buffer& buf, const std::string& prefix, const std::string& directory) const;

       int _fd;
       int _epoll_fd;
       size_t _fmt_lght;
       size_t _fmt_wdt;
       std::string _device;
       
       std::vector<void*> _buffer;
       std::vector<size_t> _buffer_size;
};
// std::ostream &operator<<(std::ostream &o, const CameraV4L2 &rhs);
#endif // CAMERA_H

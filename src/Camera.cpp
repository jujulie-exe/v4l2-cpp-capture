#include "../include/Camera.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <cstring> // For memset

       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
Camera::Camera(const std::string& pathDevice, size_t lght, size_t wdt)
: _fd(-1), _buffer_lght(lght), _buffer_wdt(wdt), _device(pathDevice), _buffer(NULL)
{
	this->_fd = open(_device.c_str(), O_NONBLOCK | O_RDWR);
	if (_fd < 0)
		throw ErrorOpen();
	struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
	if (ioctl(_fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        // Handle ioctl error if needed, or just let the capability check fail
    }
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		throw IsNotACamera();
	}
    // ctor
}

       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
const std::string Camera::getNameCamera() const {
	return "";
}
const std::vector<std::string> Camera::getFormatType() const {
	return {""};
}
 
       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/

bool	Camera::initABuffer(void)
{
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;
        if (ioctl(_fd, VIDIOC_QUERYBUF, &buf) < 0) {
            return false;
        }
        _buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, buf.m.offset);
        if (_buffer == MAP_FAILED) {
            return false;
        }
        return true;
}

bool	Camera::takeAFrame( void )
{ 

	return false;
}

bool	Camera::setParameters( int flags )
{
    (void)flags; // Unused for now
	return false;
}

bool Camera::ft_ioctl(const int fd, const int flags, const void *args) const
{
    // Wrapper for ioctl if needed, or implement logic
    // For now just a placeholder as it was declared in header
    if (ioctl(fd, flags, args) < 0)
        return false;
    return true;
}
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
Camera::~Camera()
{
    std::cout << "Destructor called" << std::endl;
    if (_fd > 0)
        close(_fd);
    // TODO: munmap _buffer if it was mapped
    // dtor
}

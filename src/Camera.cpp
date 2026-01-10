#include "../include/Camera.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <cstring> // For memset

       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
Camera::Camera(const std::string& pathDevice, size_t lght, size_t wdt)
: _fd(-1), _fmt_lght(lght), _fmt_wdt(wdt), _device(pathDevice), _buffer(NULL)
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
	_SetFormat();
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
int		Camera::_SetFormat( void ){
		struct v4l2_format fmt = {0};
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = _fmt_wdt;
		fmt.fmt.pix.height = _fmt_lght;
		//TODO verificare il fromato corretto 
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG ;
		fmt.fmt.pix.field = V4L2_FIELD_NONE;
		if (ioctl(this->_fd, VIDIOC_S_FMT , &fmt) < 0){
			return ERROR_SET_FMT;
		}
		return OK;
}
int		Camera::_reqBuffer(void){
		struct v4l2_requestbuffers req = {0};
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.count = NUMBER_REQ_BUFFER;
		req.memory = V4L2_MEMORY_MMAP;
		if (ioctl(this->_fd,  VIDIOC_REQBUFS, &req) < 0){
			return ERROR_REQ_BUFFER;
		}
		_buffer.resize(req.count);
		_buffer_size.resize(req.count);
		return OK;
}
int		Camera::_mmapBuffer(int index){
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = index;
        if (ioctl(_fd, VIDIOC_QUERYBUF, &buf) < 0) {
            return ERROR_QBUF;
        }
		_buffer_size[index] = buf.length; 
        _buffer[index] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, buf.m.offset);
        if (_buffer[index] == MAP_FAILED) {
            return MAP_FAILED;
		}
        if (ioctl(_fd, VIDIOC_QBUF, &buf) < 0) {
            return ERROR_QBUF;
        }
}
       /* struct epoll_event {
           uint32_t      events;
           epoll_data_t  data;    
       };

       union epoll_data {
           void     *ptr;
           int       fd;
           uint32_t  u32;
           uint64_t  u64;
       }; */

int		Camera::_epollStart( void ) {
	this->_epoll_fd = epoll_create(0);
	if (this->_epoll_fd < 0){
		return ERROR_EPOLL_CREATE;
	}
	struct epoll_event ev = {0};
	ev.events = EPOLLIN;
	ev.data.fd = this->_fd;
	if ( epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->fd, &ev) < 0){
		return ERROR_CTL_EPOLL;
	}
	return OK;
}
int		Camera::initV4L2(void)
{
	_reqBuffer();
	for (size_t i = 0; i < _buffer.size(), ++i)
	{
		_mmapBuffer(i);
	}
	_epollStart();
	enum v4l2_buf_type	type;
	type.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(_fd,  VIDIOC_STREAMON , &type) < 0) {
		return ERROR_QBUF;
    }
	_epollStart();
	return OK;
}

int	Camera::takeAFrame( void )
{ 
	struct epoll_event ev[1];
	int ntfcs = -1;

	ntfcs = epoll_wait(this->_epoll_fd, ev, 1, WATIN_TIME_MAX);

	if (ntfcs < 0){
		return 
	}
	if (ntfcs == 0)
	{
		return
	}
	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	// toglie dalla coda
	if (ioctl(_fd,  VIDIOC_DQBUF , &buf) < 0) {
		return ERROR_DQBUF;
    }
	// salvo in un vector con le info del buffer
	// rimette in coda
	if (ioctl(_fd,  VIDIOC_QBUF , &buf) < 0) {
		return ERROR_QBUF;
    }
	return OK;
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

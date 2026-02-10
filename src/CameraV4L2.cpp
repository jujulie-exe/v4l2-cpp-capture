#include "../include/CameraV4L2.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <cstring> // For memset

       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
CameraV4L2::CameraV4L2(const std::string& pathDevice, size_t height, size_t width)
: _fd(-1), _epoll_fd(-1), _fmt_lght(height), _fmt_wdt(width), _device(pathDevice)
{
	this->_fd = open(_device.c_str(), O_NONBLOCK | O_RDWR);
	if (_fd < 0)
		throw ErrorOpen();
	struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
	if (ioctl(_fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
		throw ErrorOpen();
    }
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		throw IsNotACamera();
	}
	_SetFormat();
    // ctor
}

       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
const std::string CameraV4L2::getNameCamera() const {
	return "";
}
const std::vector<std::string> CameraV4L2::getFormatType() const {
	return {""};
}

       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/
int		CameraV4L2::_SetFormat( void ){
		struct v4l2_format fmt;
        memset(&fmt, 0, sizeof(fmt));
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = _fmt_wdt;
		fmt.fmt.pix.height = _fmt_lght;
		//TODO verificare il fromato corretto 
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG ;
		fmt.fmt.pix.field = V4L2_FIELD_NONE;
		if (ioctl(this->_fd, VIDIOC_S_FMT , &fmt) < 0){
			return ERROR_SET_FMT;
		}
		return OK;
}
int		CameraV4L2::_reqBuffer(void){
		struct v4l2_requestbuffers req;
        memset(&req, 0, sizeof(req));
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
int		CameraV4L2::_mmapBuffer(int index){
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
            return ERROR_REQ_BUFFER;
		}
        if (ioctl(_fd, VIDIOC_QBUF, &buf) < 0) {
            return ERROR_QBUF;
        }
        return OK;
}

int		CameraV4L2::_epollStart( void ) {
	this->_epoll_fd = epoll_create1(0);
	if (this->_epoll_fd < 0){
		return ERROR_EPOLL_CREATE;
	}
	struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = this->_fd;
	if ( epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_fd, &ev) < 0){
		return ERROR_CTL_EPOLL;
	}
	return OK;
}
int		CameraV4L2::initCamera(void)
{
	if (_reqBuffer() != OK) return ERROR_REQ_BUFFER;
	for (size_t i = 0; i < _buffer.size(); ++i)
	{
		if (_mmapBuffer(i) != OK) return ERROR_REQ_BUFFER;
	}
	
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(_fd,  VIDIOC_STREAMON , &type) < 0) {
		return ERROR_QBUF;
    }
	if (_epollStart() != OK) return ERROR_EPOLL_CREATE;
	return OK;
}
int CameraV4L2::_saveFrame(const std::vector<uint8_t>& frame, const struct v4l2_buffer& buf, const std::string& prefix, const std::string& directory) const{
	std::time_t now = std::time(NULL);
	long long r = static_cast<long long>(std::rand());
    long long id = static_cast<long long>(now) * 100000 + r;
	std::string name = directory + prefix + std::to_string(id) + ".jpg";
	std::ofstream file(name, std::ios::binary | std::ios::out);
	if (!file.is_open()) {
		return ERROR_OFSTREM_NON_OPEN;
	}
	file.write((char*)frame.data(), buf.bytesused);
	file.close();
	return OK;
}
int	CameraV4L2::takeAFrame( int flag, const std::string& prefix, const std::string& directory )
{ 
	struct epoll_event ev[1];
    memset(ev, 0, sizeof(ev)); // Initialize array
	int ntfcs = -1;

	ntfcs = epoll_wait(this->_epoll_fd, ev, 1, WATIN_TIME_MAX);

	if (ntfcs < 0){
		return ERROR_CTL_EPOLL;
	}
	if (ntfcs == 0){
		return TIME_OUT;
	}
	struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	// toglie dalla coda
	if (ioctl(_fd,  VIDIOC_DQBUF , &buf) < 0) {
		return ERROR_DQBUF;
    }

	// salvo in un vector con le info del buffer
	if (flag == SAVE_LOCAL){
		std::vector<uint8_t> frame(_buffer_size[buf.index]);
		memcpy(frame.data(), _buffer[buf.index], _buffer_size[buf.index]);
		if (_saveFrame(frame, buf, prefix, directory) != OK){
			return ERROR_OFSTREM_NON_OPEN;
		}
	}
	
	// rimette in coda
	if (ioctl(_fd,  VIDIOC_QBUF , &buf) < 0) {
		return ERROR_QBUF;
    }
	return OK;
}

int	CameraV4L2::setParameters( uint32_t flag, int32_t value )
{
	struct v4l2_control ctrl;
	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.id = flag;
	ctrl.value = value;
	if (ioctl(_fd, VIDIOC_S_CTRL, &ctrl) < 0){
		return ERROR_NO_SET_CTRL;
	}
	return OK;
}
/* struct v4l2_queryctrl queryctrl;
struct v4l2_control control;

memset(&queryctrl, 0, sizeof(queryctrl));
queryctrl.id = V4L2_CID_BRIGHTNESS;

if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
    if (errno != EINVAL) {
        perror("VIDIOC_QUERYCTRL");
        exit(EXIT_FAILURE);
    } else {
        printf("V4L2_CID_BRIGHTNESS is not supportedn");
    }
} else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
    printf("V4L2_CID_BRIGHTNESS is not supportedn");
} else {
    memset(&control, 0, sizeof (control));
    control.id = V4L2_CID_BRIGHTNESS;
    control.value = queryctrl.default_value;

    if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)) {
        perror("VIDIOC_S_CTRL");
        exit(EXIT_FAILURE);
    }
} */

bool CameraV4L2::ft_ioctl(const int fd, const int flags, const void *args) const
{
    // Wrapper for ioctl if needed, or implement logic
    // For now just a placeholder as it was declared in header
    if (ioctl(fd, flags, args) < 0)
        return false;
    return true;
}
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
CameraV4L2::~CameraV4L2()
{
    std::cout << "Destructor called" << std::endl;
    if (_fd > 0)
        close(_fd);
    if (_epoll_fd > 0)
        close(_epoll_fd);
    
    for (size_t i = 0; i < _buffer.size(); ++i) {
        if (_buffer[i] != MAP_FAILED && _buffer[i] != NULL) {
            munmap(_buffer[i], _buffer_size[i]);
        }
    }
}

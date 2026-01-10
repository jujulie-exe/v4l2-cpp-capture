#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>
#include <exception>
#include <unistd.h>
#include <sys/mman.h> 

//https://docs.kernel.org/userspace-api/media/v4l/func-mmap.html
//https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/common.html
//https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/buffer.html
//https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/vidioc-create-bufs.html#vidioc-create-bufs

class Camera
{
   public:
       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       Camera(const std::string& pathDevice, size_t lght, size_t wdt);  //cannon
       Camera(Camera const & src) = delete ;   //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
	   const std::string getNameCamera(void) const;
	   const std::vector<std::string>  getFormatType(void) const;
 
       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/
	   bool setParameters(int flags);
	   bool takeAFrame( void );
 
       /*♡♡♡♡♡♡♡♡♡♡♡OPERATOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       Camera& operator=(Camera const & rsh) = delete;    //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual ~Camera(); //Cannon
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
	   bool initABuffer( void );
	   int _fd;
	   size_t _buffer_lght;
	   size_t _buffer_wdt;
	   std::string _device;
       void* _buffer;

};
// std::ostream &operator<<(std::ostream &o, const Camera &rhs);
#endif // CAMERA_H

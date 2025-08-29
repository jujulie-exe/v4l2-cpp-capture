#ifndef CAMERA_H
#define CAMERA_H

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
       Camera(const std::string& device, size_t lght; size_t wdt);  //cannon
       Camera(Camera const & src);   //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
	   void getNameCamera(void) const;
	   std::vector<std::string> > getFormatType(void) const;
 
       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/
	   bool setParameters(int falgs);
	   bool takeAFrame( void );
 
       /*♡♡♡♡♡♡♡♡♡♡♡OPERATOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       Camera& operator=(Camera const & rsh);    //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual ~Camera(); //Cannon
  private:
	   bool ft_ioctl(const int fd, const int flags, const void *args) const;
	   bool initABuffer( void )
	   int _fd;
	   std::string _device;
       Camera();  //cannon

};
// std::ostream &operator<<(std::ostream &o, const Camera &rhs);
#endif // CAMERA_H


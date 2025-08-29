#ifndef CAMERA_H
#define CAMERA_H
 
class Camera
{
   public:
       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       Camera(const std::string& device, size_t lght; size_t wdt);  //cannon
       Camera(Camera const & src);   //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
	   void getNameCamera(void) const;
 
       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/
	   bool setParameters(int falgs);
	   bool takeAFrame( void );
 
       /*♡♡♡♡♡♡♡♡♡♡♡OPERATOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       Camera& operator=(Camera const & rsh);    //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual ~Camera(); //Cannon
  private:
	   int _fd;
	   std::string _device;
       Camera();  //cannon

};
// std::ostream &operator<<(std::ostream &o, const Camera &rhs);
#endif // CAMERA_H


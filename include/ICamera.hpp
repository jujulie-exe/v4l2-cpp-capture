#ifndef ICAMERA_H
#define ICAMERA_H
#define SAVE_LOCAL 1
class ICamera
{
   public:
       /*♡♡♡♡♡♡♡♡♡♡♡CTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       ICamera() = default; //
      
    

       /*♡♡♡♡♡♡♡♡♡♡♡GETTER♡♡♡♡♡♡♡♡♡♡♡♡♡*/
      virtual const std::string getNameCamera(void) const = 0;
       virtual const std::vector<std::string> getFormatType(void) const = 0;

       /*♡♡♡♡♡♡♡♡♡♡♡FT♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual int setParameters(uint32_t flag, int32_t value) = 0;
       virtual int takeAFrame(int flag = SAVE_LOCAL, const std::string& prefix = "frame", const std::string& directory = "./") = 0;
       virtual int initCamera(void) = 0;

       /*♡♡♡♡♡♡♡♡♡♡♡OPERATOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       ICamera& operator=(ICamera const & rsh) = delete;    //Cannon
 
       /*♡♡♡♡♡♡♡♡♡♡♡DTOR♡♡♡♡♡♡♡♡♡♡♡♡♡*/
       virtual ~ICamera() = default; //

   private:
};
// std::ostream &operator<<(std::ostream &o, const ICamera &rhs);
#endif // CAMERA_H

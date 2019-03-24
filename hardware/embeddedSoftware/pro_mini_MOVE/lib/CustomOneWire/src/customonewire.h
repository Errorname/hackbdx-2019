
class customonewire {

  public:
  customonewire(int port, int freq_khz);
  int readByte(unsigned char* data, unsigned long int timeout);
  int sendByte(unsigned char data);

  
  private:

  int pin;
  void writeBit(int val);
  int wait_us;


};


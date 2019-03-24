
class customonewire {

  public:
  customonewire(int port, int freq_khz);
  int readByte(unsigned char* data, unsigned long int timeout);
  int sendByte(unsigned char data);
  const char *type[7] = {"repeat-start", "run", "start", "repeat-end", "left", "right", "klaxon"};

  
  private:

  int pin;
  void writeBit(int val);
  int wait_us;


};


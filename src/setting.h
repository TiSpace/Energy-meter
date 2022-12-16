
unsigned long myChannelNumber = 12345678;
const char * myWriteAPIKey = "abcdefghijklm";




#define BAUDRATE 115200
#ifdef debugMode
#define mDNSName "test"   
#else
#define mDNSName "energy"   // Aufruf mit http://energy.local
#endif

/* Use software serial for the PZEM
   Pin 12 Rx (Connects to the Tx pin on the PZEM)
   Pin 13 Tx (Connects to the Rx pin on the PZEM)
*/
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 0 //5 //12
#define PZEM_TX_PIN 2 //4 //13
#endif


uint16_t sendToThingspeak= 50;    // Intervall, wie oft Thingspeak bedient wird [ms]
uint8_t sendToThingspeakEnable = 1;
uint8_t sendToThingspeakEEPROMAdr = 45;

struct Measures {
  float voltage;
  float current;
  float power;
  float energy ;
  float frequency ;
  float pf ;
};
struct Measures Messwerte;

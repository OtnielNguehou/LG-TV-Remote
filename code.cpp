//*****************************************
// NEC (Japanese) Infrared code sending library for the Arduino
// Send a standard NEC 4 byte protocol direct to an IR LED on the define pin
// Assumes an IR LED connected on I/O pin to ground, or equivalent driver.
// Tested on a Freetronics Eleven Uno compatible
// Written by David L. Jones www.eevblog.com
// Youtube video explaining this code: http://www.youtube.com/watch?v=BUvFGTxZBG8
// License: Creative Commons CC BY
//*****************************************

#define IRLEDpin  5              //the arduino pin connected to IR LED to ground. HIGH=LED ON
#define BITtime   562            //length of the carrier bit in microseconds

struct Button {
  int pin;
  uint32_t code;
};

uint32_t makeNEC(uint8_t addr, uint8_t cmd) {
  return (addr << 24) |
         ((uint8_t)~addr << 16) |
         (cmd << 8) |
         (uint8_t)~cmd;
}

// ADDR (8 bits) | INV_ADDR (8 bits) | CMD (8 bits) | INV_CMD (8 bits)
uint8_t ADDRESS = 0x20;   

const uint8_t PowerTracker = 0;

const uint32_t PowerOn     = makeNEC(0x20, 0x23);
const uint32_t PowerOff    = makeNEC(0x20, 0xA3);

const uint32_t VolumeUp    = makeNEC(0x20, 0x40);
const uint32_t VolumeDown  = makeNEC(0x20, 0xC0);

const uint32_t ChannelUp   = makeNEC(0x20, 0x00);
const uint32_t ChannelDown = makeNEC(0x20, 0x80);

const uint32_t Mute        = makeNEC(0x20, 0x90);
const uint32_t Input       = makeNEC(0x20, 0xD0);

const uint32_t Enter       = makeNEC(0x20, 0x22);

const uint32_t ArrowUp     = makeNEC(0x20, 0x02);
const uint32_t ArrowDown   = makeNEC(0x20, 0x82);
const uint32_t ArrowLeft   = makeNEC(0x20, 0xE0);
const uint32_t ArrowRight  = makeNEC(0x20, 0x60);

const uint32_t HomeMenu    = makeNEC(0x20, 0xC2);
const uint32_t Back        = makeNEC(0x20, 0x14);

Button Buttons[] = {
  {6, PowerOn},
  {7, VolumeUp},
  {8, VolumeDown},
  {0, ArrowUp},
  {15, ArrowDown},
  {2, HomeMenu},
  {4, Back},
  {16, Enter}
};



void setup()
{
}

void IRsetup()
{
  pinMode(IRLEDpin, OUTPUT);
  for(int i = 0; i < Buttons.size(); i++){
    pinMode(Buttons[i].pin,INPUT);
  }
  digitalWrite(IRLEDpin, LOW);    //turn off IR LED to start

}

// Ouput the 38KHz carrier frequency for the required time in microseconds
// This is timing critial and just do-able on an Arduino using the standard I/O functions.
// If you are using interrupts, ensure they disabled for the duration.
void IRcarrier(int IRtimemicroseconds)
{
  for(int i=0; i < (IRtimemicroseconds / 26); i++)
    {
    digitalWrite(IRLEDpin, HIGH);   //turn on the IR LED
    //NOTE: digitalWrite takes about 3.5us to execute, so we need to factor that into the timing.
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    digitalWrite(IRLEDpin, LOW);    //turn off the IR LED
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    }
}

//Sends the IR code in 4 byte NEC format
void IRsendCode(uint32_t code))
{
    IRsetup();                       //Only need to call this once to setup

  //send the leading pulse
  IRcarrier(9000);            //9ms of carrier
  delayMicroseconds(4500);    //4.5ms of silence
  
  //send the user defined 4 byte/32bit code
  for (int i=0; i<32; i++)            //send all 4 bytes or 32 bits
    {
    IRcarrier(BITtime);               //turn on the carrier for one bit time
    if (code & 0x80000000)            //get the current bit by masking all but the MSB
      delayMicroseconds(3 * BITtime); //a HIGH is 3 bit time periods
    else
      delayMicroseconds(BITtime);     //a LOW is only 1 bit time period
     code<<=1;                        //shift to the next bit for this byte
    }
  IRcarrier(BITtime);                 //send a single STOP bit.
}

void loop()                           //some demo main code
{
  for(Button button : Buttons){
    if(digitalRead(button.pin) == HIGH){
      IRsendCode(button.code);
    }
  }
  delay(50);

}
#include <Arduino.h>
#line 1 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
int clockPin = 9;
int buttonPin = 11;
int disablePin = 12;
uint32_t count = 0;
uint32_t btnCount = 0;
bool btnState = 0;
bool btnPress = 0;
bool debug = 0;
bool tickActive = 1;
bool tickEnable = 1;
bool tickEnableBtn = 1;
bool outputSerial = 0;
#define    shift_data     10    //dados do shift register 74HC595
#define    shift_latch    11    //latch do shift register 74HC595
#define    shift_clk      12    //clock do shift register 74HC595
#define    write_en       13    //sinal de escrita para EEPROM
#define    output_en      8    //sinal de escrita para EEPROM
int busReq = 5;
int FlipFlopClock = 7;
//int resetPin = 9;
uint32_t tickEnableCountACycle = 200000;
long tickCountCycle = 3;
uint32_t tickDelay = 0;


#line 26 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void setup();
#line 41 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void setAddress(int address, bool outEnable);
#line 57 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
String toBinary(int n, int len);
#line 68 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void setupControlledClock();
#line 78 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void loop();
#line 107 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void clock();
#line 173 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void tick();
#line 186 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
long Set_frequency(long freq);
#line 26 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
void setup() {
    setupControlledClock();

    // pinMode(shift_latch, OUTPUT);   //saída para latch
    // pinMode(shift_data,  OUTPUT);   //saída para dados
    // pinMode(shift_clk,   OUTPUT);   //saída para clock
    // pinMode(output_en,   OUTPUT);      
    // pinMode(busReq,      OUTPUT);
    // pinMode(FlipFlopClock, OUTPUT);
    // pinMode(resetPin, INPUT);
    // digitalWrite(output_en, HIGH);  
    // digitalWrite(busReq, HIGH);
    // digitalWrite(FlipFlopClock, LOW);
}

void setAddress(int address, bool outEnable)
{
   // Seleciona os 3 bits mais significativos de endereço com bit outEnable
   shiftOut(shift_data, shift_clk, MSBFIRST, (address >> 8) | (outEnable ? 0x00 : 0x80));
   
   //Seleciona os 8 bits menos significativos de endereço
   shiftOut(shift_data, shift_clk, MSBFIRST, address);

   //gera pulso de latch para escrever dados nas saídas dos shift registers
   digitalWrite(shift_latch,  LOW);
   digitalWrite(shift_latch, HIGH);
   digitalWrite(shift_latch,  LOW);
} //end setAddress



String toBinary(int n, int len)
{
    String binary;

    for (unsigned i = (1 << len - 1); i > 0; i = i / 2) {
        binary += (n & i) ? "1" : "0";
    }
 
    return binary;
}

void setupControlledClock() {
    Serial.begin(250000);
    Serial.println("Enter Clock step ignore (lower value for higher clock frequency): ");
    pinMode(clockPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(disablePin, INPUT);
    digitalWrite(clockPin, LOW);
}
uint16_t address = 0x0;
uint64_t memCount = 0;
void loop() {
  if(Serial.available()) 
  {
    tickCountCycle = Serial.parseInt();
    //Serial.print(Set_frequency(tickCountCycle));
    //Serial.println(" Hz");
    delay(1000);
  }
 clock();

//  if (memCount >= 5000) {
//     if (!digitalRead(resetPin)) {
//         address = 0x0;

//     }
//     digitalWrite(busReq, LOW);
//     digitalWrite(output_en, LOW);  
//     digitalWrite(FlipFlopClock, LOW);
//     setAddress(address, true);
//     digitalWrite(FlipFlopClock, HIGH);
//     delay(500);
//     address++;
//     digitalWrite(output_en, HIGH);  
//     digitalWrite(busReq, HIGH);
//     memCount=0;
//  }
//  memCount++;
}

void clock()
{
    if (count >= tickCountCycle)
    {
        if (tickActive && tickEnable) {
            tick();
            digitalWrite(LED_BUILTIN, HIGH);
        }
        else {
            digitalWrite(LED_BUILTIN, LOW);
        }

        count = 0;
    }
    bool actualState = digitalRead(buttonPin);

    if (actualState)
    {
        btnCount++;
    }

    if (actualState != btnState)
    {
        btnState = actualState;

        if (actualState)
        {
            btnPress = 1;
        }

        if (btnPress && !actualState)
        {
            if (!tickEnable)
            {
                tick();
            }
            if (debug)
            {
                Serial.print("Fire > ");
                Serial.println(btnCount);
            }
            btnPress = 0;
            btnCount = 0;
            tickActive = !tickActive;
        }

        if (debug)
        {
            Serial.print("Changed State to ");
            Serial.println(actualState);
        }
    }

    if (btnCount > tickEnableCountACycle)
    {
        tickEnable = !tickEnable;
        btnCount = 0;
        if (debug)
        {
            Serial.print("TickEnable ");
            Serial.println(tickEnable);
        }
    }
    count++;
}

void tick()
{
    if (debug) {
        Serial.print(".");
    }
    if (!digitalRead(disablePin)) {
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(tickDelay);
        digitalWrite(clockPin, LOW);
        //delayMicroseconds(tickDelay);
    }
}

long Set_frequency(long freq)
{
  TCCR1A = 0b01000000; 
  uint16_t ocr;
  if(freq < 134) 
  {
    TCCR1B = 0b00001010; 
    ocr = 1000000UL/freq - 1;
    freq = 1000000UL/(ocr+1);
  }
  else 
  {
    TCCR1B = 0b00001001;
    ocr = 8000000UL/freq - 1;
    freq = 8000000UL/(ocr+1);
  }
  OCR1AH = highByte(ocr);
  OCR1AL = lowByte(ocr);
  TIMSK1 = 0;
  return freq;
}

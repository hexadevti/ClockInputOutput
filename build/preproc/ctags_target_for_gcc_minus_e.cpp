# 1 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
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





int busReq = 5;
int FlipFlopClock = 7;
//int resetPin = 9;
uint32_t tickEnableCountACycle = 200000;
long tickCountCycle = 3;
uint32_t tickDelay = 0;


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
   shiftOut(10 /*dados do shift register 74HC595*/, 12 /*clock do shift register 74HC595*/, 1, (address >> 8) | (outEnable ? 0x00 : 0x80));

   //Seleciona os 8 bits menos significativos de endereço
   shiftOut(10 /*dados do shift register 74HC595*/, 12 /*clock do shift register 74HC595*/, 1, address);

   //gera pulso de latch para escrever dados nas saídas dos shift registers
   digitalWrite(11 /*latch do shift register 74HC595*/, 0x0);
   digitalWrite(11 /*latch do shift register 74HC595*/, 0x1);
   digitalWrite(11 /*latch do shift register 74HC595*/, 0x0);
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
    pinMode(clockPin, 0x1);
    pinMode(buttonPin, 0x0);
    pinMode(disablePin, 0x0);
    digitalWrite(clockPin, 0x0);
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
            digitalWrite(13, 0x1);
        }
        else {
            digitalWrite(13, 0x0);
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
        digitalWrite(clockPin, 0x1);
        delayMicroseconds(tickDelay);
        digitalWrite(clockPin, 0x0);
        //delayMicroseconds(tickDelay);
    }
}

long Set_frequency(long freq)
{
  
# 188 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino" 3
 (*(volatile uint8_t *)(0x80)) 
# 188 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
        = 0b01000000;
  uint16_t ocr;
  if(freq < 134)
  {
    
# 192 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino" 3
   (*(volatile uint8_t *)(0x81)) 
# 192 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
          = 0b00001010;
    ocr = 1000000UL/freq - 1;
    freq = 1000000UL/(ocr+1);
  }
  else
  {
    
# 198 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino" 3
   (*(volatile uint8_t *)(0x81)) 
# 198 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
          = 0b00001001;
    ocr = 8000000UL/freq - 1;
    freq = 8000000UL/(ocr+1);
  }
  
# 202 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino" 3
 (*(volatile uint8_t *)(0x89)) 
# 202 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
        = ((uint8_t) ((ocr) >> 8));
  
# 203 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino" 3
 (*(volatile uint8_t *)(0x88)) 
# 203 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
        = ((uint8_t) ((ocr) & 0xff));
  
# 204 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino" 3
 (*(volatile uint8_t *)(0x6F)) 
# 204 "c:\\Users\\Luciano Faria\\Desktop\\Z80\\ClockInputOutput\\clockOutput.ino"
        = 0;
  return freq;
}

#include <DS3231.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <SoftwareSerial.h>

int rx= 4;
int tx= 2;
int inp = 10;

char bt_data ;

DS3231 myRTC;
SoftwareSerial BTserial(rx,tx);

void setup(){
  pinMode(inp,OUTPUT);
  Serial.begin(9600);
  BTserial.begin(9600);
}

void loop(){
  while(BTserial.available()){
    bt_data = BTserial.read();
    Serial.println(bt_data);
    if(bt_data == 'O'){                          // On Button 
      intro();
      bt_data = NULL;
      while(bt_data == NULL){
        bt_data = BTserial.read();
        if (bt_data == 'T'){                     // Button for Time
          Serial.println(bt_data);
          readtime();
          BTserial.println(" ");
          bt_data = NULL;
        }
        else if(bt_data == 'P'){                 // Button for Temperature
          Serial.println(bt_data);
          printTemp();
          BTserial.println(" ");
          bt_data = NULL;
        }
        else if(bt_data == 'F'){                 // Off Button
          Serial.println(bt_data);
          BTserial.println("Turning off AUTO_FARMER...");
          BTserial.println("Thankyou");
          BTserial.println("  ");
          bt_data = 'F';
        }
        else if(bt_data == 'S'){                 // Button for Soil Moisture
          Serial.println(bt_data);
          BTserial.print("Soli moisture is :- ");
          BTserial.print(soilmoisture());
          BTserial.println("%");
          BTserial.println(" ");
          bt_data = NULL;
        }
        else if(bt_data == 'A'){
          BTserial.println("Watering mode set to auto...");
          autofarm();
          bt_data = NULL;
        }
        else if(bt_data == 'M'){
          relay('1');
          delay(3000);
          relay('0');
          BTserial.println("Last watered manually at :- ");
          readtime();
          bt_data = NULL;
        }
        else{
          bt_data = NULL;
        }
      }
    }
    bt_data = NULL;
  }
}


void printTemp(){                           // Function to print temperature.
    BTserial.print("Temperature :- ");
    BTserial.print(myRTC.getTemperature(), 2);
    BTserial.println(" Celsius");    
}

void readtime(){                            // Function to read time from module.
  tmElements_t tm;
  if (RTC.read(tm)) {
    BTserial.print("Time = ");
    print2digits(tm.Hour);
    BTserial.write(':');
    print2digits(tm.Minute);
    BTserial.write(':');
    print2digits(tm.Second);
    BTserial.print(",  Date (D/M/Y) = ");
    BTserial.print(tm.Day);
    BTserial.write('/');
    BTserial.print(tm.Month);
    BTserial.write('/');
    BTserial.print(tmYearToCalendar(tm.Year));
    BTserial.println();
  } else {
    if (RTC.chipPresent()) {
      BTserial.println("The DS1307 is stopped.  Please run the SetTime");
      BTserial.println("example to initialize the time and begin running.");
      BTserial.println();
    } else {
      BTserial.println("DS1307 read error!  Please check the circuitry.");
      BTserial.println();
    }
    delay(9000);
  }
}

void print2digits(int number) {             // To print 0 when time is 12:00 AM
  if (number >= 0 && number < 10) {
    BTserial.write('0');
  }
  BTserial.print(number);
}

void intro(){                               // To give introduction.
  BTserial.println("Hello user welcome to AUTO_FARMER....");
  BTserial.println("  ");
  BTserial.println("The current ");
  readtime();
  printTemp();
  BTserial.println("  ");
  BTserial.println("Press Time to view time.");
  BTserial.println("Press Soil to view Soil Moisture");
  BTserial.println("Press Auto_farm to water automatically.");
  BTserial.println("Press Manual to water manually");
  BTserial.println("Press Temperature to view current temperature.");
  BTserial.println("Press OFF to turn off AUTO_FARMER.");
  BTserial.println(" ");
}
float soilmoisture(){                       // Soil moisture percentage.
  int s = analogRead(A0);
  float l = (100 -((s/1023.00) * 100));   
  return l; 
}

void autofarm(){                            // Automatic watering and updater.
  while(bt_data =='A'){
    if(BTserial.available()){
      bt_data = BTserial.read();
    }
    if(bt_data == 'B'){
      BTserial.println("Manual mode on.");
      bt_data ='B';
    }
    else if(bt_data == 'T'){
      Serial.println(bt_data);
      readtime();
      BTserial.println(" ");
      bt_data ='A';
    }
    else if(bt_data == 'P'){
      Serial.println(bt_data);
      printTemp();
      BTserial.println(" ");
      bt_data = 'A';
    }
    else if(bt_data == 'S'){                 
      Serial.println(bt_data);
      BTserial.print("Soli moisture is :- ");
      BTserial.print(soilmoisture());
      BTserial.println("%");
      BTserial.println(" ");
      bt_data = 'A';
    }
    else{
      bt_data = 'A';
    }
    if(soilmoisture() <= 30.00 && bt_data == 'A'){
      relay('1');
      delay(3000);
      relay('0');
      BTserial.println("Last watered at :- ");
      readtime();
    }
  }
}

void relay(char par){                       // To turn on relay
  if (par == '1'){
    digitalWrite(inp,HIGH);
  }
  else if (par == '0'){
    digitalWrite(inp,LOW);
  }
  else{
    NULL;
  }
}
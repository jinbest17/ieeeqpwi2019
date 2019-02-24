/*
  Linear Tech LTC1859 Demo Board (DC682) & Arduino Uno
  3/27/13 
  Leah Martinez
  
  This is a generalized driver for the LTC1859 A/D.  Please see the datasheet for complete details:
  http://cds.linear.com/docs/en/datasheet/185789fa.pdf
  
  This example controls multimple channels using a linear tech a/d converter
  on their DC682 demo board.  The connections to J1 on the demo board are as follows:
  
  
SPI Line                             MOSI MISO
Arduino Connection         GND        11  12 GND +5V
                             |          |  |  |  |
                            13  11  9   7  5  3  1
                            14  12  10  8  6  4  2
                                        |  |  |  |                            
Arduino Connection                    GND 10 13 +5V
SPI Line                                  CS SCK

*/

#include <SPI.h>  //Arduino's SPI library
#include <Statistic.h>  //This is a non-standard Arduino library.  It is not normally downloaded as part of the IDE.
                        //The code was obtained from the Arduino playground:
                        //http://playground.arduino.cc/Main/Statistics
#include <SdFat.h> //include the SDFat library
SdFat sd;            
SdFile myFile;


unsigned int  AD_data;  //returned AtoD data
int A_D_ChipSelect = 10;  //chip select for AtoD
boolean AtoD_Diff_Mode = false;  //use the AtoD in the single-ended mode.
int AtoD_Range = 1;  //this is the range for 0-5V
int AtoD_PowerMode = 0;  //power mode is "on"
int AtoD_No_Of_Readings = 10;  //number of readings to average for each A/D call
Statistic myStats;  //name for Statistic object

int SD_ChipSelect = 4;  //chip select for SD card

void setup()
{
  Serial.begin(9600);  //start serial comm at 9600 baud
  Serial.print("Statistics lib version =  ");
  Serial.println(STATISTIC_LIB_VERSION);
  myStats.clear(); //explicitly start clean

  //set SPI parameters for AtoD
  pinMode(10, OUTPUT);  //must set this as an output even if the SPI enable is on a different pin for the device
  SPI.begin();  // wake up the SPI bus.


}

void loop()
{

  for(int i = 6; i > -1; i-=2)  //read channels 0,2,4,6
    {
     AD_data = Read_A_D(A_D_ChipSelect, AtoD_Diff_Mode, i, AtoD_Range, AtoD_PowerMode, AtoD_No_Of_Readings); //call the a/d function for channel i
     Serial.print("Channel "); 
     Serial.print(i);  //print the Channel number to the monitor  
     Serial.print(" Counts = ");
     Serial.println(AD_data, DEC);  //print out the counts to the monitor


     delay(1000);  //pause for a second to review data
    }
     Write_To_SD_Card(SD_ChipSelect, AD_data);

}


//***********************          AtoD Read Function         *************************************************************
int Read_A_D(int CS_A_D, boolean Differential_Mode, int A_D_Channel, int Input_Range, int Power_Mode, int NumberOfReadings)
{
pinMode(4,OUTPUT);
digitalWrite(4,HIGH);


  SPI.setBitOrder(MSBFIRST);  //set the MSB first per data sheet
  SPI.setDataMode(SPI_MODE0);  //set SPI mode to CPOL = 0, CPHA = 0 per data sheet
  SPI.setClockDivider(SPI_CLOCK_DIV4);  //max speed of AtoD 4MHz = 16MHz/4 (16MHz=clk speed of Arduino)

  byte data_array_1[100];  //AtoD data returned as one of two bytes
  byte data_array_2[100]; //AtoD data returned as two of two bytes


  unsigned int Counts_A_D;  //this is the returned value
  
                                    //           Channel Selection                      
                                    //| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | COM |A_D_Channel|
  byte Channel_Selection[] = {0x00, //| + | - |   |   |   |   |   |   |     | diff 0+   |
                              0x40, //| - | + |   |   |   |   |   |   |     | diff 1+   |
                              0x10, //|   |   | + | - |   |   |   |   |     | diff 2+   |
                              0x50, //|   |   | - | + |   |   |   |   |     | diff 3+   |
                              0x20, //|   |   |   |   | + | - |   |   |     | diff 4+   |
                              0x60, //|   |   |   |   | - | + |   |   |     | diff 5+   |
                              0x30, //|   |   |   |   |   |   | + | - |     | diff 6+   |
                              0x70, //|   |   |   |   |   |   | - | + |     | diff 7+   |
                              0x80, //| + |   |   |   |   |   |   |   |  -  |    0      |
                              0xC0, //|   | + |   |   |   |   |   |   |  -  |    1      |
                              0x90, //|   |   | + |   |   |   |   |   |  -  |    2      |
                              0xD0, //|   |   |   | + |   |   |   |   |  -  |    3      |
                              0xA0, //|   |   |   |   | + |   |   |   |  -  |    4      |
                              0xE0, //|   |   |   |   |   | + |   |   |  -  |    5      |
                              0xB0, //|   |   |   |   |   |   | + |   |  -  |    6      |
                              0xF0, //|   |   |   |   |   |   |   | + |  -  |    7      |                 
                              };

                                       //        Input Range Selection 
                                       //| UNI | GAIN | INPUT RANGE|Input_Range|
  byte Input_Range_Selection[] ={0x00, //|  0  |  0   |   +/- 5V   |    0      |
                                 0x08, //|  1  |  0   |  0V to 5V  |    1      |
                                 0x04, //|  0  |  1   |   +/- 10V  |    2      |
                                 0x0C, //|  0  |  1   |  0V to 10V |    3      | 
                                 }; 

                                    //Power Down Selection 
                                    //| NAP | SLEEP | POWER DOWN MODE|Power_Mode| 
  byte Power_Selection[] =   {0x00, //|  0  |   0   |      Power On  |    0    |
                              0x02, //|  1  |   0   |         Nap    |    1    |
                              0x01, //|  X  |   1   |        Sleep   |    2    |
                              0x03, //                               |    3    |
                              };
                              
    if(!Differential_Mode)  // if differential mode was not slected then...
    {  
        A_D_Channel = A_D_Channel + 8;  // ...switch to single ended mode
    }

    byte A_D_Cmd = Channel_Selection[A_D_Channel] | Input_Range_Selection[Input_Range] | Power_Selection[Power_Mode];  //build the command to be sent to the AtoD

    //Throw away the first reading........................... 
    digitalWrite(CS_A_D, LOW);  //enable the AtoD on the SPI line
    delay(10);  //allow AtoD set-up time
    SPI.transfer(A_D_Cmd);  //transfer command byte
    SPI.transfer(0x00);  //transfer second command byte - all zero's
    delay(10);  //allow AtoD set-up time; 
    digitalWrite(10, HIGH);  //disable the AtoD on the SPI line

    //Start readings
    for(int j = 0; j < NumberOfReadings; j++) //Take the number of readings
    {
        digitalWrite(CS_A_D, LOW);  //enable the AtoD on the SPI line
        delay(10);  //allow AtoD set-up time
        data_array_1[j] = SPI.transfer(A_D_Cmd);  //transfer command byte
        data_array_2[j] = SPI.transfer(0x00);  //transfer second command byte - all zero's
        Counts_A_D = (data_array_1[j])<<8;  //shift data1 8 to the left and place into counts
        Counts_A_D = Counts_A_D | data_array_2[j];  //or data2 with data1 for complete counts word
        myStats.add(Counts_A_D);  //add the tempcounts to the stats heap 
        digitalWrite(CS_A_D, HIGH);  //disable the AtoD on the SPI line
    }

    if(NumberOfReadings > 1)           // if enough data points to average then...
    {
        Counts_A_D = myStats.average(); //...get the average number of counts... 
    }                                  //...otherwise just return the the counts Counts_A_D

    myStats.clear();                //clear the stats queue

    return Counts_A_D;  // return the counts to the caller
}

//***********************         SD Write/Read Function         *************************************************************
void Write_To_SD_Card(int chipSelect, int last_data)
{

  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  // open the file for write at end like the Native SD library
  if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
  // if the file opened okay, write to it:
  Serial.print("Writing to test.txt...");
  myFile.println(last_data);

  // close the file:
  myFile.close();
  Serial.println("done.");

  // re-open the file for reading:
  if (!myFile.open("test.txt", O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println("test.txt:");

  // read from the file until there's nothing else in it:
  int data;
  while ((data = myFile.read()) >= 0) Serial.write(data);
  // close the file:
  myFile.close();
} 

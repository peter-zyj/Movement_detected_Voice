/* 
 * /////////////////////////////////////////////////////////////
 * //Movement detected, and Voice triggered for the Notification
 * /////////////////////////////////////////////////////////////
 *
 * Switches a LED according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 *
 * @author: Yijunzhu/Carl Zhu
 * @date:   16. March 2015 
 *
 *  
 */

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;  
long unsigned int HornTime;

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime; 
boolean HornOn = false;
boolean itr = false;

int pirPin = 2;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;
int hornPin = 7;



/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(hornPin, OUTPUT);
  digitalWrite(pirPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(hornPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){

     if(HornOn && !itr){
//       Serial.print("diff::");
//       Serial.println(millis() - HornTime);
       if((millis() - HornTime) > 10000){
         digitalWrite(hornPin, LOW);
         HornOn = false;
         itr = true;
         Serial.print("debug:horn-timeout::");
         Serial.println(HornOn);
       }
     }
      
     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(!HornOn && !itr){
         digitalWrite(hornPin, HIGH);  //this is the one time triggered voice
         HornTime = millis();
         HornOn = true;
         Serial.print("debug:horn-timein::");
         Serial.println(HornOn);
         Serial.println(HornTime);
       }
        
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);   //delay(10*1000)//should extend the time to the max length of voice recording
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;  
           itr = false;           
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           }
       }
  }

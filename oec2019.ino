/*
//Ultrasonic sensor 
const int trigPin = 9;
const int echoPin = 10;
*/

//////////INCLUDES//////////

#include <Servo.h>

//////////CONSTANTS/////////

//time to execute a 90 degree turn
const int TURN_TIME = 200;

//speed of sound in cm/us
const float SPEED_OF_SOUND = 0.034;

//motors
const int LEFT_FORWARD = 0;
const int LEFT_BACKWARD = 1;
const int RIGHT_FORWARD = 2;
const int RIGHT_BACKWARD = 3;

//GPIO for sensor 1
const int TRIG_PIN1 = 8;
const int ECHO_PIN1 = 9;

//GPIO for sensor 2
const int TRIG_PIN2 = 10;
const int ECHO_PIN2 = 11;

//GPIO for servo
const int SERVO_PIN = 13;

//////////VARIABLES/////////

//sensor 1 detects potholes
//sensor 2 detects walls
int sensor1Distance;
int sensor2Distance;

//checking state
bool firstHoleFound = false;
bool secondHoleFound = false;
bool rightTurn = false;

//////////FUNCTIONS/////////

//TRIG_PIN1 should be set to output, ECHO_PIN1 should be set to input
//poll the ultrasonic sensor for distance in CM
void pollSensor1()
{
    long duration;
   
    //give a 10 us pulse to the trigger pin
    //give a short low pulse first to ensure a clean high pulse
    digitalWrite(TRIG_PIN1, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN1, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN1, LOW);
   
    //read echo pin for time until ultrasonic pulse received
    duration = pulseIn(ECHO_PIN1, HIGH);
   
    //calculate distance in CM
    //distance would be return path, so divide by 2 to get 1-way distance
    sensor1Distance = duration * SPEED_OF_SOUND / 2;
}
 
//TRIG_PIN2 should be set to output, ECHO_PIN2 should be set to input
//poll the ultrasonic sensor for distance in CM
void pollSensor2()
{
    long duration;
   
    //give a 10 us pulse to the trigger pin
    //give a short low pulse first to ensure a clean high pulse
    digitalWrite(TRIG_PIN2, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN2, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN2, LOW);
   
    //read echo pin for time until ultrasonic pulse received
    duration = pulseIn(ECHO_PIN2, HIGH);
   
    //calculate distance in CM
    //distance would be return path, so divide by 2 to get 1-way distance
    sensor2Distance = duration * SPEED_OF_SOUND / 2;
}


void forward()
{
  digitalWrite(RIGHT_FORWARD , HIGH);
  digitalWrite(RIGHT_BACKWARD , LOW);
  digitalWrite(LEFT_FORWARD , HIGH);
  digitalWrite(LEFT_BACKWARD , LOW);
}
 
void right()
{
    digitalWrite(RIGHT_FORWARD, LOW);
    digitalWrite(RIGHT_BACKWARD, HIGH);
    digitalWrite(LEFT_FORWARD, HIGH);
    digitalWrite(LEFT_BACKWARD, LOW);
}
 
void left()
{
    digitalWrite(RIGHT_FORWARD, HIGH);
    digitalWrite(RIGHT_BACKWARD, LOW);
    digitalWrite(LEFT_FORWARD, LOW);
    digitalWrite(LEFT_BACKWARD, HIGH);
}
 
void brake()
{
    digitalWrite(RIGHT_FORWARD, LOW);
    digitalWrite(RIGHT_BACKWARD, LOW);
    digitalWrite(LEFT_FORWARD, LOW);
    digitalWrite(LEFT_BACKWARD, LOW);
}
 
//make a 90 degree right turn
void turnRight()
{
    //brake the robot for more consistent turning
    brake();
    delay(250);
   
    //turn then brake (again, for more consistent turning)
    right();
    delay(TURN_TIME);
    brake();
}
 
//make a 90 degree left turn
void turnLeft()
{
    //brake the robot for more consistent turning
    brake();
    delay(250);
   
    //turn then brake (again, for more consistent turning)
    left();
    delay(TURN_TIME);
    brake();
}
 
//advance forward very slightly
void snakeForward()
{
    brake();
    delay(250);
    forward();
    delay(200);
    brake();
}

Servo servo;  

//dump one shot of sand
void dumpSand()
{
  if(!firstHoleFound)
  {
    //90 degrees for the first shot
    servo.write(90);
  }
  else
  {
    //180 degrees for the second shot
    servo.write(180);
  }
}

//Threads for ultrasonic sensors 
//TimedAction ultrasonicThread1 = TimedAction(10, pollSensor1);
//TimedAction ultrasonicThread2 = TimedAction(10, pollSensor2);
 
void setup() 
{
  //set motors to be stopped
  brake();
  
  //set up ultrasonic 
  pinMode(TRIG_PIN1, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN1, INPUT); // Sets the echoPin as an Input
  pinMode(TRIG_PIN2, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN2, INPUT); // Sets the echoPin as an Input
  
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(RIGHT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  Serial.begin(9600); // Starts the serial communication
  servo.attach(SERVO_PIN);
  servo.write(0);
}
 
void loop()
{
  //if the second hole has been found, stop moving
  if(secondHoleFound)
  {
    brake();
    Serial.println("Second hole found");
  }
  else
  {
    if(!firstHoleFound)
    {
      Serial.println("First hole not found");
      forward();
      //check if there's a pothole based on what sensor 1 returns
      pollSensor1();
      if(sensor1Distance > 5 && sensor1Distance < 10)
      {
        Serial.println("Pothole found");
        snakeForward();
        brake();
        dumpSand();
        turnRight();
        firstHoleFound = true;
      }
    }
    else
    {
      forward();
      //check if a pothole is found
      pollSensor1();
      if(sensor1Distance > 5 && sensor1Distance < 10)
      {
        snakeForward();
        brake();
        dumpSand();
        secondHoleFound = true;
      }
      //check for collision with a wall
      pollSensor2();
      if(sensor2Distance < 5)
      {
        if(rightTurn) 
        {
          turnRight();
          //check if a pothole is found
          pollSensor1();
          if(sensor1Distance > 5)
          {
            snakeForward();
            brake();
            dumpSand();
            secondHoleFound = true;
          }
          snakeForward();
          //check if a pothole is found
          if(sensor1Distance > 5)
          {
            snakeForward();
            brake();
            dumpSand();
            secondHoleFound = true;
          }
          turnRight();
          //check if a pothole is found
          if(sensor1Distance > 5)
          {
            snakeForward();
            brake();
            dumpSand();
            secondHoleFound = true;
          }
          rightTurn = false;
        }
        else
        {
          turnLeft();
          //check if a pothole is found
          pollSensor1();
          if(sensor1Distance > 5)
          {
            snakeForward();
            brake();
            dumpSand();
            secondHoleFound = true;
          }
          snakeForward();
          //check if a pothole is found
          if(sensor1Distance > 5)
          {
            snakeForward();
            brake();
            dumpSand();
            secondHoleFound = true;
          }
          turnLeft();
          //check if a pothole is found
          if(sensor1Distance > 5)
          {
            snakeForward();
            brake();
            dumpSand();
            secondHoleFound = true;
          }
          rightTurn = true;
        }
      }
    }
  }

  delay(10);
}

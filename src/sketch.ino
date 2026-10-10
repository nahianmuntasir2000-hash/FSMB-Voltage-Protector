#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Pin configuration
const byte V_PIN = A0;
const byte C_PIN = A1;
const byte R_PIN = 7;
const byte G_LED = 8;
const byte R_LED = 9;
const byte BUTTON = 2;

// Simulation ranges
const float MAX_VOLTAGE = 300;
const float MAX_CURRENT = 15;

// Default protection thresholds
float v_limit = 250;
float c_limit = 10;

float voltage = 0;
float current = 0;

bool fault = false;
bool relayOn = false;
bool lastButtonState = HIGH;

unsigned long lastSample = 0;
unsigned long lastReport = 0;

bool autoReport = true;  //Controls automatic status printing

char command[48];        //For command processing
byte commandIndex = 0;

//Function for the load to disconnect
void disconnectLoad() 
{
  relayOn = false;           //relayOn logic will be 0
  digitalWrite(R_PIN, LOW);  //Relay pin will be 0, so the relay will turn off
  digitalWrite(G_LED, LOW);  //Green LED will be turn OFF
  digitalWrite(R_LED, HIGH); //RED LED will be turn ON
}

//Function for the load to connect
void connectLoad() 
{
  relayOn = true;            //relayOn logic will be 1
  digitalWrite(R_PIN, HIGH); //Relay pin will be 1, so the relay will turn on
  digitalWrite(G_LED, HIGH); //Green LED will be turn ON
  digitalWrite(R_LED, LOW);  //RED LED will be turn OFF
}

//Function to check if the voltage and current is lower than the limit
bool isSafe() 
{
  return voltage <= v_limit && current <= c_limit;  //Will return 0 if the voltage and current is higher value
}

//Function to read the input values of voltage and current source
void readSensors() 
{
  int rawVoltage = analogRead(V_PIN); //Read the value of voltage
  int rawCurrent = analogRead(C_PIN); //Read the value of current

  voltage = rawVoltage * MAX_VOLTAGE / 1023;
  current = rawCurrent * MAX_CURRENT / 1023;
}

//Check the protection is on or not
void checkProtection() 
{
  if (isSafe() == 0)        
  {
    if (fault == 0) 
    {
      Serial.println("FAULT DETECTED!");       //if voltage and current is high this msg will be shown in serial monitor

      if (voltage > v_limit)
        Serial.println("Reason: OVERVOLTAGE"); //if voltage is high this msg will be shown

      if (current > c_limit)
        Serial.println("Reason: OVERCURRENT"); //if voltage is high this msg will be shown
    }

    fault = true;                              //fault logic will be 1
    disconnectLoad();                          //this function will be ON
  }
}

//Function to reset the relay
void resetProtection() 
{
  readSensors();

  if (isSafe() == 1) 
  {
    fault = false;
    connectLoad();
    Serial.println("RESET SUCCESSFUL - LOAD ON");
  } 
  else 
  {
    fault = true;
    disconnectLoad();
    Serial.println("RESET DENIED - FAULT STILL PRESENT");
  }
}

void printStatus() 
{
  Serial.print("Voltage: ");
  Serial.print(voltage, 1);
  Serial.print(" V | Current: ");
  Serial.print(current, 2);
  Serial.print(" A | Relay: ");
  Serial.print(relayOn ? "ON" : "OFF");
  Serial.print(" | Fault: ");
  Serial.println(fault ? "LATCHED" : "NONE");
}

//To process the cmmand given by the user
void processCommand(char *cmd) 
{
  for (char *p = cmd; *p; p++)
    *p = toupper((unsigned char)*p);

  if (strcmp(cmd, "STATUS") == 0) 
  {
    readSensors();
    checkProtection();
    printStatus();
  }
  else if (strcmp(cmd, "RESET") == 0) 
  {
    autoReport = true;
    resetProtection();
  }
  else if (strncmp(cmd, "SETV ", 5) == 0) 
  {
    char *end;
    float value = strtod(cmd + 5, &end);

    if (end != cmd + 5 && *end == '\0' && value >= 50 && value <= 300) 
    {
      v_limit = value;
      autoReport = true;
      Serial.print("Voltage limit: ");
      Serial.println(v_limit);
      readSensors();
      checkProtection();
    } 
    else 
    {
      Serial.println("Invalid voltage limit (50-300)");
    }
  }
  else if (strncmp(cmd, "SETI ", 5) == 0) 
  {
    char *end;
    float value = strtod(cmd + 5, &end);

    if (end != cmd + 5 && *end == '\0' && value > 0 && value <= 15) 
    {
      c_limit = value;
      autoReport = true;
      Serial.print("Current limit: ");
      Serial.println(c_limit);
      readSensors();
      checkProtection();
    } 
    else 
    {
      Serial.println("Invalid current limit (0-15]");
    }
  }
  else if (strcmp(cmd, "HELP") == 0) 
  {
    autoReport = false;
    Serial.println("Commands:");
    Serial.println("STATUS");
    Serial.println("SETV 250");
    Serial.println("SETI 10");
    Serial.println("RESET");
    Serial.println("HELP");
    Serial.println("Automatic status reporting PAUSED");
  }
  else 
  {
    Serial.println("Unknown command. Type HELP");
  }
}

//Function to handle serial monitor
void handleSerial() 
{
  while (Serial.available()) 
  {
    char c = Serial.read();

    if (c == '\n' || c == '\r') 
    {
      if (commandIndex > 0) 
      {
        command[commandIndex] = '\0';
        processCommand(command);
        commandIndex = 0;
      }
    }
    else if (commandIndex < sizeof(command) - 1) 
    {
      command[commandIndex++] = c;
    }
  }
}

void setup() 
{
  // Keep relay disconnected during startup
  digitalWrite(R_PIN, LOW);
  pinMode(R_PIN, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(R_LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(G_LED, LOW);
  digitalWrite(R_LED, HIGH);

  Serial.begin(9600);
  Serial.println("FSMB VOLTAGE PROTECTOR");
  Serial.println("Arduino UNO Simulation");
  Serial.println("Type HELP for commands");

  readSensors();

  if (isSafe() == 1) 
  {
    connectLoad();
    Serial.println("STARTUP SAFE - LOAD ON");
  } 
  else 
  {
    fault = true;
    disconnectLoad();
    Serial.println("STARTUP FAULT - LOAD OFF");
  }
}

void loop() 
{
  handleSerial();

  if (millis() - lastSample >= 50) 
  {
    lastSample = millis();
    readSensors();
    checkProtection();

    bool buttonState = digitalRead(BUTTON);

    if (lastButtonState == HIGH && buttonState == LOW) 
    {
      resetProtection();
    }

    lastButtonState = buttonState;
  }

  if (millis() - lastReport >= 1000)
 {
   lastReport = millis();

   if (autoReport == true)  // NEW: Print only when enabled
   {
     printStatus();
   }
 }
}

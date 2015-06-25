#include <Base64.h>
#include <SoftwareSerial.h>
#include <AES.h>
//#include <Time.h>

#define GET_CMD "GET /safe/a.php?a=1&s=2&c="
#define DEBUG true
#define GE ">"
#define SERVER_ADDRESS " HTTP/1.1\r\nHost: safe-proj.azurewebsites.net\r\n"
SoftwareSerial esp8266(10,9);
bool connectedToWifi = false;

#define DEFAULT_DELAY 1000 
int buttonPin = 11;
int buttonState = 0;
bool bDoorOpened = false; //false for close true for open 
bool bNFC = false;
unsigned long NFCtime;
AES aes ;
int index = 321; //should be saved on ROM
byte plain[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
} ;

byte plain64[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
} ;

byte cipher64[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
} ;

byte cipher [2*16] ;

void setup() {
  // Initialize serials
  Serial.begin(9600);
  esp8266.begin(57600); 
  pinMode(buttonPin, INPUT);
  reset_servo();
  cryptoSetup();
  bool success=false;
  int timesToRetry = 5;
  while (timesToRetry > 0)
  {
    success = connectWiFi();
    if (success) {
      //Serial.println("Connect WiFi success");
      connectedToWifi = true;
      break;
    } /*else {
      Serial.println("Problem connect to WiFi. Retrying"); 
    }*/
    timesToRetry-=1;
  }
  wifiOpenTCP();
  open_door();
  delay(1000);
  close_door();
  nfcSetup();
  cryptoSetup();
//  String s = decryptCommand("ITkDCq703CmqAMMbm4T2tg==");
//  Serial.println("Decrypt:");
//  Serial.println(s);  
  if (timesToRetry <= 0) Serial.println("No"); //No Wifi
}


void loop() {
  String cmd;
  String response;
  if (!connectedToWifi) return;
  //Add Here check button
  //Check how much time the poll is taking.
  pollServer();
  buttonState = digitalRead(buttonPin);
  if ((buttonState == LOW) && (bDoorOpened)) {
    close_door();
    delay(1000);
    cmd = GET_CMD;
    cmd+= encryptCommand('C');
    cmd+= SERVER_ADDRESS;
    response = wifiRequestSizeApproval(cmd.length()+2);      
    if (response.indexOf(GE) != -1) {
      response = sendDataAzure(cmd, 1000, DEBUG, true);
      /*if (DEBUG){
        Serial.println("Sent Opened_Door to Azure");
      }*/
    }
  } else if ((buttonState == HIGH) && (!bDoorOpened)) {
    cmd = GET_CMD;
    cmd+= encryptCommand('X');
    cmd+= SERVER_ADDRESS;
    response = wifiRequestSizeApproval(cmd.length()+2);      
    if (response.indexOf(GE) != -1) {
      response = sendDataAzure(cmd, 1000, DEBUG, true);
      /*if (DEBUG){
        Serial.println("Sent Opened_Door to Azure");
      }*/
    }   
  }
  if (bNFC){
    if (millis() - NFCtime > 10000){
      bNFC = false;
    } else{
      uint32_t uid = nfcCheck();
      if (uid != 0){
        cmd = GET_CMD;
        cmd+= encryptCommandNFC('A', uid);
        cmd+= SERVER_ADDRESS;
        response = wifiRequestSizeApproval(cmd.length()+2);      
        if (response.indexOf(GE) != -1) {
          response = sendDataAzure(cmd, 1000, DEBUG, true);
        /*if (DEBUG){
          Serial.println("Sent Opened_Door to Azure");
        }*/
        }
      }
    }
  }
      
}




void pollServer() {
  String cmd = GET_CMD;
  cmd+= encryptCommand('R');
  cmd+= SERVER_ADDRESS;
  String response = wifiRequestSizeApproval(cmd.length()+2);
  
  if (response.indexOf("bus") != -1) {//If not working return to busy inet
     Serial.println("Res s"); //Reset server
     delay(DEFAULT_DELAY);
     //resetWifi();
     wifiReset(); //Yoav Changed Here!!! Delte after review "busy inet..."
     wifiOpenTCP();
     delay(DEFAULT_DELAY);
  }
  
  //response = Send
  // Check if request size was approved by the module
  else if (response.indexOf("is n") != -1){//link is not
      wifiOpenTCP();
  }
  else if (response.indexOf(GE) != -1) {
    response = sendDataAzure(cmd, 1000, DEBUG, true);
      /*if (DEBUG){
        Serial.println(response);
      }*/
    if ((response.indexOf("SP") != -1)){
      /*if (DEBUG){
          Serial.println("Got Prepare NFC");
      }*/
      //Add NFC Code Here
      // Send Prepare NFC to the Server
      //TODO: Convert to function to be generic
      cmd = GET_CMD;
      cmd += encryptCommand('P'); 
      cmd+= SERVER_ADDRESS;
      response = wifiRequestSizeApproval(cmd.length()+2);      
      if (response.indexOf(GE) != -1) {
        response = sendDataAzure(cmd, 1000, DEBUG, true);
        /*if (DEBUG){
          Serial.println("Sent PREPARED_NFC");
        }*/
        // We need to apply here a timer to check if weve got a NFC TAG (We need to discuss it together)
      }
      bNFC = true;
      NFCtime = millis();
    }
    else if ((response.indexOf("SN") != -1)){
      if (DEBUG){
          Serial.println("SN");
      }
    }
    else if ((response.indexOf("SO") != -1)){
        /*if (DEBUG){
          Serial.println("Sent Opened_Door to Azure");
        }*/
      open_door();
      cmd = GET_CMD;
      cmd += encryptCommand('O');
      cmd += SERVER_ADDRESS;
      response = wifiRequestSizeApproval(cmd.length()+2);      
      if (response.indexOf(GE) != -1) {
        response = sendDataAzure(cmd, 1000, DEBUG, true);
        /*if (DEBUG){
          Serial.println("Sent Opened_Door to Azure");
        }*/
      }
      delay(5000);
    }
      else if ((response.indexOf("SC") != -1)){
        /*if (DEBUG){
          Serial.println("Close"); //Close Door
        }*/
      close_door();
      delay(1000);
      cmd = GET_CMD;
      cmd += encryptCommand('C');
      cmd+= SERVER_ADDRESS;
      response = wifiRequestSizeApproval(cmd.length()+2);      
      if (response.indexOf(GE) != -1) {
        response = sendDataAzure(cmd, 1000, DEBUG, true);
        /*if (DEBUG){
          Serial.println("Open"); //Open door
        }*/
      }
    }
  }
}


bool sendCheckOK(String command, const int timeout, boolean debug) {
  String response = "";
  response = sendData(command, timeout, debug);
  if (response.indexOf("OK") == -1) return false;
    return true;
}

String sendDataAzure(String command, const int timeout, boolean debug, boolean encryption) {

    String response;
    esp8266.print(command+ "\r\n"); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
          if (esp8266.find("HERE")){  
            response = esp8266.readStringUntil('\n');
            break;        
          }
      }
    }
    /*if (debug){
      Serial.println(response);
    }*/
    if (encryption) {
      String s = decryptCommand(response);
      //Serial.print(s);
      return s;
    }
    else{
      return response;
    }
}

String sendData(String command, const int timeout, boolean debug) {
    String response = "";
    esp8266.print("AT+" + command + "\r\n"); // send the read character to the esp8266
    
    long int time = millis();
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        // The esp has data so display its output to the serial window
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    if(DEBUG) {
      Serial.print(response);
    }
    
    return response;
}

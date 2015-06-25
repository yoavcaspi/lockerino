

// Network configurations
#define SSID "AndroidAP"
#define PASS ""
#define SERVER_IP "safe-proj.azurewebsites.net"
#define SERVER_PORT "80"


boolean connectWiFi() {
  wifiReset();
  wifiConfigAP();
  int result = wifiConnectToNetwork();
  if (DEBUG) wifiGetIP();
  return result;
}


bool wifiOpenTCP() {
  String cmd = "CIPSTATUS";
  String response = sendData(cmd, 4000, DEBUG);
  if (response.indexOf("STATUS:3") != -1){
    return true;
  }  
  cmd = "CIPSTART=\"TCP\",\"";
  cmd += SERVER_IP;
  cmd += "\",";
  cmd += SERVER_PORT;
  //Check the length of response if needed
  response = sendData(cmd, DEFAULT_DELAY, DEBUG);
  if (response.indexOf("OK") == -1 && response.indexOf("ALREAY") == -1) {
    Serial.println("Cant con Ser"); //Cant connect to server
    
    // TODO: Check what is the best way to handle "busy inet..."
    if (response.indexOf("busy") != -1) {//busy inet
      Serial.println("Res serv"); //Reset server
      delay(DEFAULT_DELAY);
      //resetWifi();
      wifiReset(); //Yoav Changed Here!!! Delte after review "busy inet..."
      delay(DEFAULT_DELAY);
    } else {
      wifiReset(); // If Working Combine the two if else together
      Serial.println("Run deb");
      return false;
    }
  }
}

void wifiUnlink() {
  sendData("CIPCLOSE", DEFAULT_DELAY, DEBUG);
}

void wifiReset() {
  sendData("RST",DEFAULT_DELAY,DEBUG); // reset module
}

void wifiConfigAP() {
  sendData("CWMODE=1",6000,DEBUG); // configure as access point
}

int wifiConnectToNetwork() {
  String cmd="CWJAP=\"";  
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  return sendCheckOK(cmd, 6000, DEBUG);
}

void wifiGetIP() {
  sendData("CIFSR",DEFAULT_DELAY,DEBUG); // get ip address
}

String wifiRequestSizeApproval(int length) {
  String requestSizeCMD = "CIPSEND=";
  requestSizeCMD += length;
  return sendData(requestSizeCMD, 500, DEBUG);
}


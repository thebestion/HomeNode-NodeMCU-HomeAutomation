
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
const char* ssid = "SSIDNAME"; // Change Router Name
const char* password = "PASSWORD"; //  Change Wifi Password
WiFiServer server(80);
//ESP8266WebServer server(80);

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

void setup() 
{
  Serial.begin(9600);
  mySwitch.enableTransmit(D4);
  mySwitch.setProtocol(1);
  
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  // config static IP
  IPAddress ip(192, 168, 1, 105); // Change IP (optional)
  IPAddress gateway(192, 168, 1, 1); 
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  // connecting 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.println(WiFi.localIP());  
}

void loop() 
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  String code = getValue(getValue(req, '/', 1), ' ', 0);  
  String letter = getValue(getValue(req, '/', 2), ' ', 0);  
  String status = getValue(getValue(req, '/', 3), ' ', 0);    
  char c[] = {code[0], code[1], code[2], code[3], code[4]};
  char l[] =  {letter[0], letter[1], letter[2], letter[3], letter[4]};  
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/json\r\n\r\n");
  if(status == "1"){
    Serial.println(letter);
    Serial.println(code);
    mySwitch.switchOn(c, l);
    client.print("{\"message\":\"success\", \"status\":\"on\"}");
  }else if(status == "0"){
    Serial.println(letter);
    Serial.println(code);
    mySwitch.switchOff(c, l);
    client.print("{\"message\":\"success\", \"status\":\"off\"}");
  }else{
    client.print("{\"message\":\"error\"}");
  }  
  delay(1);
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

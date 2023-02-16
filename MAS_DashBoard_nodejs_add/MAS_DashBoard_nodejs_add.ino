#include <ESP8266WiFi.h>        // 本程序使用 ESP8266WiFi庫
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>   // ESP8266WiFiMulti庫
#include <ESP8266WebServer.h>   // ESP8266WebServer庫
#include <SoftwareSerial.h>     // 讓NodeMCU可以跟Arduino溝通

#define rxPin D1
#define txPin D2
#define wifi_id "Aaron"
#define password "88888888"

SoftwareSerial ESP_softSerial(rxPin,txPin);   //建立軟體串列埠腳位 (RX, TX)
ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti對象,對象名稱是'wifiMulti'
ESP8266WebServer esp8266_server(80);// 建立網絡服務器對象，該對像用於響應HTTP請求。監聽端口（80）

// Set Global variable data
char c;
String dataIn;
bool Send_allow = LOW;
String serverName = "140.116.82.45";
int8_t indexOfA, indexOfB, indexOfC, indexOfD;
String Flame_status, Tilt_status, Motor_status, Power_status;
String Flame_status_pre = "", Tilt_status_pre = "", Motor_status_pre = "", Power_status_pre = "";
 
void setup(){
  Serial.begin(115200);         // open serial communication (NodeMCU -> PC)
  ESP_softSerial.begin(115200); // open serial communication (NodeMCU -> Arduino)
  delay(10);                     
  Serial.println("started...");
 
  wifiMulti.addAP(wifi_id, password);
  Serial.println("Connecting...");


  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 此處的wifiMulti.run()是重點。通過wifiMulti.run()，NodeMCU將會在當前
    delay(1000);                             // 環境中搜索addAP函數所存儲的WiFi。如果搜到多個存儲的WiFi那麼NodeMCU
    Serial.print(i++); Serial.print(' ');    // 將會連接信號最強的那一個WiFi信號。
  }                                          // 一旦連接WiFI成功，wifiMulti.run()將會返回“WL_CONNECTED”。這也是
                                             // 此處while循環判斷是否跳出循環的條件。
  // WiFi連接成功後將通過串口監視器輸出連接成功信息 
  Serial.println('\n');                     // WiFi連接成功後
  Serial.print("Connected to ");            // NodeMCU將通過串口監視器輸出。
  Serial.println(WiFi.SSID());              // 連接的WiFI名稱
  Serial.print("IP address:\t");            // 以及
  Serial.println(WiFi.localIP());           // NodeMCU的IP地址
  
  esp8266_server.begin();                  
  esp8266_server.on("/", handleRoot);      
  esp8266_server.onNotFound(handleNotFound);        
 
  Serial.println("HTTP esp8266_server started");//  告知用戶ESP8266網絡服務功能已經啟動
}
 
void loop(){
  esp8266_server.handleClient();     // 處理http服務器訪問
  
  while(ESP_softSerial.available()>0) //不斷loop直到ESP接收到另一個板子傳來的訊息
    {
        c = ESP_softSerial.read();

        // Read multiple data and store into string
        if(c=='\n') {break;}
        else        {dataIn+=c;}
    }

  if(c=='\n')
    {
        Parse_the_Data();

        //Show all data to serial monitor
        Serial.println("Flame status: " + Flame_status);
        Serial.println("Tilt status: " + Tilt_status); 
        Serial.println("Motore status: " + Motor_status);
        Serial.println("Power status: " + Power_status);
        Serial.println("======================================");
        // delay(500); 
        if((Flame_status.compareTo(Flame_status_pre) != 0) || (Tilt_status.compareTo(Tilt_status_pre) != 0) || (Power_status.compareTo(Power_status_pre) != 0)){
          Send_allow = HIGH;
          Flame_status_pre = Flame_status;
          Tilt_status_pre = Tilt_status;
          Power_status_pre = Power_status;
        }
        // Reset the variable
        c = 0;
        dataIn = "";

        if(Send_allow){
          post_to_nodejs(json_pro());
          Send_allow = LOW;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Parse_the_Data()
{
    indexOfA = dataIn.indexOf("A");
    indexOfB = dataIn.indexOf("B");
    indexOfC = dataIn.indexOf("C");
    indexOfD = dataIn.indexOf("D");

    Flame_status = dataIn.substring (0, indexOfA);
    Tilt_status  = dataIn.substring (indexOfA+1, indexOfB);
    Motor_status = dataIn.substring (indexOfB+1, indexOfC);
    Power_status = dataIn.substring (indexOfC+1, indexOfD); 
}


/* 以下函數處理網站首頁的訪問請求 */    
void handleRoot() {   //處理網站目錄“/”的訪問請求 
  esp8266_server.send(200, "text/html", sendHTML(Flame_status,Tilt_status,Motor_status,Power_status));
  
}
 
/*
建立用於發送給客戶端瀏覽器的HTML代碼。此代碼將會每隔5秒刷新頁面。
通過頁面刷新，引腳的最新狀態也會顯示於頁面中
*/
String sendHTML(String Flame_status, String Tilt_status, String Motor_status, String Power_status){
  
  String htmlCode = "<!DOCTYPE html> <html>\n";
  htmlCode +="<meta charset=\"utf-8\">\n";
  htmlCode +="<head><meta http-equiv='refresh' content='1'/>\n";
  htmlCode +="<title>機台警報模擬系統</title>\n";
  htmlCode +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  htmlCode +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  htmlCode +="</style>\n";
  htmlCode +="</head>\n";
  htmlCode +="<body>\n";
  htmlCode +="<h1>機台警報模擬系統</h1>\n";
  
  if(Flame_status=="1")
    {htmlCode +="<p>火焰感測: 偵測到火焰!!!</p>\n";}
  else
    {htmlCode +="<p>火焰感測: 正常</p>\n";}

  if(Tilt_status=="1")
    {htmlCode +="<p>傾斜偵測: 偵測到傾斜!!!</p>\n";}
  else
    {htmlCode +="<p>傾斜偵測: 正常</p>\n";}

  if(Motor_status=="1")
    {htmlCode +="<p>馬達狀態: 運轉中...</p>\n";}
  else
    {htmlCode +="<p>馬達狀態: 待機</p>\n";}

  if(Power_status=="1")
    {htmlCode +="<p>電源狀態: 供電中...</p>\n";}
  else
    {htmlCode +="<p>電源狀態: 未供電，請接上電源</p>\n";}
  
  htmlCode +="</body>\n";
  htmlCode +="</html>\n";
  
  return htmlCode;
}
 
// 設置處理404情況的函數'handleNotFound'
void handleNotFound(){                                        // 當瀏覽器請求的網絡資源無法在服務器找到時，
  esp8266_server.send(404, "text/plain", "404: Not found");   // NodeMCU將調用此函數。
}

String json_pro(){
  String str;
  str = "{\"flame\":" + String(Flame_status) + ",\"tilt\":" + String(Tilt_status) + 
    ",\"motor\":" + String(Motor_status) + ",\"power\":" + String(Power_status) + "}";

  return str;
}

void post_to_nodejs(String post_json){
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, serverName.c_str(), 9453, "/json"); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST(post_json);

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  }
}

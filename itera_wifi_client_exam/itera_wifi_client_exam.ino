#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define TAG_SIZE 32

ESP8266WiFiMulti WiFiMulti;
String lifebit="1";
  
  typedef struct {
    String _name;
    float _value;
  }tag_struct;
  
  typedef struct  {
    String ckey;
    int conn_status;
    tag_struct tag[TAG_SIZE];
  }data_struct;

data_struct send_data;
data_struct receive_data;
String json_send;
int tickle=0;
int counter_ai[TAG_SIZE];
int ssid_index;
int gl_tag_index=0;
boolean connectioWasAlive = true;

void setup() {
          //ADD YOUR TAG AND CKEY//////////////////////
          ite_init();
          send_data.ckey="dbc53ffbffe4aef9d4ef2e37521c2d9a55b493462c4dbf6596a4bf02291abac6"; //<= PUT YOUR CKEY HERE
          ite_AddTag("tag1"); //ADD YOUR TAG NAME
          ite_AddTag("tag2"); //ADD YOUR TAG NAME
          ite_AddTag("tag3"); //ADD YOUR TAG NAME
          ite_AddTag("tag4"); //ADD YOUR TAG NAME
          ite_AddTag("tag5"); //ADD YOUR TAG NAME
          ite_AddTag("tag6"); //ADD YOUR TAG NAME
          ite_AddTag("tag7"); //ADD YOUR TAG NAME
          ite_AddTag("tag8"); //ADD YOUR TAG NAME
          ////////////////////////////////////////////
          //Pin Part/////////////////////////////////
           pinMode(2, OUTPUT);          
          //Wifi part ///////////////////////////////
          int numberofAP = WiFi.scanNetworks();
          Serial.begin(115200);
          delay(3000);
          Serial.println(" wifi Networks: ");
          for (int k = 0; k < numberofAP; k++)
        {
          Serial.print("(");
          Serial.print(k);
          Serial.print(")");
          Serial.println(WiFi.SSID(k));
        }
          
          Serial.println();
          Serial.println();
          Serial.println();
        
          for (uint8_t t = 4; t > 0; t--) {
            Serial.printf("[WIFI CONNECT] WAIT %d...\n", t);
            Serial.flush();
            delay(1000);
          }
          WiFi.mode(WIFI_STA);
          WiFiMulti.addAP("Century 03","tu1den10");
          WiFiMulti.addAP("FPT Telecom","tu1den10");
          WiFiMulti.addAP("Passio Coffee","");
          WiFiMulti.addAP("Itera.cc","goldchest");
          WiFiMulti.addAP("0892132-Itera.cc","d-goldchest");        
        }
        void monitorWiFi()
        {
          if (WiFiMulti.run() != WL_CONNECTED)
          {
            if (connectioWasAlive == true)
            {
              connectioWasAlive = false;
              Serial.print("Looking for WiFi ");
            }
            Serial.print(".");
            delay(500);
          }
          else if (connectioWasAlive == false)
          {
            connectioWasAlive = true;
            Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
          }
          else
          {
             Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
          }
}
void loop() { 
    monitorWiFi();
    ite_polling(); //<- For polling server
    //YOUR CODE HERE/////////////////////////////////////////////////
    ite_putval("tag1",ite_getval("tag2"));
    if(ite_getval("tag2")==1.0){
        digitalWrite(2, HIGH);
    }else{
        digitalWrite(2, LOW);
    }
    delay(200);
}
void ite_polling(){
  //json encode////////////////////////////
  json_encode();
  Serial.print("JSON for send: \n");
  Serial.print(json_send);
  Serial.println();
    // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    //Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://itera.cc/0_itera_tag_srv/iteara_tag_srv.php")) {  // HTTP


    //Serial.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      http.addHeader("Content-type", "application/x-www-form-urlencoded");
      int httpCode = http.POST(json_send);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.print("Receive data: ");
          Serial.println(payload);
          json_decode(payload);
        }
        
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
}
void ite_init(){
  int i;
  for(i=0;i<TAG_SIZE;i++){
    send_data.tag[i]._name = "undefine";
    send_data.tag[i]._value= 0.0;
    receive_data.tag[i]._name = "undefine";
    receive_data.tag[i]._value =0.0;
  }
}
void ite_AddTag(String tagname){
  send_data.tag[gl_tag_index]._name = tagname;
  receive_data.tag[gl_tag_index]._name=tagname;
  if(gl_tag_index<TAG_SIZE){
    gl_tag_index++;
  }
}
void ite_putval(String tagname,float tagvalue){
  int i;
  for(i=0;i<TAG_SIZE;i++){
    if(send_data.tag[i]._name==tagname){
      send_data.tag[i]._value=tagvalue;
    }
  }
}

float ite_getval(String tagname){
  int i;
  for(i=0;i<TAG_SIZE;i++){
    if(receive_data.tag[i]._name==tagname){
      return receive_data.tag[i]._value;
    }
  }
}
void json_encode(){
  //change status for connection;
  if(tickle==1){
   tickle=0;
  }else if(tickle==0){
   tickle=1;
  }
  send_data.conn_status=tickle;
  json_send = json_make(send_data); 
}
String json_make(data_struct data0){
  int i;
  String temp_json;
  temp_json = "data={\"ckey\":\""+ data0.ckey +"\",\"conn_status\":"+ data0.conn_status + ",\"tag\":{";
  for(i=0;i<gl_tag_index;i++){
    if(i<gl_tag_index-1){
    temp_json = temp_json + "\"" + data0.tag[i]._name + "\":" +  data0.tag[i]._value + "," ;
    }else
    {
     temp_json = temp_json + "\"" + data0.tag[i]._name + "\":" +  data0.tag[i]._value; 
    }
  }
  temp_json = temp_json + "}}";
  return temp_json;
}
void json_decode(String json_data){
 //  Serial.print("Lenght of JSON : ");
 //  Serial.println(json_data.length());
   int total_lenght=json_data.length();
   int i,j;
   int first_quote = 0;
   int next_is_value =0;
   int tag_index=0;
   String ckey,conn_status,temp_str,maybe_tag_name,tag_name,maybe_tag_value,tag_value;
   for(i=10;i<74;i++){
     ckey=ckey+json_data[i];
   }
  //  Serial.println(ckey); 
    conn_status = json_data[90];
   // Serial.print("Connection Status: ");
  //  Serial.println(conn_status);  
    for(i=99;i<total_lenght;i++){
     switch (json_data[i]){
      case '\"':              // "
       if(next_is_value !=1){  //next is not value
          if(first_quote==0){
            first_quote = 1;
          }else{
            maybe_tag_name = temp_str;
            first_quote=0;
          }
       }else{   //next is value
          if(first_quote==0){
            first_quote = 1;
          }else{
            maybe_tag_value = temp_str;
            first_quote=0;
          }
       }
      break;
      case ':':
         tag_name = maybe_tag_name;
         receive_data.tag[tag_index]._name = tag_name;
         tag_name ="";
         next_is_value =1;
         temp_str="";
      break;
      case ',': 
      case '}':
         tag_value = maybe_tag_value;
         receive_data.tag[tag_index]._value = tag_value.toFloat();
         tag_value="";
         maybe_tag_value="";
         tag_index++;
         next_is_value =0;
         temp_str="";
      break;
      default:  //normal character
         if(json_data[i]!='}'){
          temp_str = temp_str + json_data[i];
         }
      break;
     }
   } 
}

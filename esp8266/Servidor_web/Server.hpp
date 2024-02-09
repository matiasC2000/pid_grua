ESP8266WebServer server(80);
#include "pags.hpp" 


void control(){
  String s;
  String led = server.arg(String("led"));
  if(led.length()>0){
    
        if (led == "1"){
         
          int estado_led1 = digitalRead(4);
          digitalWrite(4,!estado_led1);
          estado_led1 = digitalRead(4);
          if(estado_led1==1){
            clase1="ON";
          }else{
            clase1 ="OFF";
          }

       }

        if (led == "2"){
         
          int estado_led2 = digitalRead(0);
          digitalWrite(0,!estado_led2);
          estado_led2 = digitalRead(0);
          if(estado_led2==1){
            clase2="ON";
          }else{
            clase2 ="OFF";
          }

       }

           
  }

  
  s = p_inicio();
  server.send(200, "text/html",s );
}



void InitServer()
{
   server.on("/", control);
   server.on("/action", control); 


   // Iniciar servidor
   server.begin();
   Serial.println("Servidor Iniciado");
}

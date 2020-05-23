# itera-esp8266-lib
Library example for Arduino Sketch ESP8266 

Functions Description:
void ite_init()  : For setup data before communicate
void ite_AddTag('tagname') : Add new Tag with "tagname" is string style
void ite_putval('tagname',value) : put value to server itera.cc with 'tagname' (string), value (float)
float ite_getval('tagname') : Get value (float) of Tag with 'tagname'
void  ite_polling() : Communicate with iteara.cc

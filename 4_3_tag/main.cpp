#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <cmath>



Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);

BufferedSerial uart(D1,D0);
DigitalIn encoder(D11);
DigitalInOut pin10(D10);
BBCar car(pin5, pin6, servo_ticker);

void set_cm(Arguments *in, Reply *out);
RPCFunction rpcset_cm(&set_cm, "set_cm");

void park(Arguments *in, Reply *out);
RPCFunction rpcPark(&park, "park");

void trace(Arguments *in, Reply *out);
RPCFunction rpcTrace(&trace, "trace");

void findTag(Arguments *in, Reply *out);
RPCFunction rpcfindTag(&findTag, "findTag");



volatile int steps;
volatile int last;

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

void findTag(Arguments *in, Reply *out) {
   int x_pos = in->getArg<int>();
   int theta = in->getArg<int>();
   printf("in\n\r");

   
   if (1.5 <= x_pos) { // turn right
      car.turn_normp(20, 0.4);
   } else if (-1.5 >= x_pos) { //
      car.turn_normp(20, -0.4);
   } else if (-1.5 < x_pos && x_pos < 1.5) { //
      car.turn_normp(20, 1);
   }  else {
      car.stop();
   }

   /*
   if ( 350 <= theta && theta < 360) { // turn right
      car.turn_normp(20, -0.5);
   } else if (340 <= theta && theta < 350) { //
      car.turn_normp(20, -0.95);
   } else if (0 <= theta && theta < 10) { // turn left  
      car.turn_normp(20, 0.5);
   } else if (10 <= theta && theta < 20) { // turn left
      car.turn_normp(20, 0.95);
   } else {
      car.stop();
   }
   */

   ThisThread::sleep_for(20ms);
   printf("theta = %d\r\n", theta);
   //return;
}

void trace(Arguments *in, Reply *out) {
   double theta = in->getArg<int>();
   double cos_theta;

   char buffer[200], outbuf[256];
   char strings[20];

   printf("in\n");
   if (theta < 90) {
      printf("< 90\n");
      cos_theta = cos(theta);
      printf("cos\n");

      sprintf(strings, "/turn/run 100 -0.6 \n");
      //sprintf(strings, "/turn/run 55 %f \n", cos_theta);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      //ThisThread::sleep_for(1000ms);
      
      //sprintf(strings, "/stop/run \n");
      //strcpy(buffer, strings);
      //RPC::call(buffer, outbuf);
   }  else {
      printf("> 90\n");
      theta = 180 - theta;
      cos_theta = cos(theta);
      printf("cos\n");

      sprintf(strings, "/turn/run 100 0.6 \n");
      //sprintf(strings, "/turn/run 55 -%f \n", cos_theta);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      //ThisThread::sleep_for(1000ms);

      //sprintf(strings, "/stop/run \n");
      //strcpy(buffer, strings);
      //RPC::call(buffer, outbuf);

   }
   ThisThread::sleep_for(1500ms);
   sprintf(strings, "/stop/run \n");
   strcpy(buffer, strings);
   RPC::call(buffer, outbuf);
}

void park(Arguments *in, Reply *out) {
   int d1 = in->getArg<int>();
   int d2 = in->getArg<int>();
   char dir = in->getArg<char>();

   char buffer[200], outbuf[256];
   char strings[20];

   if (dir == 'E') {
      d1 += 28;
      d2 += 35;

      sprintf(strings, "/turn/run 55 0.1 \n", d2);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(7000ms);

/*
      RPC::call("/turn/run 90 0.1 \n", outbuf);
      ThisThread::sleep_for(1500ms);
      RPC::call("/stop/run \n"), outbuf);
*/

      sprintf(strings, "/turn/run 90 0.1 \n");
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(1500ms);
      sprintf(strings, "/stop/run \n");
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);

      sprintf(strings, "/set_cm/run %d \n", d1);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(8000ms);


   } else if (dir == 'W') {

      d1 += 28;
      d2 += 35;

      sprintf(strings, "/set_cm/run %d \n", d2);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(7000ms);

      //RPC::call("/turn/run 90 -0.2 \n", outbuf);
      sprintf(strings, "/turn/run 90 -0.2 \n");
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(950ms);
      //RPC::call("/stop/run \n"), outbuf);
      sprintf(strings, "/stop/run \n");
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      

      sprintf(strings, "/set_cm/run %d \n", d1);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(8000ms);
   } else if (dir = 'S') {
      d1 = d1 + 28;
      
      sprintf(strings, "/set_cm/run %d \n", d2);
      strcpy(buffer, strings);
      RPC::call(buffer, outbuf);
      ThisThread::sleep_for(8000ms);
   }
}

void set_cm(Arguments *in, Reply *out) {
   int len = in->getArg<int>();
   len = len - 6;
   
   steps = 0;
   last = 0;
   car.goStraight(-55);
   //int len =  50;
   while(steps*6.5*3.14/32< len) {
      //printf("encoder = %d\r\n", steps);
      ThisThread::sleep_for(100ms);
   }
   car.stop();
}

int main() {

   encoder_ticker.attach(&encoder_control, 10ms);

   char buf[256], outbuf[256];
   uart.set_baud(9600);



   parallax_ping  ping1(pin10);

   while(1) {
      printf("%d\r\n", (int)ping1);
      for (int i=0; ; i++) {
         char *recv = new char[1];
         //printf("stop?\r\n");
         uart.read(recv, 1);
         buf[i] = *recv;
         if (*recv == '\n') {
            break;

         }
      }
      //printf("where?\r\n", buf);
      if((int)ping1>20){
         printf("%s\r\n", buf);
         RPC::call(buf, outbuf);
      } else {
         car.stop();
         printf("out\r\n");
      }
      ThisThread::sleep_for(10ms);
   }

/*
   while(1) {
      printf("%d\r\n", (int)ping1);
      if((int)ping1>20){
         for (int i=0; ; i++) {
            char *recv = new char[1];
            uart.read(recv, 1);
            buf[i] = *recv;
            if (*recv == '\n') {
               break;
            }
         }
         printf("%s\r\n", buf);
         RPC::call(buf, outbuf);
      } else {
         car.stop();
         printf("out\r\n");
      }
      ThisThread::sleep_for(10ms);
   }
*/

   /*
   while(1){
      //printf("i'm in\n");
      for (int i=0; ; i++) {
         char *recv = new char[1];
         uart.read(recv, 1);
         buf[i] = *recv;
         if (*recv == '\n') {
         break;
         }
      }
      printf("%s\r\n", buf);
      RPC::call(buf, outbuf);
   }
   */

}
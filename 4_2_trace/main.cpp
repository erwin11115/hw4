#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <cmath>

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
//BufferedSerial xbee(D1, D0);
BufferedSerial uart(D1,D0);
DigitalIn encoder(D11);

BBCar car(pin5, pin6, servo_ticker);

void set_cm(Arguments *in, Reply *out);
RPCFunction rpcset_cm(&set_cm, "set_cm");

void park(Arguments *in, Reply *out);
RPCFunction rpcPark(&park, "park");

void trace(Arguments *in, Reply *out);
RPCFunction rpcTrace(&trace, "trace");


volatile int steps;
volatile int last;

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
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
   //FILE *devin = fdopen(&xbee, "r");
   //FILE *devout = fdopen(&xbee, "w");
   /*
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }
   RPC::call(buf, outbuf);
   }*/
   uart.set_baud(9600);
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

      //printf("%s\r\n", outbuf);
      //ThisThread::sleep_for(1s);
   }

}
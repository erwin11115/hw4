#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
DigitalIn encoder(D11);

BBCar car(pin5, pin6, servo_ticker);

void set_cm(Arguments *in, Reply *out);
RPCFunction rpcset_cm(&set_cm, "set_cm");

void park(Arguments *in, Reply *out);
RPCFunction rpcPark(&park, "park");


volatile int steps;
volatile int last;

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
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

      sprintf(strings, "/set_cm/run %d \n", d2);
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

   /*   // please contruct you own calibration table with each servo
   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-17.220, -16.837, -15.690, -12.118, -5.612, 0.000, 6.888, 12.883, 15.817, 16.710, 17.093};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-16.838, -16.327, -15.179, -11.863, -5.612, 0.000, 7.016, 12.756, 15.562, 16.072, 16.838};
   */
   // first and fourth argument : length of table
   //car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
   encoder_ticker.attach(&encoder_control, 10ms);

   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
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
   }
}
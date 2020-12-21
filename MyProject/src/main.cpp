#include <mbed.h>
#include "LIS3DSH.h"


DigitalOut myledo(LED3);
DigitalOut myledg(LED4);
DigitalOut myledr(LED5);
DigitalOut myledb(LED6);
static int sitUpCounter = 0;
static int pushUPCounter = 0;
static int jumpingCounter = 0;
static int squatCounter = 0;
LIS3DSH acc(PA_7, PA_6, PA_5, PE_3);
InterruptIn button(PA_0);
Serial uart(PC_6,PC_7);

int8_t state = 0;//state enum 0:siteup 1:pushup 2:jump 3:squat

//startState:when a action move to the end point,it turns to 1, when the action move back it turns to 0,
//guarantee completing a whole action
int8_t startState = 0;

//step to nect action
void buttonISr(){

    myledr = 1;
    myledg = 1;
    myledo = 1;
    myledb = 1;

    state++;
    if (state>3)
    {
      state = 0;
    }
    startState = 0;
    wait_ms(1000);

    myledr = 0;
    myledg = 0;
    myledo = 0;
    myledb = 0;
    }




int main() {

while(1) {
    // put your main code here, to run repeatedly:
    button.fall(&buttonISr);
    int16_t x,y;
    int16_t zAccel = 0; //acceleration in z (raw)
    float g_z = 0;  //acceleration in z(g force)
    double angle = 0; //angle reletive to z axis
    const float PI = 3.1415926;
    volatile double anglev = 0;

    const uint8_t N = 20; // filter length
    float ringbuf[N]; // sample buffer
    uint8_t ringbuf_index = 0; // index to insert sample





    while (acc.Detect() != 1) {
      printf("Could not detect Accelerometer\n\r");
      wait_ms(200);
    }

    int count = 0;
    while (1) {
      acc.ReadData(&x, &y, &zAccel);

      // normalise to 1g
      g_z = (float)zAccel / 17694.0;

      // ringbuf[ringbuf_index++] = g_z;

      //   // at the end of the buffer, wrap around to the beginning
      // if (ringbuf_index >= N) {
      //   ringbuf_index = 0;
      // }

      //       // START OF FILTERING
      // float g_z_filt = 0;

      // // add all samples
      // for (uint8_t i = 0; i < N; i++) {
      //   g_z_filt += ringbuf[i];
      // }


      // // divide by number of samples
      // g_z_filt /= (float)ringbuf_index;


      // // restrict to 1g (acceleration not decoupled from orientation)
      // if (g_z_filt > 1) {
      //   g_z_filt = 1;
      // }





      // compute angle in degrees
      angle = 180 * acos(g_z) / PI;   
      //avoid optimized by IDE
      anglev = angle;   
      
      switch (state)
      {
      case 0:
            //siteUp
            if (angle > 80)
            {
              myledo = 1;
              if (startState == 1)
              {
                sitUpCounter++;
                startState = 0;
              }
              
            }else if (angle<20)
            {
              myledo = 0;
            }else if (angle>=40 || angle<=60 ){
                startState = 1;
            }
            
            //when doing sitUp 5 times, chenge to pushUp
            if (sitUpCounter>= 5)
            {
              myledr = 1;
              myledg = 1;
              myledo = 1;
              myledb = 1;
              state = 1;
              startState = 0;
              wait_ms(1000);
            }
        break;
      
      case 1:
            //pushUp
            if (angle>25 && angle<40)
            {
                myledr = 0;
                myledg = 0;
                myledo = 0;
                myledb = 0;
              if (startState == 1)
              {
                pushUPCounter++;
                startState = 0;
              }
              
            }else if (angle< 10)
            {
              myledg = 1;
              startState = 1;
            }
             //when doing sitUp 5 times, chenge to pushUp
              if (pushUPCounter >= 5)
              {
                  myledr = 1;
                  myledg = 1;
                  myledo = 1;
                  myledb = 1;
                  state = 2;
                  startState = 0;
                  wait_ms(1000);
              }  
       break;

       case 2:
              //Jumping Jacks
              if (angle>80)
              {
                  myledr = 0;
                  myledg = 0;
                  myledo = 0;
                  myledb = 0;
                  if (startState == 1)
                  {
                    jumpingCounter++;
                    startState = 0;
                  }
              }else if (myledr == 0 && angle< 65)
              {
                myledr = 1;
                startState = 1;
              }
              //when doing sitUp 5 times, chenge to pushUp
              if (jumpingCounter >= 5)
              {
                  myledr = 1;
                  myledg = 1;
                  myledo = 1;
                  myledb = 1;
                  state = 3;
                  startState = 0;
                  wait_ms(1000);
              } 
       break;

       case 3:
              //squart
              if (angle>80)
              {
                  myledr = 0;
                  myledg = 0;
                  myledo = 0;
                  myledb = 0;
                  if (startState == 1)
                  {
                    squatCounter++;
                    startState = 0;
                  }
              }else if (angle< 20)
              {
                myledb = 1;
                startState = 1;
              }
              //when doing sitUp 5 times, chenge to pushUp
              if (squatCounter >= 5)
              {
                  myledr = 1;
                  myledg = 1;
                  myledo = 1;
                  myledb = 1;
                  state = 0;
                  startState = 0;
                  wait_ms(1000);
              }       
       break;

      
      default:
        break;
      }

     

      wait_ms(500);
      
    }
  
}
}






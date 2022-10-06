#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t reading[SensorCount];

boolean onoff = false;

const uint8_t maxspeeda = 255;
const uint8_t maxspeedb = 255;
const uint8_t basespeeda = 200;
const uint8_t basespeedb = 200;
const uint8_t lowspeeda = 150;
const uint8_t lowspeedb = 150;


const int Af = 8;
const int Ab = 7;
const int PWM_A = 9;

const int Bf = 4;
const int Bb = 5;
const int PWM_B = 3;

int buttonstart = 11;
bool stopp = false;
char path[50];
int pointer = 0;
char final_path[50];
/*===========================================SETUP===============================================*/

void setup() {
  Serial.begin(9600);
  qtr.setTypeRC();
 
  qtr.setSensorPins((const uint8_t[]){6, 10, A0, A1, A2, A3, A4, A5}, SensorCount);
  pinMode(Af, OUTPUT);
  pinMode(Ab, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(Bf, OUTPUT);
  pinMode(Bb, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  
  delay(500);
  
  pinMode(LED_BUILTIN, OUTPUT);

//  boolean Ok = false;
//  while (Ok == false) { 
//      calibration(); 
//      Ok = true;
//  }
    calibration();
}

void calibrate_move_forward(uint8_t speed_a, uint8_t speed_b){
  digitalWrite(Af, LOW);
  digitalWrite(Ab, HIGH);
  analogWrite(PWM_A, speed_a);

  digitalWrite(Bf, HIGH);
  digitalWrite(Bb, LOW);
  analogWrite(PWM_B, speed_b);
}

void calibration() {
  digitalWrite(LED_BUILTIN, HIGH);
  calibrate_move_forward(100, 100);
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW);
  move_forward(0,0);
  delay(3000);
}

/*===========================================MOVEMENT FUNCTIONS===============================================*/

void move_forward(int posa, int posb) {
  digitalWrite(Af, HIGH);
  digitalWrite(Ab, LOW);
  analogWrite(PWM_A, posa);

  digitalWrite(Bf, HIGH);
  digitalWrite(Bb, LOW);
  analogWrite(PWM_B, posb);
}

void inch(){
  move_forward(maxspeeda, maxspeedb);
  delay(250);
  move_forward(0,0);
}

void u_turn(uint8_t speed_ab){
  digitalWrite(Af, LOW);
  digitalWrite(Ab, HIGH);
  analogWrite(PWM_A, speed_ab);

  digitalWrite(Bf, HIGH);
  digitalWrite(Bb, LOW);
  analogWrite(PWM_B, speed_ab);
  
  delay(600);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}
void turn_right(uint8_t speed_ab){
  digitalWrite(Af, LOW);
  digitalWrite(Ab, HIGH);
  analogWrite(PWM_A, speed_ab);

  digitalWrite(Bf, HIGH);
  digitalWrite(Bb, LOW);
  analogWrite(PWM_B, speed_ab);

  delay(300);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}
void turn_left(uint8_t speed_ab){
  digitalWrite(Af, HIGH);
  digitalWrite(Ab, LOW);
  analogWrite(PWM_A, speed_ab);

  digitalWrite(Bf, LOW);
  digitalWrite(Bb, HIGH);
  analogWrite(PWM_B, speed_ab);

  delay(300);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

/*====================================CHECK FUNCTION=====================================*/
bool check_end(){
  uint16_t position = qtr.readLineWhite(reading);
  if((reading[4]<200 or reading[3]<200) and (reading[0]<200 or reading[7]<200)){
      move_forward(0, 0);
      stopp = true;
      return true;
  }
  return false;
}
bool check_straight(){
  uint16_t position = qtr.readLineWhite(reading);
  if(reading[5]<200 or reading[4]<200 or reading[3]<200 or reading[2]<200){
      return true;
  }
  return false;
}
/*=====================================REDUCE PATH FUNCTION==============================*/


/*======================================DRY RUN==========================================*/

void dry_run(){
  uint16_t position = qtr.readLineWhite(reading);

   // right turn detected
   if( (reading[4]<200 or reading[3]<200)and reading[0]<200){
      move_forward(0, 0);
      delay(50);
      // checking for left 
      if(reading[7]<200){
          inch();
          delay(50);
          if(check_end()){
              path[pointer] = 'X';
              pointer++;
              return;
          }else{
              path[pointer] = 'R';
              pointer++;
              turn_right(200);
              delay(50);
          }
      }
      //checking for straight
      else{
        inch();
        delay(50);
        if(check_straight()){
            if(check_end()){
              path[pointer] = 'X';
              pointer++;
              return;
            }else{
                path[pointer] = 'R';
                pointer++;
                turn_right(200);
                delay(50);
            }
        }else{
            turn_right(200);
            delay(50);
        }
      }
      move_forward(100, 100);
   }
   // left turn detected
   else if( (reading[4]<200 or reading[3]<200)and reading[7]<200){
        move_forward(0, 0);
        delay(50);
        inch();
        delay(50);
        //checking for straight
        if(check_straight()){
            if(check_end()){
                path[pointer] = 'X';
                pointer++;
                return;
            } else{
                path[pointer] = 'S';
                pointer++;
                move_forward(100, 100);
                delay(50);
            }
        }else{
            turn_left(200);
            delay(50);
            move_forward(100, 100);
        }
        
    }
//   if((reading[4]<200 or reading[3]<200)and reading[0]<200){
//      move_forward(0, 0);
//      delay(50);
//      inch();
//      delay(50);
//      turn_rightx(200);
//      delay(50);
//      move_forward(100, 100);
//   }
//   else if((reading[4]<200 or reading[3]<200)and reading[7]<200){
//      move_forward(0, 0);
//      delay(50);
//      inch();
//      delay(50);
//      turn_left(200);
//      delay(50);
//      move_forward(100, 100);
//   }
   //code for keeping bot on track
   else if(reading[7]<200){
      move_forward(basespeeda, lowspeedb);
   }else if(reading[1]<200){
      move_forward(80, basespeedb);
   }else if(reading[6]<200){
      move_forward(basespeeda, 80);
   }else if(reading[0]<200){
    move_forward(lowspeeda, basespeedb);
  }
  // black surface/ dead end
  else if(reading[0]>500 and reading[1]>500 and reading[2]>500 and reading[3]>500 and reading[4]>500 and reading[5]>500 and reading[6]>500 and reading[7]>500){
      // STORE TURN 'B'
//      path[pointer] = 'B';
//      pointer++;
//      u_turn(50);
      move_forward(0, 0);
      delay(50);
      inch();
      delay(50);
      u_turn(200);
      delay(50);
      move_forward(100, 100);
  }
  // when bot is on track without turns
  else{
      move_forward(basespeeda, basespeedb); 
   }
}

void final_run(){
  //final run
}
/*======================================LOOP=============================================*/

void loop() {
  if(onoff == true){
    final_run();
  }else if(stopp == false){
    dry_run();
  }
}

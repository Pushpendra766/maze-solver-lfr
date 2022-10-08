#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t reading[SensorCount];

boolean onoff = false;

const uint8_t e = 2;

const uint8_t maxspeeda = 150;
const uint8_t maxspeedb = 150*e;
const uint8_t basespeeda = 100;
const uint8_t basespeedb = 100*e;
const uint8_t lowspeeda = 70;
const uint8_t lowspeedb = 70*e;
const uint8_t tspeeda = 20;
const uint8_t tspeedb = 20*e;

const int Af = 8;
const int Ab = 7;
const int PWM_A = 9;

const int Bf = 4;
const int Bb = 5;
const int PWM_B = 3;

int buttonstart = 11;
bool stopp = false;
String path;
int pointer = 0;
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

  boolean Ok = false;
  while (Ok == false) { 
      calibration(); 
      Ok = true;
  }
  move_forward(0, 0);
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
  calibrate_move_forward(30, 30*e);
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
  move_forward(basespeeda, basespeedb);
  delay(70);
  move_forward(0,0);
}

void u_turn(){
  digitalWrite(Af, LOW);
  digitalWrite(Ab, HIGH);
  analogWrite(PWM_A, tspeeda);

  digitalWrite(Bf, HIGH);
  digitalWrite(Bb, LOW);
  analogWrite(PWM_B, tspeedb);
  
  delay(690);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}
void right_turn(){
  digitalWrite(Af, LOW);
  digitalWrite(Ab, HIGH);
  analogWrite(PWM_A, tspeeda);

  digitalWrite(Bf, HIGH);
  digitalWrite(Bb, LOW);
  analogWrite(PWM_B, tspeedb);

  delay(100);
  while(reading[2]>200 or reading[3]>200 or reading[4]>200 or reading[5]>200){
    uint16_t position = qtr.readLineWhite(reading);
    digitalWrite(Af, LOW);
    digitalWrite(Ab, HIGH);
    analogWrite(PWM_A, tspeeda);
  
    digitalWrite(Bf, HIGH);
    digitalWrite(Bb, LOW);
    analogWrite(PWM_B, tspeedb);
  }
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}
void left_turn(){
  digitalWrite(Af, HIGH);
  digitalWrite(Ab, LOW);
  analogWrite(PWM_A, tspeeda);

  digitalWrite(Bf, LOW);
  digitalWrite(Bb, HIGH);
  analogWrite(PWM_B, tspeedb);

  delay(250);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

/*=====================================REDUCE PATH FUNCTION==============================*/

void reduce_path(){
    for(int i=0;i<path.length();){
        if(path[i] == 'B'){
            if(path[i-1] == 'L' and path[i+1] == 'L'){
                path[i-1] = 'S';
            }else if(path[i-1] == 'R' and path[i+1] == 'R'){
                path[i-1] = 'S';
            }else if(path[i-1] == 'S' and path[i+1] == 'S'){
                path[i-1] = 'B';
            }else if(path[i-1] == 'L' and path[i+1] == 'R'){
                path[i-1] = 'B';
            }else if(path[i-1] == 'R' and path[i+1] == 'L'){
                path[i-1] = 'B';
            }else if(path[i-1] == 'L' and path[i+1] == 'S'){
                path[i-1] = 'R';
            }else if(path[i-1] == 'S' and path[i+1] == 'L'){
                path[i-1] = 'R';
            }else if(path[i-1] == 'R' and path[i+1] == 'S'){
                path[i-1] = 'L';
            }else if(path[i-1] == 'S' and path[i+1] == 'R'){
                path[i-1] = 'L';
            }
            path.remove(i, 2);
            i--;
        }else{
            i++;
        }
    }
}

/*======================================DRY RUN==========================================*/

void dry_run(){
  uint16_t position = qtr.readLineWhite(reading);

   // right turn detected
//   if( (reading[4]<200 or reading[3]<200)and reading[0]<200){
//      // checking for presence of junction(checking left turn)
//      if(reading[7]<200){
//          // STORE TURN 'R'
//          path[pointer] = 'R';
//          pointer++;
//          inch();
//          right_turn();
//      }else{
//          inch();
//          // checking for end of maze
//          if((reading[4]<200 or reading[3]<200) and reading[0]<200){
//            move_forward(0, 0);
//            stopp = true;
//          }
//          // checking for presence of junction(checking straight line)
//          else if(reading[5]<200 or reading[4]<200 or reading[3]<200 or reading[2]<200){
//              // STORE TURN 'R'
//              path[pointer] = 'R';
//              pointer++;
//          }
//          right_turn();
//      }
//   }
   // left turn detected
//   else if( (reading[4]<200 or reading[3]<200)and reading[7]<200){
//        inch();
//        //checking for end of maze
//        if((reading[4]<200 or reading[3]<200) and reading[7]<200){
//            move_forward(0, 0);
//            stopp = true;
//        }
//        // checking for presence of junction(checking straight line)
//        else if(reading[5]<200 or reading[4]<200 or reading[3]<200 or reading[2]<200){
//          // STORE TURN 'S'
//          path[pointer] = 'S';
//          pointer++;
//          move_forward(basespeeda, basespeedb); 
//        }else{
//          left_turn();
//        }
//    }
   if( (reading[4]<200 or reading[3]<200)and reading[0]<200){
      move_forward(0, 0);
      inch();
      right_turn();
      move_forward(basespeeda, basespeedb);
   }
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
      move_forward(0, 0);
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

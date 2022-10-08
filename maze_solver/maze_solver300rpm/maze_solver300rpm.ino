#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t reading[SensorCount];

boolean onoff = HIGH;

const uint8_t maxspeeda = 255;
const uint8_t maxspeedb = 255;
const uint8_t basespeeda = 200;
const uint8_t basespeedb = 200;
const uint8_t lowspeeda = 150;
const uint8_t lowspeedb = 150;

const int th = 500;

const int Af = 8;
const int Ab = 7;
const int PWM_A = 9;

const int Bf = 4;
const int Bb = 5;
const int PWM_B = 3;

const int LED = 12;

int buttonPin = 11;
bool stopp = false;
String path;
int pointer = 0;
char turn;
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

  pinMode(buttonPin, INPUT);
  
  pinMode(LED, OUTPUT);
  delay(500);
  
  pinMode(LED_BUILTIN, OUTPUT);
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

void slight_turn(){
  uint16_t position = qtr.readLineWhite(reading);
  if(reading[3]<th or reading[4]<th){
    return;
  }else{
    turn_right(100);
    position = qtr.readLineWhite(reading);
    if(reading[3]<th or reading[4]<th){
      return;
    }else{
      turn_left(200);
    }
  }
}

/*====================================CHECK FUNCTION=====================================*/
bool check_end(){
  uint16_t position = qtr.readLineWhite(reading);
  if((reading[4]<th or reading[3]<th) and (reading[0]<th or reading[7]<th)){
      move_forward(0, 0);
      stopp = true;
      return true;
  }
  return false;
}
bool check_straight(){
  uint16_t position = qtr.readLineWhite(reading);
  if(reading[5]<th or reading[4]<th or reading[3]<th or reading[2]<th){
      return true;
  }
  return false;
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
   if( (reading[4]<th or reading[3]<th)and reading[0]<th){
      move_forward(0, 0);
      delay(50);
      // checking for left 
      if(reading[7]<th){
          inch();
          delay(50);
          if(check_end()){
              path += 'X';
              digitalWrite(12, HIGH);
              delay(3000);
              digitalWrite(12, LOW);
              return;
          }else{
              path += 'R';
              turn_right(200);
              delay(50);
              slight_turn();
          }
      }
      //checking for straight
      else{
        inch();
        delay(50);
        if(check_straight()){
            if(check_end()){
              path += 'X';
              digitalWrite(12, HIGH);
              delay(3000);
              digitalWrite(12, LOW);
              return;
            }else{
                path += 'R';
                turn_right(200);
                delay(50);
                slight_turn();
            }
        }else{
            turn_right(200);
            delay(50);
            slight_turn();
        }
      }
      move_forward(100, 100);
   }
   // left turn detected
   else if( (reading[4]<th or reading[3]<th)and reading[7]<th){
        move_forward(0, 0);
        delay(50);
        inch();
        delay(50);
        //checking for straight
        if(check_straight()){
            if(check_end()){
                path += 'X';
                digitalWrite(12, HIGH);
                delay(3000);
                digitalWrite(12, LOW);
                return;
            } else{
                path += 'S';
                move_forward(100, 100);
                delay(50);
            }
        }else{
            turn_left(200);
            delay(50);
            slight_turn();
            move_forward(100, 100);
        }      
    }
   //code for keeping bot on track
   else if(reading[7]<th){
      move_forward(basespeeda, 60);
   }else if(reading[1]<th){
      move_forward(80, basespeedb);
   }else if(reading[6]<th){
      move_forward(basespeeda, 80);
   }else if(reading[0]<th){
    move_forward(60, basespeedb);
  }
  // black surface/ dead end
  else if(reading[0]>500 and reading[1]>500 and reading[2]>500 and reading[3]>500 and reading[4]>500 and reading[5]>500 and reading[6]>500 and reading[7]>500){
      // STORE TURN 'B'
      path += 'B';
      move_forward(0, 0);
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

/*===================FINAL RUN FUNCTIONS==================================*/

void take_turn(char turn){
  if(turn == 'R'){
    turn_right(200);
  }else if(turn == 'L'){
    turn_left(200);
  }else if(turn == 'S'){
    move_forward(basespeeda, basespeedb);
  }
  pointer++;
  delay(50);
}

void done(){
  digitalWrite(12, HIGH);
  delay(60000);
}

/*=================================FINAL RUN==================================*/

void final_run(){
  //final run
  uint16_t position = qtr.readLineWhite(reading);
  turn = path[pointer];
  
   if( (reading[4]<th or reading[3]<th) and reading[0]<th and reading[7]<th){
      move_forward(0, 0);
      delay(50);
      inch();
      delay(50);
      if(check_end()){
        done();
      }else{
        take_turn(turn);
        slight_turn();
      }
   }
   // right turn detected
   else if((reading[4]<th or reading[3]<th) and reading[0]<th){
      move_forward(0, 0);
      delay(50);
      inch();
      delay(50);
      
      if(check_straight()){
        if(check_end()){
          done();
        }else{
          take_turn(turn);
          slight_turn();
        }
      }else{
        turn_right(200);
        slight_turn();
      }
   }
   // left turn detected
   else if( (reading[4]<th or reading[3]<th)and reading[7]<th){
        move_forward(0, 0);
        delay(50);
        inch();
        delay(50);
        if(check_straight()){
          if(check_end()){
            done();
          }else{
            take_turn(turn);
            slight_turn();
          }
        }else{
          turn_left(200);
          slight_turn();
        }    
    }
   //code for keeping bot on track
   else if(reading[7]<th){
      move_forward(basespeeda, lowspeedb);
   }else if(reading[1]<th){
      move_forward(80, basespeedb);
   }else if(reading[6]<th){
      move_forward(basespeeda, 80);
   }else if(reading[0]<th ){
    move_forward(lowspeeda, basespeedb);
  }
  // when bot is on track without turns
  else{
      move_forward(basespeeda, basespeedb); 
   }
}

/*======================================LOOP=============================================*/

void loop() {
  onoff = digitalRead(buttonPin);
  if(onoff == LOW){
    reduce_path();
    while(true){
      final_run();
    }
  }else if(stopp == false){
    dry_run();
  }
}

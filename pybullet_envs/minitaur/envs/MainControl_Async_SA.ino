// This program is used to control the motor asynchronously with different angle, direction and speed.

// RS485 mode selection pin: RE&DE high for sending info; low for receiving info.
int RE = 4;  // Receiver Enable pin
int DE = 5;  // Driver Enable pin

int angelInt = 0;
int speedInt = 0;
int motorID = 1;
byte mode = 'g';
byte direction = 0x00;
byte angelHigh = 0x00;  // 角度的高位字节(目前角度在±360内，只需要2个字节表示)
byte angelLow = 0x00;   // 角度的低位字节
byte speedHigh = 0x00;  // 速度的高位字节(为简化，目前只用2个字节表示速度，最大值为655dps。如果之后发现这个速度还不够用，需要拓展字节，最高可用4个字节)
byte speedLow = 0x00;  // 速度的低位字节
byte checkSum = 0x00;  // Check sum of the command

// byte positionCommand[18] = {0x3e, 0xa4, 0x01, 0x0c, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x23, 0x00, 0x00, 0x4b}; // Position close loop command 1
byte positionCommand[14] = {0x3e, 0xa6, 0x01, 0x08, 0xed, 0x00, 0x28, 0x23, 0x00, 0x28, 0x23, 0x00, 0x00, 0x96}; // Position close loop command 4
byte getPositionCommand[5] = {0x3e, 0x90, 0x01, 0x00, 0xcf};  // Read encoder command
byte returnInfo[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0xcf, 0x0f, 0x0};  // Feedback of the motor

unsigned beginTime = 0;
unsigned endTime = 0;

void setup() {
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  Serial.begin(115200);   // Serial: Communicate with the computer
  Serial1.begin(115200);  // Serial1: Communicate with the RS485

  while (Serial.read() >= 0) {} //clear serial port
}

void loop() {
  // ----------------Select Mode----------------
  while (Serial.available() == 0) {}
  mode = direction = Serial.read();
  if(mode == 'g') {
    getPosition();
  }
  else if(mode == 'd') {
    driveMotor();
  }
}

void driveMotor() {
  // ----------------Read Parameters----------------
  // Read the motor ID
  while (Serial.available() == 0) {}
  motorID = Serial.parseInt();

  // Read the direction of angel
  while (Serial.available() == 0) {}
  direction = Serial.read();

  // Read the desired angel
  while (Serial.available() == 0) {}
  angelInt = Serial.parseInt();

  // Read the desired speed
  while (Serial.available() == 0) {}
  speedInt = Serial.parseInt();

  // Read the extra "|"
  Serial.read();

  // ----------------Generate Command----------------
  generateCommand();

  // ----------------Send Command----------------
  digitalWrite(RE, HIGH);
  digitalWrite(DE, HIGH);
  Serial1.write(positionCommand, 14);
  //Serial.write(positionCommand, 14);

  // Make sure the serial buffer is empty
  Serial1.flush();
}


void generateCommand() {
  // Direction
  if(direction == '+') {
    direction = 0x00;
  }
  else {
    direction = 0x01;
  }

  // Angel from int to byte
  angelLow = byte(angelInt);
  angelHigh = byte(angelInt / (16 * 16));
  /*
  The angel will not be nagative, so this part could be ignored.
  if (angelInt < 0) {
    angelHigh = angelHigh - 1;
  }
  */

  // Speed from int to byte
  speedLow = byte(speedInt);
  speedHigh = byte(speedInt / (16 * 16));

  // Calculate check sum
  checkSum = direction + angelHigh + angelLow + speedHigh + speedLow;
  if (angelInt < 0) {
    unsigned char i = 0;
    for (i = 0; i < 8; i++) {
      checkSum = checkSum + 0xFF;
    }
  }

  // Compose the command
  positionCommand[2] = byte(motorID);
  positionCommand[4] = positionCommand[0] + positionCommand[1] + positionCommand[2] + positionCommand[3];
  positionCommand[5] = direction;
  positionCommand[6] = angelLow;
  positionCommand[7] = angelHigh;
  positionCommand[9] = speedLow;
  positionCommand[10] = speedHigh;
  positionCommand[13] = checkSum;

  /*
  The angel will not be nagative, so this part could be ignored.
  unsigned char i = 0;
  if (angelInt >= 0) {
    for (i = 7; i < 13; i++) {
      positionCommand[i] = 0x00;
    }
  }
  else {
    for (i = 7; i < 13; i++) {
      positionCommand[i] = 0xFF;
    }
  }
  */
}


// 电机的返回值可能会在Serial1的buffer中，如果造成了影响需要进行清理
void getPosition() {
  // Get motor ID
  while (Serial.available() == 0) {}
  motorID = Serial.parseInt();

  // Generate request command
  getPositionCommand[2] = byte(motorID);
  getPositionCommand[4] = getPositionCommand[0] + getPositionCommand[1] + getPositionCommand[2] + getPositionCommand[3];

  // RS485 send mode
  digitalWrite(RE, HIGH);
  digitalWrite(DE, HIGH);

  // Send command
  Serial1.write(getPositionCommand, 5);

  // Make sure the serial buffer is empty
  Serial1.flush();

  // RS485 read mode
  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);

  // Read feedback from motor
  while(Serial1.available() == 0) {
  }
  Serial1.readBytes(returnInfo, 8);

  Serial.write(returnInfo, 8);

  // printCommand(returnInfo, 8);
}


void printCommand(byte* content, int len) {
  unsigned char i = 0;
  for (i = 0; i < len; i++) {
    Serial.print(content[i], HEX);
    Serial.print(" ");
  }
  Serial1.println();
}

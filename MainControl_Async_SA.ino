// This program is used to control the motor asynchronously with different angle, direction and speed.

// RS485 mode selection pin: RE&DE high for sending info; low for receiving info.
int RE = 4;  // Receiver Enable pin
int DE = 5;  // Driver Enable pin

int angelInt = 0;
int speedInt = 0;
byte angelHigh = 0x00;  // 角度的高位字节(目前角度在±360内，只需要2个字节表示)
byte angelLow = 0x00;   // 角度的低位字节
byte speedHigh = 0x00;  // 速度的高位字节(为简化，目前只用2个字节表示速度，最大值为655dps。如果之后发现这个速度还不够用，需要拓展字节，最高可用4个字节)
byte speedLow = 0x00;  // 速度的低位字节
byte checkSum = 0x00;  // Check sum of the command

byte positionCommand[18] = {0x3e, 0xa4, 0x01, 0x0c, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x23, 0x00, 0x00, 0x4b}; // Position close loop command 1
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

  // ----------------Get Posisition----------------
  getPosition();

  // ----------------Send Posisition----------------


  // ----------------Read Parameters----------------
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
  Serial1.write(positionCommand, 18);

  // Make sure the serial buffer is empty
  Serial1.flush();
}


void generateCommand() {
  // Angel from int to byte
  angelLow = byte(angelInt);
  angelHigh = byte(angelInt / (16 * 16));
  if (angelInt < 0) {
    angelHigh = angelHigh - 1;
  }

  // Speed from int to byte
  speedLow = byte(speedInt);
  speedHigh = byte(speedInt / (16 * 16));

  // Calculate check sum
  checkSum = angelHigh + angelLow + speedHigh + speedLow;
  if (angelInt < 0) {
    unsigned char i = 0;
    for (i = 0; i < 8; i++) {
      checkSum = checkSum + 0xFF;
    }
  }

  // Compose the command
  positionCommand[5] = angelLow;
  positionCommand[6] = angelHigh;
  positionCommand[13] = speedLow;
  positionCommand[14] = speedHigh;
  positionCommand[17] = checkSum;
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
  positionCommand[15] = 0x00;
  positionCommand[16] = 0x00;
}


void getPosition() {
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

  // Serial.write(returnInfo, 8);
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

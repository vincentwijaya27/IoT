#include <SPI.h>//include the SPI bus library
#include <MFRC522.h>//include the RFID reader library

#define SS_PIN 21  //slave select pin
#define RST_PIN 22  //reset pin
MFRC522 mfrc522(SS_PIN, RST_PIN);        // instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information
byte blockAddr = 20;
String readData = "";
MFRC522::StatusCode status;
int led = 12;
//int leds = 14;

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");
  pinMode(12, OUTPUT);
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
//  _byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);

}


int blockTitle = 20; //this is the block number we will write into and then read. Do not write into 'sector trailer' block, since this can make the block unusable.
int blockData = 21;
byte blockcontentTitle[16] = {"Saldo Anda: "};//an array with 16 bytes to be written into one of the 64 card blocks is defined!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
byte blockcontentData[16];
byte clearBlockContent[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //all zeros. This can be used to delete a block.
byte readbackblock[18];//This array is used for reading out a block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.


int tempSaldo = 0;
int saldo = 0;
bool flag = true;
int count = 0;
char datas[6];
char tempChar[16];
byte len[18];
void loop() {
  // Look for new cards (in case you wonder what PICC means: proximity integrated circuit card)
  if ( ! mfrc522.PICC_IsNewCardPresent()) {//if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
    return;//if it did not find a new card is returns a '0' and we return to the start of the loop
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {//if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.
    return;//if it returns a '0' something went wrong and we return to the start of the loop
  }
  Serial.println("card selected");

  while (Serial.available()) {
    byte temp = Serial.read();
    if (temp != 0x0a) {
      //blockcontentData[count] = temp;
      datas[count] = temp;
      count++;
    }
    else {

    }

  }
  //convert char to int ( to be calculated );
  tempSaldo = atoi(&datas[0]);
  saldo += tempSaldo;
  itoa(saldo, tempChar, 10);
  //      blockcontentData[ = tempChar;
  for (int i = 0; i <= 16; i++) {
    blockcontentData[i] = tempChar[i];
  }







  Serial.println("Written Data: ");
  Serial.println(tempChar);
  Serial.println("Length Data: ");
  Serial.println(count);
  Serial.println();

  //clear block
  // ResetBlock(20,clearBlockContent);


  writeBlock( blockData, blockcontentData);//the blockcontent array is written into the card block
  readBlock(blockData, readbackblock);
  Serial.print("Saldo Anda: ");
  Serial.println(tempChar);

  count = 0;


  //Halt PICC
  mfrc522.PICC_HaltA();
  //Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}




int writeBlock( int blocknoData, byte arrayAddressData[]) {
  //write title first one time


  /*****************************************authentication of the desired block for access***********************************************************/
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blocknoData, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3;
  }

  /*****************************************writing the block***********************************************************/

  status = mfrc522.MIFARE_Write(blocknoData, arrayAddressData, 16);//valueBlockA is the block number, MIFARE_Write(block number (0-15), byte array containing 16 values, number of bytes in block (=16))
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println("Saldo Has been Topped up");
  Serial.println("Current data Blocks");

  Serial.println(F("Current data in Sector: "));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key , 5);
  Serial.println();



}
int readBlock(int blockNumber, byte arrayAddress[]) {
  //membaca data saldo pada rfid
  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNumber, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println("Saldo Anda Sekarang: ");
  for (int j = 0; j < 16; j++) {
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNumber, buffer, &size);
    String sTemp = String ((char*)buffer);
    readData += sTemp.substring(0, 16);
  }

  Serial.println("Block Saldo Was Read");

}
int ResetBlock(int blockNum, byte arrayAddress[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;//
  }

  /*****************************************writing the block***********************************************************/
  status = mfrc522.MIFARE_Write(blockNum, arrayAddress, 16);
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;//return "4" as error message
  }
  Serial.println("block was cleared");

}

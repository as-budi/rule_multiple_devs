#include <EEPROM.h>
#include <unordered_map>

#define EEPROM_SIZE 512
#define MAX_RULE 3
#define MAX_TRIGGER 4
#define serialbufferSize 50

char inputBuffer[serialbufferSize];
int serialIndex = 0; 

int t_id, t_val, s_val;
std::unordered_map<int, int> data_log;
int dev_rule[MAX_RULE][MAX_TRIGGER];
int val_rule[MAX_RULE][MAX_TRIGGER];
int s_rule[MAX_RULE]; 

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, please ask me something");
  Serial.println("type \"Hello\", \"Goodby\", \"web\" or \"dosomething\" or make up your own command");
  Serial.println();

  EEPROM.begin(EEPROM_SIZE);
  for (int a = 0; a < EEPROM_SIZE; a++){
    EEPROM.write(a, 255);
  }

  EEPROM.write(0, 254);
  EEPROM.write(1, 2);
  EEPROM.write(2, 1);
  EEPROM.write(3, 1);
  EEPROM.write(4, 2);
  EEPROM.write(5, 1);
  EEPROM.write(6, 1);
  EEPROM.write(7, 254);
  EEPROM.write(8, 2);
  EEPROM.write(9, 1);
  EEPROM.write(10, 0);
  EEPROM.write(11, 2);
  EEPROM.write(12, 0);
  EEPROM.write(13, 0);
  EEPROM.write(14, 254);
  EEPROM.write(15, 1);
  EEPROM.write(16, 1);
  EEPROM.write(17, 0);
  EEPROM.write(18, 1);
  EEPROM.commit();

  delay(1000);

  eepromToDataStructure();
}

void loop() {
  // Notice how the main loop is very simple and the functions 
  // seperate the logic into easily manageable parts
  if (CheckSerial()) DoCommand(inputBuffer); 
  // Do other stuff

}

boolean DoCommand(char * commandBuffer)
{
  // Standard way to handle commands
  if (strstr(commandBuffer, "read")){
    for(int i = 0; i < 100; i++){
      Serial.println(EEPROM.read(i));
    }
  }
  else if (strstr(commandBuffer, "dataStructure")){
    printDataStructure();
  }
  else if (strstr(commandBuffer, "logshow")){
    showDataLog();
  }
  else if (strstr(commandBuffer, "trigger1")){
    t_id = 1;
    t_val = 1;
    Serial.print("d_id: ");
    Serial.print(t_id);
    Serial.print(", t_val: ");
    Serial.println(t_val);
    log(t_id, t_val);
    matchData(t_id, t_val);
  }  
  else if (strstr(commandBuffer, "trigger2")){
    t_id = 2;
    t_val = 1;
    Serial.print("d_id: ");
    Serial.print(t_id);
    Serial.print(", t_val: ");
    Serial.println(t_val);
    log(t_id, t_val);
    matchData(t_id, t_val);
  }
  else if (strstr(commandBuffer, "trigger3")){
    t_id = 1;
    t_val = 0;
    Serial.print("d_id: ");
    Serial.print(t_id);
    Serial.print(", t_val: ");
    Serial.println(t_val);
    log(t_id, t_val);
    matchData(t_id, t_val);
  }
  else if (strstr(commandBuffer, "trigger4")){
    t_id = 2;
    t_val = 0;
    Serial.print("d_id: ");
    Serial.print(t_id);
    Serial.print(", t_val: ");
    Serial.println(t_val);
    log(t_id, t_val);
    matchData(t_id, t_val);
  }
  else if (strstr(commandBuffer, "trigger5")){
    t_id = 2;
    t_val = 0;
    Serial.print("d_id: ");
    Serial.print(t_id);
    Serial.print(", t_val: ");
    Serial.println(t_val);
    log(t_id, t_val);
    matchData(t_id, t_val);
  }
  else {
    Serial.print("I dont understand you \nYou said: ");
    Serial.println(commandBuffer);
    //    Do some other work here
    //    and here
    //    and here
  }
return true;
}

boolean CheckSerial()
{
  boolean lineFound = false;
  // if there's any serial available, read it:
  while (Serial.available() > 0) {
    //Read a character as it comes in:
    //currently this will throw away anything after the buffer is full or the \n is detected
    char charBuffer = Serial.read(); 
      if (charBuffer == '\r') {
           inputBuffer[serialIndex] = 0; // terminate the string
           lineFound = (serialIndex > 0); // only good if we sent more than an empty line
           serialIndex=0; // reset for next line of data
         }
         else if(serialIndex < serialbufferSize && lineFound == false) {
           /*Place the character in the string buffer:*/
           inputBuffer[serialIndex++] = charBuffer; // auto increment index
         }
  }// End of While
  return lineFound;
}// End of CheckSerial()

void log(int id, int val){
  auto it = data_log.find(id);
  if (it != data_log.end()) {
      // Update existing entry
      it->second = val;
      Serial.printf("Updated data for trigger_id %d: %d\n", id, val);
  } else {
      // Add new entry
      data_log[id] = val;
      Serial.printf("Received data for trigger_id %d: %d\n", id, val);
  }
}


void showDataLog(){
  Serial.println("Data Log:");
  for (auto& entry : data_log) {
      Serial.printf("Trigger_id: %d, Trigger_value: %d\n", entry.first, entry.second);
  }
}

void eepromToDataStructure(){
  int i = 0;
  int j = 0;
  int ruleIndex = 0;
  bool scan = true;
  int eepromData;
  int numOfTrigger;
  while(scan){
    eepromData = EEPROM.read(i);
    Serial.print("EEPROM(");
    Serial.print(i);
    Serial.print("): ");
    Serial.println(eepromData);
    if(eepromData == 254){
      numOfTrigger = EEPROM.read(i + 1);
      for(j = 0; j < numOfTrigger; j++){
        dev_rule[ruleIndex][j] = EEPROM.read((i + 2) + (j * 2));
        val_rule[ruleIndex][j] = EEPROM.read((i + 2) + (j * 2) + 1);
      }
      s_rule[ruleIndex] = EEPROM.read(i + 2 + numOfTrigger * 2);
      i = i + 3 + numOfTrigger * 2;
      ruleIndex++;
    }
    else {
      scan = false;
    }
  }
}

void printDataStructure(){
  Serial.println("dev_rule: ");
  for ( int i = 0; i < MAX_RULE; ++i ) {
    // loop through columns of current row
    for ( int j = 0; j < MAX_TRIGGER; ++j ){
      Serial.print (dev_rule[ i ][ j ] );
      Serial.print ("\r" ) ; // start new line of output
    }
  }
  Serial.print ("\r" );
  Serial.println("val_rule: ");
  for ( int i = 0; i < MAX_RULE; ++i ) {
    // loop through columns of current row
    for ( int j = 0; j < MAX_TRIGGER; ++j ){
      Serial.print (val_rule[ i ][ j ] );
      Serial.print ("\r" ) ; // start new line of output
    } 
  }
  Serial.print ("\r" );
  Serial.println("s_rule: ");
  for ( int i = 0; i < MAX_RULE; ++i ) {
      Serial.print (s_rule[ i ]);
  } 
  Serial.println(" ");
}

void matchData(int id, int val){
  for(int x = 0; x < MAX_RULE; x++){
    for(int y = 0; y < MAX_TRIGGER; y++){
      if(id == dev_rule[x][y]){
        if(val == val_rule[x][y]){
          checkAllTrigger(x);
        }
      }
    }
  }
}

void checkAllTrigger(int ruleID){
  bool complete = false;
  bool match = true;
  int triggerID = 0;
  while(!complete){
    if(dev_rule[ruleID][triggerID] != 0){
      if((data_log[dev_rule[ruleID][triggerID]] == val_rule[ruleID][triggerID])){
        match = true;
        triggerID++;
      }
      else{
        match = false;
        complete = true;
        Serial.println("Rule does not match");
      }
    }
    else{
      if(match){
        s_val = s_rule[ruleID];
        Serial.println("Rule matched");
        Serial.print("s_val: ");
        Serial.println(s_val);
      } 
      complete = true;
    }  
  }
}
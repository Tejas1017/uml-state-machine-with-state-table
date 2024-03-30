#include <Arduino.h>

// Define enums and structs
typedef enum {
  idle,
  incPress,
  decPress,
  statePress,
  maxState
} state_t;

typedef enum {
  
  incCount,
  decCount,
  resetCount,
  countVariable,
  ENTRY,
  EXIT,
  maxEvent
} event_t;

typedef enum {
  eventHandled,
  eventTransition,
  eventIgnored
} event_status_t;

struct p1 {
  int count;
  state_t activeState;
  uintptr_t *state_table;//pointing to 2d array
};

// Global variables
event_t ee = ENTRY;//ee is event variable which show which is current event ....i should have used structure embedding concept for this 
p1 c = {.count = 0, .activeState = idle};

// Function prototypes
void eventDispatch();
//event_status_t statMachine();
typedef event_status_t (*eHandler)();
eHandler ehandler; 

//idle state
//////////////////////////////////////////////////////////////////////////
event_status_t idlerEntry(){
  Serial.println("1) to increment count 2) decrement count 3) reset count");
  return eventHandled;  

}
event_status_t idlerExit(){
     return eventHandled;
}
event_status_t idlerIncCount(){
      Serial.println("Incrementing count");
      c.count++;
      c.activeState = incPress;
      return eventTransition;   
}
event_status_t idlerDecCount(){
      Serial.println("Decrementing count is not allowed in idle state");
      return eventIgnored;
}
event_status_t idlerResetCount(){
      Serial.println("Resetting count");
      c.count = 0;
      return eventHandled;  
}
event_status_t idlerCountVariable(){
      Serial.println(c.count);
      c.activeState=statePress;
      return eventTransition;  
}

//increment state ///////
/////////////////////////////////////////
event_status_t incEntry(){
      Serial.println("You are in increment state");
      return eventHandled;
}

event_status_t incExit(){
    return eventHandled;
}
event_status_t incIncCount(){
    Serial.println("Incrementing count");
      c.count++;
      return eventHandled;
}
event_status_t incDecCount(){
      Serial.println("Decrementing inc count");
      if (c.count > 0) {
        c.count--;
        c.activeState=decPress;
        return eventTransition;
      } else {
        // If count is already 0, decrement is not allowed
        Serial.println("Count is already 0, decrement not allowed");
        return eventIgnored;
      }
}
event_status_t incResetCount(){
    // Resetting count and transitioning back to idle state
      Serial.println("Resetting inc count");
      c.count = 0;
      c.activeState = idle;
      return eventTransition;
} 

event_status_t incCountVariable(){
       Serial.println(c.count);
      c.activeState=statePress;
      return eventTransition;
}
///////dec handler
event_status_t decEntry(){
    Serial.println("You are in decrement state");
      return eventHandled;
}
event_status_t decExit(){
  return eventHandled;
    
}
event_status_t decIncCount(){
      Serial.println("Incrementing in dec state");
      c.count++;
      c.activeState=incPress;
      return eventTransition;
}
event_status_t decDecCount(){
     Serial.println("Decrementing count");
      if (c.count > 0) {
        c.count--;
        return eventHandled;
      } else {
       
        Serial.println("Count is already 0, decrement not allowed");
        return eventIgnored;
      }
}
event_status_t decResentCount(){
       // Resetting count and transitioning back to idle state
      Serial.println("Resetting count");
      c.count = 0;
      c.activeState = idle;
      return eventTransition;
}
event_status_t decShowVariable(){
    Serial.println(c.count);
      c.activeState=statePress;
      return eventTransition;  
}

//state handler function

event_status_t stateEntry(){
    Serial.println(" ");
   // Serial.println(c.count);
      return eventHandled;
}
event_status_t stateExit(){
    return eventHandled;  
}
event_status_t stateIncCount(){
      Serial.println("Incrementing count");
      c.count++;
      c.activeState=incPress;
      return eventTransition;
}
event_status_t stateDecCount(){
       Serial.println("Decrementing count");
      if (c.count > 0) {
        c.count--;
        c.activeState=decPress;
        return eventTransition;
      } else {
        
        Serial.println("Count is already 0, decrement not allowed");
        return eventIgnored;  
      }

}
event_status_t stateResetCount(){
        // Resetting count and transitioning back to idle state
      Serial.println("Resetting count");
      c.count = 0;
      c.activeState = idle;
      return eventTransition;
}
event_status_t stateShowCount(){
    Serial.println(c.count);
    return eventHandled;  
}
void stateTableInit(){
    static eHandler stateTable2array[maxState][maxEvent]= {
        [idle]={idlerIncCount,idlerDecCount,idlerResetCount,idlerCountVariable,idlerEntry,idlerExit},
        [incPress]={incIncCount,incDecCount,incResetCount,incCountVariable,incEntry,incExit},
        [decPress]={decIncCount,decDecCount,decResentCount,decShowVariable,decEntry,decExit},
        [statePress]={stateIncCount,stateDecCount,stateResetCount,stateShowCount,stateEntry,stateExit},
    };
    c.state_table=(uintptr_t*)&stateTable2array[0][0];
}





void setup() {
 
  Serial.begin(9600);
  stateTableInit();
  ehandler = (eHandler)c.state_table[idle * maxEvent + ENTRY];
 ehandler();
}

void loop() {
  char s = 0;
  if (Serial.available() > 0) {
    s = Serial.read();
  }

  // Handling serial input to trigger events
  switch (s) {
    case '1':
      ee = incCount;
      break;
    case '2':
      ee = decCount;
      break;
    case '3':
      ee = resetCount;
      break;
    case '4':
      ee =countVariable;
      break;
    default:
      // If no valid input received, do nothing
      return;
  }

  eventDispatch();
  //rial.println(c.count);
}

void eventDispatch() {
  event_status_t status;
  state_t curr = c.activeState;
  state_t target;
  eHandler eha;
  eha = (eHandler)c.state_table[c.activeState *maxEvent +ee];
  status =eha();  
  if (status == eventTransition) {
    target = c.activeState;

    ee = EXIT;
    //c.activeState = curr;
    eha= c.state_table[curr*maxEvent+ee];//first previus active state exit need to be exe before transition ee store exit event of current state
    eha();
    ee = ENTRY;
    //c.activeState = target;
    eha =c.state_table[target*maxEvent+ee];//after exe exit event of perious state we need to exe entry event of target state 2d array make it ez to find target state exit event function handler
    //statMachine();
    eha();
  }
 
}


import controlP5.*;
import processing.serial.*;

Serial port;

ControlP5 cp5;

String textValue = "";
String start_calibration = "Would you like to begin calibration?";
String type_y = "Type Y to continue";
String instruction_1 = "You need a container of compressed gas mixture with __ppm of CO,__ppm of CO2, and a container of compressed air or Nitrogen.";
String instruction_2 = "Connect these gasses to the calibration chamber as described in the user manual.";
String confirm = "When complete, please type 'C' to confirm";
String waiting = "Calibration Beginning - Please Wait";
String step_2 = "You are ready to begin Step 2";
String instruction_3 = "SecondYou need a container of compressed gas mixture with __ppm of CO,__ppm of CO2, and a container of compressed air or Nitrogen.";
String step_3 = "You are ready to begin Step 3";
String final_message = "Calibration Complete";
PFont font;
int i = 0;

void setup() {
  //port = new Serial(this, "/dev/cu.usbserial-1410", 9600);
  font = createFont("arial",20);  
  
  cp5 = new ControlP5(this);
  
  // Add in the first textfield and button.
  cp5.addTextfield(type_y)
     .setPosition(240,150)
     .setSize(200,40)
     .setFont(font)
     .setFocus(true) // No idea what this does
     .setColor(color(255,0,0))
     ;
    
  cp5.addBang("Yes")
     .setPosition(300,300)
     .setSize(80,40)
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)// Center the word Yes in the middle of the button
     ;    
  
  textFont(font);
  
  
}

public void settings() {
  size(700,400); // Set the size of the window
}

void draw() {
  background(0);
  fill(255);
  // Depending on which question of the calibration process we are on, show that question
  if (i == 0) {
    text(start_calibration, width/2, 60); // Print the text on the screen
    textAlign(CENTER); // Center it in the middle of the box that is created by the second and third parameters in the text function
  }
  else if (i == 1) {
    text(instruction_1, 10, 10, 690, 80);
    text(instruction_2, 10, 80, 690, 200);
    textAlign(CENTER);
  }
  else if (i == 2) {
    text(step_2, width/2, 30);
    text(instruction_1, 10, 50, 690, 110);
    text(instruction_2, 10, 110, 690, 230);
    textAlign(CENTER);
  }
  else if (i == 3) {
    text(step_3, width/2, 30);
    text(instruction_1, 10, 50, 690, 110);
    text(instruction_2, 10, 110, 690, 230);
    textAlign(CENTER);
  }
  else if (i == 4) {
    text(final_message, width/2, 30);
    textAlign(CENTER);
  }
}

// This function is what happened when the Yes button (Bang) is pressed.
public void Yes() {
  String text = cp5.get(Textfield.class,type_y).getText(); // Get the text that was entered and store it in this variable
  if (text.equals("y") || text.equals("Y")) {
    Textfield t = cp5.get(Textfield.class,type_y);
    t.remove(); // Delete the first question textfield
    //Label l = t.getCaptionLabel();
    //l.setText(question_2); // This was a way to try to change the text on in the textfield instead of creating a new one for the next question
    cp5.addTextfield(confirm) // Add a new textfield, this could be up in the draw() as well
     .setPosition(90, 150)
     .setSize(200,40)
     .setFont(font)
     .setFocus(true)
     .setColor(color(255,0,0))
     ;
    i++;
    // Ideally we could make the Yes button invisible and put it in the corner, but I couldn't get it to work,
    // so instead I just put the Continue button on top of it.
    //Bang done = cp5.get(Bang.class,"Yes");
    //done.setPosition(0,0);
    //done.setVisible(false);
    cp5.addBang("Continue")
       .setPosition(300,300)
       .setSize(80,40)
       .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
       ;   
  }
  //cp5.get(Textfield.class,question_1).clear(); // Another way to erase what was enterd in the text field
}

public void Continue() {
  String text = cp5.get(Textfield.class,confirm).getText(); 
  //port.write(text); send the entered data over to the Arduino
  if (text.equals("c") || text.equals("C")) {
    i++;
    // This was an idea to make an Again button for when the calibration proccess is over, asking if they want to do 
    // it again. However, I couldn't get this to work becuase of all the buttons and textfields that are already created
    // and are just hidden. So instead, if they want to redo the calibration process they can close the program and 
    // launch it again
    //if (i == 4) {
    //  cp5.addBang("Again")
    //   .setPosition(300,300)
    //   .setSize(80,40)
    //   .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    //   ;   
    //}
  }
  cp5.get(Textfield.class,confirm).clear();
}

// What the Again button was going to do
//public int Again() {
  //i = 0;
  // I had trouble finding textfields that I had already created and displaying them again, since 
  // previously I always created a new one to display it.
  //Textfield t = cp5.get(Textfield.class,type_y);
  //Controller c = cp5.getController(type_y);
  //c.setPosition(100,100);
  //print("Got the textfield");
  ////t.setText("New text");
  //print("Got here"); // how you print to the console for debugging
  //Bang yes = cp5.get(Bang.class,"Yes");
  //yes.setPosition(300,300);
//}

// What happens when the user hits the enter/return key
//void controlEvent(ControlEvent theEvent) {
//  if(theEvent.isAssignableFrom(Textfield.class)) {
//    if (theEvent.getStringValue() == "y") {
//      println("Got this string");
//    }
//    else {
//      println("controlEvent: accessing a string from controller '"
//              +theEvent.getName()+"': "
//              +theEvent.getStringValue()
//              );
//      cp5.get(Textfield.class,"Question 1").clear();
//    }
//    //port.write(theEvent.getStringValue());
//  }
//}



/*
a list of all methods available for the Textfield Controller
use ControlP5.printPublicMethodsFor(Textfield.class);
to print the following list into the console.

You can find further details about class Textfield in the javadoc.

Format:
ClassName : returnType methodName(parameter type)

controlP5.Textfield : String getText() 
controlP5.Textfield : Textfield clear() 
controlP5.Textfield : Textfield keepFocus(boolean) 
controlP5.Textfield : Textfield setAutoClear(boolean) 
controlP5.Textfield : Textfield setFocus(boolean) 
controlP5.Textfield : Textfield setFont(ControlFont) 
controlP5.Textfield : Textfield setFont(PFont) 
controlP5.Textfield : Textfield setFont(int) 
controlP5.Textfield : Textfield setText(String) 
controlP5.Textfield : Textfield setValue(String) 
controlP5.Textfield : Textfield setValue(float) 
controlP5.Textfield : boolean isAutoClear() 
controlP5.Textfield : int getIndex() 
controlP5.Textfield : void draw(PApplet) 
controlP5.Textfield : void keyEvent(KeyEvent) 
controlP5.Textfield : void setInputFilter(int) 
controlP5.Textfield : void setPasswordMode(boolean) 
controlP5.Controller : CColor getColor() 
controlP5.Controller : ControlBehavior getBehavior() 
controlP5.Controller : ControlWindow getControlWindow() 
controlP5.Controller : ControlWindow getWindow() 
controlP5.Controller : ControllerProperty getProperty(String) 
controlP5.Controller : ControllerProperty getProperty(String, String) 
controlP5.Controller : Label getCaptionLabel() 
controlP5.Controller : Label getValueLabel() 
controlP5.Controller : List getControllerPlugList() 
controlP5.Controller : PImage setImage(PImage) 
controlP5.Controller : PImage setImage(PImage, int) 
controlP5.Controller : PVector getAbsolutePosition() 
controlP5.Controller : PVector getPosition() 
controlP5.Controller : String getAddress() 
controlP5.Controller : String getInfo() 
controlP5.Controller : String getName() 
controlP5.Controller : String getStringValue() 
controlP5.Controller : String toString() 
controlP5.Controller : Tab getTab() 
controlP5.Controller : Textfield addCallback(CallbackListener) 
controlP5.Controller : Textfield addListener(ControlListener) 
controlP5.Controller : Textfield bringToFront() 
controlP5.Controller : Textfield bringToFront(ControllerInterface) 
controlP5.Controller : Textfield hide() 
controlP5.Controller : Textfield linebreak() 
controlP5.Controller : Textfield listen(boolean) 
controlP5.Controller : Textfield lock() 
controlP5.Controller : Textfield plugTo(Object) 
controlP5.Controller : Textfield plugTo(Object, String) 
controlP5.Controller : Textfield plugTo(Object[]) 
controlP5.Controller : Textfield plugTo(Object[], String) 
controlP5.Controller : Textfield registerProperty(String) 
controlP5.Controller : Textfield registerProperty(String, String) 
controlP5.Controller : Textfield registerTooltip(String) 
controlP5.Controller : Textfield removeBehavior() 
controlP5.Controller : Textfield removeCallback() 
controlP5.Controller : Textfield removeCallback(CallbackListener) 
controlP5.Controller : Textfield removeListener(ControlListener) 
controlP5.Controller : Textfield removeProperty(String) 
controlP5.Controller : Textfield removeProperty(String, String) 
controlP5.Controller : Textfield setArrayValue(float[]) 
controlP5.Controller : Textfield setArrayValue(int, float) 
controlP5.Controller : Textfield setBehavior(ControlBehavior) 
controlP5.Controller : Textfield setBroadcast(boolean) 
controlP5.Controller : Textfield setCaptionLabel(String) 
controlP5.Controller : Textfield setColor(CColor) 
controlP5.Controller : Textfield setColorActive(int) 
controlP5.Controller : Textfield setColorBackground(int) 
controlP5.Controller : Textfield setColorCaptionLabel(int) 
controlP5.Controller : Textfield setColorForeground(int) 
controlP5.Controller : Textfield setColorValueLabel(int) 
controlP5.Controller : Textfield setDecimalPrecision(int) 
controlP5.Controller : Textfield setDefaultValue(float) 
controlP5.Controller : Textfield setHeight(int) 
controlP5.Controller : Textfield setId(int) 
controlP5.Controller : Textfield setImages(PImage, PImage, PImage) 
controlP5.Controller : Textfield setImages(PImage, PImage, PImage, PImage) 
controlP5.Controller : Textfield setLabelVisible(boolean) 
controlP5.Controller : Textfield setLock(boolean) 
controlP5.Controller : Textfield setMax(float) 
controlP5.Controller : Textfield setMin(float) 
controlP5.Controller : Textfield setMouseOver(boolean) 
controlP5.Controller : Textfield setMoveable(boolean) 
controlP5.Controller : Textfield setPosition(PVector) 
controlP5.Controller : Textfield setPosition(float, float) 
controlP5.Controller : Textfield setSize(PImage) 
controlP5.Controller : Textfield setSize(int, int) 
controlP5.Controller : Textfield setStringValue(String) 
controlP5.Controller : Textfield setUpdate(boolean) 
controlP5.Controller : Textfield setValueLabel(String) 
controlP5.Controller : Textfield setView(ControllerView) 
controlP5.Controller : Textfield setVisible(boolean) 
controlP5.Controller : Textfield setWidth(int) 
controlP5.Controller : Textfield show() 
controlP5.Controller : Textfield unlock() 
controlP5.Controller : Textfield unplugFrom(Object) 
controlP5.Controller : Textfield unplugFrom(Object[]) 
controlP5.Controller : Textfield unregisterTooltip() 
controlP5.Controller : Textfield update() 
controlP5.Controller : Textfield updateSize() 
controlP5.Controller : boolean isActive() 
controlP5.Controller : boolean isBroadcast() 
controlP5.Controller : boolean isInside() 
controlP5.Controller : boolean isLabelVisible() 
controlP5.Controller : boolean isListening() 
controlP5.Controller : boolean isLock() 
controlP5.Controller : boolean isMouseOver() 
controlP5.Controller : boolean isMousePressed() 
controlP5.Controller : boolean isMoveable() 
controlP5.Controller : boolean isUpdate() 
controlP5.Controller : boolean isVisible() 
controlP5.Controller : float getArrayValue(int) 
controlP5.Controller : float getDefaultValue() 
controlP5.Controller : float getMax() 
controlP5.Controller : float getMin() 
controlP5.Controller : float getValue() 
controlP5.Controller : float[] getArrayValue() 
controlP5.Controller : int getDecimalPrecision() 
controlP5.Controller : int getHeight() 
controlP5.Controller : int getId() 
controlP5.Controller : int getWidth() 
controlP5.Controller : int listenerSize() 
controlP5.Controller : void remove() 
controlP5.Controller : void setView(ControllerView, int) 
java.lang.Object : String toString() 
java.lang.Object : boolean equals(Object) 


*/

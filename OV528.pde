import processing.serial.*;

Serial myPort;

String filename = "photo.jpg";
byte[] photo = {};
Boolean readData = false;
PImage captureImage;
long millis_actuelle;
long octet_entrant;

void setup(){
  
  size(640,480);
  String portName = Serial.list()[0];
  myPort = new Serial( this, portName, 115200 );
  
}
void draw(){
  
  byte[] buffer = new byte[128];
  
  if( readData ){
  
    while( myPort.available() > 0 ){
      millis_actuelle = millis(); 
      int readBytes = myPort.readBytes( buffer );
      octet_entrant += readBytes;
      frame.setTitle("Lecture " + octet_entrant + " octets ..."  );
      
      for( int i = 0; i < readBytes; i++ ){
        
        photo = append( photo, buffer[i] );
      
      }

    }
   
   if ((millis() - millis_actuelle) > 2000){
   
     readData = false;
     octet_entrant = 0;
     keyPressed();
     
   }

  }

  else{while( myPort.available() > 0 ){println( myPort.readString() );}}
  
}

void keyPressed(){

  if( photo.length > 0 ) {
  
    readData = false;
    frame.setTitle("Ecriture vers le disque " + photo.length +" octets ..." );
    saveBytes( filename, photo );
    frame.setTitle("Termine!");
    photo = new byte[0];
    millis_actuelle = millis();
    captureImage = loadImage(filename);
    image(captureImage, 0, 0);
  
  }
  
  else {
  
    readData = true;
    myPort.write(0);
    frame.setTitle("En attente de donnees, Veuillez appuyer sur le bouton de prise de photo."  );
  
  }
  
}

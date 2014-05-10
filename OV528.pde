import processing.serial.*;

Serial myPort;
String filename = "photo.jpg";
byte[] photo = {};
Boolean readData = false;
PImage captureImage;

void setup(){
  
  size(640,480);
  String portName = Serial.list()[0];
  myPort = new Serial( this, portName, 115200 );
  
}
void draw(){
  
  byte[] buffer = new byte[128];
  
  if( readData ){
  
    while( myPort.available() > 0 ){
      
      int readBytes = myPort.readBytes( buffer );
      print( "Lecture " );
      print( readBytes );
      println( " octets ..." );
      
      for( int i = 0; i < readBytes; i++ ){
        
        photo = append( photo, buffer[i] );
      
      }

    }

  }

  else{while( myPort.available() > 0 ){println( myPort.readString() );}}
  
}

void keyPressed(){

  if( photo.length > 0 ) {
  
    readData = false;
    print( "Ecriture vers le disque " );
    print( photo.length );
    println( " octets ..." );
    saveBytes( filename, photo );
    println( "Termine!" );
    photo = new byte[0];

    captureImage = loadImage(filename);
    image(captureImage, 0, 0);
  
  }
  
  else {
  
    readData = true;
    myPort.write(0);
    println( "En attente de donnees ..." );
  
  }

}

import processing.serial.*;

Serial mon_port;

String nom_du_fichier = "photo.jpg";
byte[] photo = {};
Boolean en_reception = false;
PImage image_recue;
long millis_actuelle;
long longueur_totale;

void setup(){
  
  size(640,480);
  String nom_du_port = Serial.list()[0];
  mon_port = new Serial( this, nom_du_port, 115200 );
  
}
void draw(){
  
  byte[] tampon_entrant = new byte[128];
  
  if( en_reception ){
  
    while( mon_port.available() > 0 ){
      millis_actuelle = millis(); 
      int longueur_partielle = mon_port.readBytes( tampon_entrant );
      longueur_totale += longueur_partielle;
      frame.setTitle("En réception: " + longueur_totale + " octets entrés..."  );
      
      for( int i = 0; i < longueur_partielle; i++ ){
        
        photo = append( photo, tampon_entrant[i] );
      
      }

    }
   
   if ((millis() - millis_actuelle) > 2000){
   
     en_reception = false;
     longueur_totale = 0;
     keyPressed();
     
   }

  }

  else{while( mon_port.available() > 0 ){println( mon_port.readString() );}}
  
}

void keyPressed(){

  if( photo.length > 0 ) {
  
    en_reception = false;
    frame.setTitle("Écriture vers le disque " + photo.length +" octets..." );
    saveBytes( nom_du_fichier, photo );
    frame.setTitle("Terminé!");
    photo = new byte[0];
    millis_actuelle = millis();
    image_recue = loadImage(nom_du_fichier);
    image(image_recue, 0, 0);
  
  }
  
  else {
  
    en_reception = true;
    mon_port.write(0);
    frame.setTitle("En attente de données, veuillez appuyer sur le bouton de l'appareil photo."  );
  
  }
  
}

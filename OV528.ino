#include <arduino.h>

#define PIC_PKT_LEN    128 
#define PIC_FMT_VGA    7
#define PIC_FMT_CIF    5
#define PIC_FMT_OCIF   3
#define CAM_ADDR       0

#define PIC_FMT        PIC_FMT_VGA

const byte cameraAddr = (CAM_ADDR << 5);  
const int broche_bouton = A5;                 
unsigned long picTotalLen = 0;            
int picNameNum = 0;


void setup(){
  
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(broche_bouton, INPUT_PULLUP);
  initializer();
  
}

void loop(){
  
  int n = 0;

  while(1){
  
    Serial.print(F("Code vient en grande partie de moi, et de quelques copier+coller trouvés ici et la. Je tiens a donner un remerciement special a Seeed-Studion\n"));
    Serial.print(F("Appuyez sur une touche du clavier pour activer la reception sur Processing ensuite sur le bouton pour activer l'envoie de la photo.\n"));

    while (digitalRead(broche_bouton) == HIGH);
  
    delay(200);
    capturer();
    obtenir_donnees();
  
  }

}

void vider_tampon_Rx(){

  while (Serial1.available()){Serial1.read();}
}
void sendCmd(char cmd[], int cmd_len){

  for (char i = 0; i < cmd_len; i++) Serial1.print(cmd[i]);

}

void initializer(){

  char cmd[] = {0xaa,0x0d|cameraAddr,0x00,0x00,0x00,0x00} ;
  unsigned char resp[6];

  Serial1.setTimeout(500);
  
  while (1){

    sendCmd(cmd,6);
    if (Serial1.readBytes((char *)resp, 6) != 6){continue;}

    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x0d && resp[4] == 0 && resp[5] == 0){

      if (Serial1.readBytes((char *)resp, 6) != 6) continue;
      if (resp[0] == 0xaa && resp[1] == (0x0d | cameraAddr) && resp[2] == 0 && resp[3] == 0 && resp[4] == 0 && resp[5] == 0) break;
        
    }
    
  }
    
  cmd[1] = 0x0e | cameraAddr;
  cmd[2] = 0x0d;
  sendCmd(cmd, 6);

}

void capturer(){
  
  char cmd[] = { 0xaa, 0x06 | cameraAddr, 0x08, PIC_PKT_LEN & 0xff, (PIC_PKT_LEN>>8) & 0xff ,0};
  unsigned char resp[6];

  Serial1.setTimeout(100);

  while (1){
  
    vider_tampon_Rx();
    sendCmd(cmd, 6);
    if (Serial1.readBytes((char *)resp, 6) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x06 && resp[4] == 0 && resp[5] == 0) break;
  }
  
  cmd[1] = 0x05 | cameraAddr;
  cmd[2] = 0;
  cmd[3] = 0;
  cmd[4] = 0;
  cmd[5] = 0;
  
  while (1){
    
    vider_tampon_Rx();
    sendCmd(cmd, 6);
    if (Serial1.readBytes((char *)resp, 6) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x05 && resp[4] == 0 && resp[5] == 0) break;
    
  }
  
  cmd[1] = 0x04 | cameraAddr;
  cmd[2] = 0x1;
  
  while (1){
        
    vider_tampon_Rx();
    sendCmd(cmd, 6);
    if (Serial1.readBytes((char *)resp, 6) != 6) continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x04 && resp[4] == 0 && resp[5] == 0){
            
      Serial1.setTimeout(1000);
      
      if (Serial1.readBytes((char *)resp, 6) != 6){continue;}

      if (resp[0] == 0xaa && resp[1] == (0x0a | cameraAddr) && resp[2] == 0x01){

        picTotalLen = (resp[3]) | (resp[4] << 8) | (resp[5] << 16);
        break;
      }
  
    }
  
  }

}

void obtenir_donnees(){
    
  unsigned int pktCnt = (picTotalLen) / (PIC_PKT_LEN - 6);
  if ((picTotalLen % (PIC_PKT_LEN-6)) != 0) pktCnt += 1;
  char cmd[] = { 0xaa, 0x0e | cameraAddr, 0x00, 0x00, 0x00, 0x00 };
  unsigned char pkt[PIC_PKT_LEN];
  
  Serial1.setTimeout(1000);

  for (unsigned int i = 0; i < pktCnt; i++){
  
    cmd[4] = i & 0xff;
    cmd[5] = (i >> 8) & 0xff;
    int retry_cnt = 0;
    
retry:
    
    delay(10);
    vider_tampon_Rx();
    sendCmd(cmd, 6);
    uint16_t cnt = Serial1.readBytes((char *)pkt, PIC_PKT_LEN);
    unsigned char sum = 0;
    for (int y = 0; y < cnt - 2; y++){sum += pkt[y];}

    if (sum != pkt[cnt-2]){
                
      if (++retry_cnt < 100) goto retry;
      else break;
    
    }

    Serial.write((const uint8_t *)&pkt[4], cnt-6);
  
  }
  
  cmd[4] = 0xf0;
  cmd[5] = 0xf0;
  sendCmd(cmd, 6);

}

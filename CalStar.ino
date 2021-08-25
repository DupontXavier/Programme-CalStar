  //**************************************************************
    // Commande du module de calibration Cal'Star
    // pour spectrographe Star'Ex
    // adaptation/simplification du sketch de pilotage du Calibrex dédié au Spectrographe Uvex
    // Xavier DUPONT 08/2021
    //**************************************************************
                   
    const int Pin_Inter_Ecran = A1;
    const int Pin_Inter_Calibre = A2;
    const int Pin_Inter_Flat = A3;
    
    const int Pin_Flat = 8;
    const int Pin_Led_Ecran = 3;
    const int Pin_Led_Flat = 4;
    const int Pin_Led_Calibre = 5;
    const byte Pin_HT = 10;
    const byte Pin_HT_Op = 12;
    
    int TestEcran;
    int TestCalibre;
    int TestFlat;
    
    bool Ecran = false ;
    bool Calibre = false ;
    bool Flat = false ;
    byte compteur = 0 ;
                   
    void setup()
     {               
         Serial.begin(9600);     
         Serial.println("Demarrage");
         
         //******** Initialisation du Timer2 pour déclencher les interruptions ************
        
         TCCR2A = 0; //default
         TCCR2B = 0b00000110; // clk/256 est incrémenté toutes les 16uS
         TIMSK2 = 0b00000001; // TOIE2
         sei();               // autorise les interruptions
         
         //***************** Définition des pins d'entrée/sortie **********************
         
         pinMode(Pin_HT, OUTPUT );
         pinMode(Pin_HT_Op, OUTPUT);
         pinMode(Pin_Led_Ecran, OUTPUT);
         pinMode(Pin_Led_Flat, OUTPUT);
         pinMode(Pin_Led_Calibre, OUTPUT);
         pinMode(Pin_Inter_Ecran, INPUT_PULLUP ); // Inter Ecran pour dark ou calibration/flat
         pinMode(Pin_Inter_Calibre, INPUT_PULLUP ); // Inter lampe de calibration
         pinMode(Pin_Inter_Flat, INPUT_PULLUP ); // Inter lampe de flat
         
      }

    void loop()
    
   {

    TestEcran = digitalRead(Pin_Inter_Ecran);
              delay(10); // anti-rebond 10ms en supplément du PULLUP...
              if (TestEcran == LOW && Ecran == false )
                  { Ecran = true;
                    digitalWrite(Pin_Led_Ecran, HIGH);
                    Serial.println("Ecran en place");
                  }  
              else if (TestEcran == HIGH && Ecran == true ) 
                  { Ecran = false;
                     Serial.println("Ecran escamoté");
                     digitalWrite(Pin_Led_Ecran, LOW);
                  }

                  
   if (Ecran == true)
                    {
                      TestCalibre = digitalRead(Pin_Inter_Calibre);
                              delay(10);
                              if (TestCalibre == LOW && Calibre == false)
                              {Calibre = true;
                              digitalWrite(Pin_Led_Calibre, HIGH);
                              Serial.println("Lampe calibration allumée");
                              
                              }    
                              else if (TestCalibre == HIGH && Calibre == true) 
                              {Calibre = false;
                              digitalWrite(Pin_HT_Op,LOW);
                              digitalWrite(Pin_HT,LOW);
                              digitalWrite(Pin_Led_Calibre, LOW);
                              Serial.println("Lampe calibration éteinte");
                              }
                  
              
                      TestFlat = digitalRead(Pin_Inter_Flat);
                              delay(10);
                              if (TestFlat == LOW && Flat == false )
                              {Flat = true;
                               digitalWrite(Pin_Flat,HIGH);
                               digitalWrite(Pin_Led_Flat, HIGH);
                               Serial.println("Lampe Flat allumée");
                              }    
                              else if (TestFlat == HIGH && Flat == true ) 
                              {Flat = false;
                              digitalWrite(Pin_Flat, LOW);
                              digitalWrite(Pin_Led_Flat, LOW); 
                              Serial.println("Lampe flat éteinte");
                              }
                    }
   else if (Calibre == true || Flat == true)
          {
          Calibre = false;
          digitalWrite(Pin_HT_Op,LOW);
          digitalWrite(Pin_HT,LOW);
          digitalWrite(Pin_Led_Calibre, LOW);
          Flat = false;
          pinMode(Pin_Flat, LOW); 
          digitalWrite(Pin_Led_Flat, LOW); 
          Serial.println("Lampe calibration et flat éteinte");
          }
    }    
      
// *********** Creation du signal alternatif par le timer *************

ISR(TIMER2_OVF_vect)
{
  TCNT2 =200;
  
  if (Calibre == true)
  {
      if (compteur++ == 1)
      {
          compteur = 0;
          bool etat = digitalRead(Pin_HT);
          digitalWrite (Pin_HT,!etat);
          digitalWrite(Pin_HT_Op,!digitalRead(Pin_HT)); // déphasage Luciole résistance 220 K-Ohms
          
      }       
  }
}       

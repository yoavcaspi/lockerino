#include <MD5.h>
//#include <sha256.h>


//Crypto definitions

#define MY_AES_KEY_LENGTH 256 
byte key[] = 
{
  0x75, 0x6c, 0x55, 0x54, 0x41, 0x58, 0x66, 0x72, 0x56, 0x4b, 0x4e, 0x74, 0x52, 0x62, 0x47, 0x63,
  0x63, 0x66, 0x74, 0x6c, 0x6b, 0x70, 0x67, 0x38, 0x34, 0x44, 0x45, 0x67, 0x4c, 0x6e, 0x4a, 0x5a,
} ;

/*uint8_t hmackey[] = {
  0x67, 0x43, 0x37, 0x30, 0x55, 0x50, 0x52, 0x78, 0x78, 0x5a, 0x42, 0x69, 0x77, 0x78, 0x6f, 0x76,
  0x46, 0x73, 0x51, 0x54, 0x6b, 0x65, 0x34, 0x45, 0x6d, 0x4f, 0x44, 0x78, 0x36, 0x46, 0x44, 0x4a,
} ;*/



byte my_iv[] = 
{
  0xa1, 0x10, 0xe6, 0xb9, 0xa3, 0x61, 0x65, 0x3a, 0x04, 0x2e, 0x3f, 0x5d, 0xfb, 0xac, 0x4c, 0x61,
} ;


/*byte check [4*N_BLOCK] ;
byte mycheck [4*N_BLOCK];*/

void cryptoSetup(){
    aes.set_key (key, MY_AES_KEY_LENGTH);
  //  unsigned char* hash=MD5::make_hash("hello world");
    //Sha256.init();
    //Sha1.initHmac(hmackey, 32); // key, and length of key in bytes
}

//AES Encryption function 
// TODO: Go Over it  
// Usage Example: prekey (128, 1) ;
/*boolean encrypt(int blocks){
  //Note bits is 256

  boolean succ = false;
  for (byte i = 0 ; i < N_BLOCK ; i++)
    iv[i] = my_iv[i];
  succ = aes.cbc_encrypt(plain, cipher, blocks, iv);
  return succ;
}*/


String encryptCommand(char command){
    byte iv [N_BLOCK];
    String s = String(command);
    s += "&t=";
    String t = String(index);
    index++;
    s += t;
    
    s.getBytes(plain,70);
//    Serial.print("s = ");
//    Serial.println(s);
    for (byte i = 0 ; i < 16 ; i++)
        iv[i] = my_iv[i] ;
    aes.cbc_encrypt(plain,cipher,1,iv);
    base64_encode(cipher64,cipher,16);
    t = String((char *)cipher64);
    return t;
}

String encryptCommandNFC(char command, uint32_t uid){
    byte iv [N_BLOCK];
    String s = String(command);
    s += "&t=";
    String t = String(index);
    index++;
    s += t;
    t = String(uid);
    s += "&n=";
    s += t;
    
    s.getBytes(plain,70);
//    Serial.print("s = ");
//    Serial.println(s);
    for (byte i = 0 ; i < 16 ; i++)
        iv[i] = my_iv[i] ;
    aes.cbc_encrypt(plain,cipher,1,iv);
    base64_encode(cipher64,cipher,16);
    t = String((char *)cipher64);
    return t;
}

String decryptCommand(String str){
    byte iv [N_BLOCK];
    str.getBytes(plain64,70);
    int j = base64_decode(cipher64,plain64,24);
    
   /* Serial.print("j = ");
    Serial.println(j);
    Serial.print("cipher64 = ");
    int k = 0;
    while (cipher64[k]!=0){
        Serial.println(cipher64[k]);
        k++;    
    }*/
    
    for (byte i = 0 ; i < 16 ; i++)
        iv[i] = my_iv[i] ;
    aes.cbc_decrypt(cipher64,plain,1,iv);
    
    String t = String((char*)plain);
    return t;
   
}
/*
void prekey (int bits, int blocks)
{
	
  
	long t0 = micros () ;
	byte succ = aes.set_key (key, bits) ;
	long t1 = micros()-t0 ;
	/*if (DEBUG){
		Serial.print ("set_key ") ; Serial.print (bits) ; Serial.print ("->") ; Serial.print ((int) succ) ;
		Serial.print ("took:") ; Serial.print (t1) ; Serial.println ("us") ;
	}
	t0 = micros () ;
	if (blocks == 1)
		succ = aes.encrypt (plain, cipher) ;
	else
	{
		for (byte i = 0 ; i < N_BLOCK ; i++)
		  iv[i] = my_iv[i] ;
		succ = aes.cbc_encrypt (plain, cipher, blocks, iv) ;
	}
	t1 = micros () - t0 ;
	/*if (DEBUG){
		Serial.print ("encrypt ") ; Serial.print ((int) succ) ;
		Serial.print (" took ") ; Serial.print (t1) ; Serial.println ("us") ;
	}
	t0 = micros () ;
	if (blocks == 1)
		succ = aes.decrypt (cipher, mycheck) ;
	else
	{
		for (byte i = 0 ; i < 16 ; i++)
		iv[i] = my_iv[i] ;
		succ = aes.cbc_decrypt (cipher, check, blocks, iv) ;
	}
	t1 = micros () - t0 ;
	/*if (DEBUG){
		Serial.print ("decrypt ") ; Serial.print ((int) succ) ;
		Serial.print (" took ") ; Serial.print (t1) ; Serial.println ("us") ;
	}
	
	for (byte ph = 0 ; ph < (blocks == 1 ? 3 : 4) ; ph++)
	{
		for (byte i = 0 ; i < (ph < 3 ? blocks*N_BLOCK : N_BLOCK) ; i++)
		{
			byte val = ph == 0 ? plain[i] : ph == 1 ? cipher[i] : ph == 2 ? mycheck[i] : iv[i] ;
			Serial.print (val>>4, HEX) ; Serial.print (val&15, HEX) ; Serial.print (" ") ;
		}
		Serial.println () ;
	}
}*/


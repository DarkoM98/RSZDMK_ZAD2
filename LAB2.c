#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>


#define USART_RX_BUFFER_SIZE 64
char Rx_Buffer[USART_RX_BUFFER_SIZE];			
volatile unsigned char Rx_Buffer_Size = 0;
volatile unsigned char Rx_Buffer_First = 0;
volatile unsigned char Rx_Buffer_Last = 0;
ISR(USART_RX_vect)
{
  	Rx_Buffer[Rx_Buffer_Last++] = UDR0;		
	Rx_Buffer_Last &= USART_RX_BUFFER_SIZE - 1;	
	if (Rx_Buffer_Size < USART_RX_BUFFER_SIZE)
		Rx_Buffer_Size++;				
}
void usartInit(unsigned long baud)
{
	UCSR0A = 0x00;	
					
	UCSR0B = 0x98;	

	UCSR0C = 0x06;	

	UBRR0 = F_CPU / (16 * baud) - 1;
	sei();
}
unsigned char usartAvailable()
{
	return Rx_Buffer_Size;
}
void usartPutChar(char c)
{
	while(!(UCSR0A & 0x20));
	UDR0 = c;
void usartPutString(char *s)
{
	while(*s != 0)
	{
		usartPutChar(*s);
		s++;
	}
}
void usartPutString_P(const char *s)
{
	while (1)
	{
		char c = pgm_read_byte(s++);
		if (c == '\0')
			return;
		usartPutChar(c);
	}
}
char usartGetChar()
{
	char c;
	if (!Rx_Buffer_Size)
		return -1;
  	c = Rx_Buffer[Rx_Buffer_First++];
	Rx_Buffer_First &= USART_RX_BUFFER_SIZE - 1;
	Rx_Buffer_Size--;
	return c;
}
unsigned char usartGetString(char *s)
{
	unsigned char len = 0;
	while(Rx_Buffer_Size)
		s[len++] = usartGetChar();
	s[len] = 0;
	return len;
}
char korisnici[10][32] =
{
    "Sundjer Bob Kockalone",
    "Dijego Armando Maradona",
    "Bond. Dzejms bond.",
    "Zoran Kostic Cane",
    "Kim Dzong Un",
    "Si Dji Ping",
    "Lazo Pajčin", 
    "Mirko Pajčin",
    "Goran Goci Ristić",
    "Marko Kon"
};
char PINCODE[10][5] = {"5346", "2133", "7445", "8756", "7435", "0667", "1998", "2908", "2017", "0221"};

int main()
{

	usartInit(9600);

	while(1)
	{
      int i=0;
      int id=-1;
      char ime[32];
      char pin[4];
      int attempt=3;
      char digit[1];
      
      usartPutString("Dobar dan, molimo vas, unesite ime i prezime:\r\n");
      
      while(!usartAvailable());
      _delay_ms(50);
      
      usartGetString(ime);
      
      for(i=0; i<10; i++)
       if(!strcmp(ime,korisnici[i])) id=i;
        
      if(id<0){
        usartPutString("Korisnicko ime ne postoji!\r\n");
        continue;
      }
      
      usartPutString("Dobrodosli ");
      usartPutString(ime);
      usartPutString("! \r\nUnesite PIN (4 cifre) redom: ");
      
      while(1){
        
        while(!usartAvailable());
        _delay_ms(50);
        usartGetString(digit);
        
        strcpy(pin,digit);
        usartPutString("*");
          
        while(!usartAvailable());
        _delay_ms(50);
        usartGetString(digit);
        
        strcat(pin,digit);
        usartPutString("*");
        
        while(!usartAvailable());
        _delay_ms(50);
        usartGetString(digit);
        
        strcat(pin,digit);
        usartPutString("*");
        
        while(!usartAvailable());
        _delay_ms(50);
        usartGetString(digit);
        
        strcat(pin,digit);
        usartPutString("*\r\n");
        if(!strcmp(pin,PINCODE[id])) break;
        
        attempt--;
        usartPutString("PIN nije validan! ");
        
        if (!attempt){
          usartPutString("3 puta ste pogresili PIN! Nemate pravo daljeg unosa!\r\n");
          break;
        }
        usartPutString("Imate 3 dozvoljena pokusaja:\r\n");
      }
      if (!attempt) continue;
      
      usartPutString("Uspesno ste uneli PIN! \r\n");
      usartPutString("Unesite podatke o novom korisniku. \r\n");
      
      while(!usartAvailable());
      usartGetString(ime); //flush
    }
	return 0;
}
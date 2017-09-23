char UART_init()
{
    TXSTA = 0x20; 			//low baud rate
	SPBRG = 31;			//val
	RCSTAbits.SPEN = 1;
}

//WRITING CHARACTER
void UART_Write(char data)
{
  while(!TXSTAbits.TRMT);
  TXREG = data;
}
//Checking Transmit Register

char UART_TX_Empty()
{
  return TXSTAbits.TRMT;
}

//Writing Text

void UART_Write_Text(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
    UART_Write(text[i]);
}
//Data Received or Not
char UART_Data_Ready()
{
  return PIR1bits.RCIF;
}

//Reading a Character
char UART_Read()
{
  while(!PIR1bits.RCIF);
  return RCREG;
}


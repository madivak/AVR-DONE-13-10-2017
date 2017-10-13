void USART0_Init( unsigned int ubrr);
void USART1_Init( unsigned int ubrr);
int USART0_Transmit( char data0, FILE *stream);
int USART0_Receive( FILE *stream );
int USART1_Transmit( char data1, FILE *stream );
int USART1_Receive( FILE *stream );


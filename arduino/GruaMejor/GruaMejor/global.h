//prendo el led
#define ONLEDGREEN PORTD |= (1<<PORTD5)
#define OFFLEDGREEN PORTD &= ~(1<<PORTD5)

#define ONLEDBLUE PORTD |= (1<<PORTD6)
#define OFFLEDBLUE PORTD &= ~(1<<PORTD6)

extern uint8_t FLAG_button_A;
extern uint8_t FLAG_button_B;
extern uint8_t FLAG_button_A_change,FLAG_button_B_change;
#include <16f877a.h>
#fuses HS, NOPROTECT, NOLVP
#device ADC=10
#use delay(clock=20M)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

// LCD Module Connections
#define LCD_RS_PIN PIN_D7
#define LCD_RW_PIN PIN_D0
#define LCD_ENABLE_PIN PIN_D6
#define LCD_DATA4 PIN_D5
#define LCD_DATA5 PIN_D4
#define LCD_DATA6 PIN_D3
#define LCD_DATA7 PIN_D2
// End LCD Module Connections

#include <lcd.c>

int edo_boton = 0, edo_menu = 0, menu = 1;
char grados = 0xDF;

#INT_RB
void isr_rb(void) {
    switch(input_b()) {
        case 0b01110000:
            edo_boton = 1;
            break;
        case 0b10110000:
            edo_boton = 2;
            break;
        case 0b11010000:
            edo_boton = 3;
            break;
        case 0b11100000:
            edo_boton = 4;
            break;
    }
    delay_ms(20);
}

void menu_inicio() {
    edo_boton = 0;

    switch (menu) {
        case 1:
            lcd_putc('\f');
            lcd_putc("* Temperatura");
            lcd_gotoxy(1,2);
            lcd_putc(" Voltimetro");
            break;
        case 2:
            lcd_putc('\f');
            lcd_putc(" Temperatura");
            lcd_gotoxy(1,2);
            lcd_putc("* Voltimetro");
            break;
        case 3:
            lcd_putc('\f');
            lcd_putc(" Voltimetro");
            lcd_gotoxy(1,2);
            lcd_putc("* Amperimetro");
            break;
    }

    while(!edo_boton);

    switch(edo_boton) {
        case 1:
            menu--;
            if(menu < 1)
                menu = 1;
            break;
        case 2:
            menu++;
            if(menu > 3)
                menu = 3;
            break;
        case 3:
            edo_menu = menu;
            break;
        case 4:
            menu = menu;
            break;
    }
}

void temperatura() {
    int temperatura_entrada;
    float temperatura_salida;

    set_adc_channel(0);
    edo_boton = 0;
    delay_us(50);
    lcd_putc('\f');
    lcd_putc(" Temperatura: ");

    while (edo_boton != 4) {
        temperatura_entrada = read_adc();
        temperatura_salida = temperatura_entrada * 0.483;
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.1f %c", temperatura_salida, grados);
        delay_ms(25);
    }

    menu = 1;
    edo_menu = 0;
}

void voltaje() {
    float volt;
    edo_boton = 0;
    
    set_adc_channel(1);
    delay_us(50);

    lcd_putc("\f Voltaje: ");
    while (edo_boton != 4) {
        volt = (float) read_adc() * ((5.0 / 1024.0) * 4.1);
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.2f V. ", volt);
        delay_ms(25);
    }

    menu = 1;
    edo_menu = 0;
}

void corriente() {
    int valor_entrada;
    float v, i;
    float r = 0.1;
    edo_boton = 0;

    set_adc_channel(2);

    delay_us(50);
    lcd_putc("\fCorriente: ");

    while (edo_boton != 4) {
        valor_entrada = (float) read_adc();
        v = valor_entrada * ((5.0 / 1023.0) * 4.09);
        i = v / r;
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.2f A. ", i);
        delay_ms(25);
    }

    menu = 1;
    edo_menu = 0;
}

void initial_config() {
    setup_adc_ports(AN0_AN1_AN2_AN3_AN4_AN5);
    setup_adc(ADC_CLOCK_INTERNAL);
    output_b(0);
    set_tris_b(0xF0);
    enable_interrupts(INT_RB);
    enable_interrupts(GLOBAL);
}

void creditos() {
    lcd_init();
    lcd_putc("\fPF: Multimetro Digital");
    lcd_gotoxy(1,2);
    lcd_putc("Equipo 6, Grupo 6");
    delay_ms(75);
    lcd_gotoxy(1,1);
    lcd_putc("\fLopez Carrasco");
    lcd_gotoxy(1,2);
    lcd_putc("Karla");
    delay_ms(75);
    lcd_gotoxy(1,1);
    lcd_putc("\fReyes Valderrama");
    lcd_gotoxy(1,2);
    lcd_putc("Rodrigo Miguel");
    delay_ms(75);
}

void main() 
{
    initial_config();
    creditos();

    while(true) {
        switch(edo_menu) {
            case 0:
                menu_inicio();
                break;
            case 1:
                temperatura();
                break;
            case 2:
                voltaje();
                break;
            case 3:
                corriente();
                break;
        }
    }
}

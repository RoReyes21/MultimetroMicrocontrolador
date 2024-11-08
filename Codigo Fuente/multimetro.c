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

// Enumeraciones para los estados del menú y del botón
typedef enum {
    MENU_INICIO,
    MENU_TEMPERATURA,
    MENU_VOLTAGE,
    MENU_CORRIENTE
} menu_state_t;

typedef enum {
    BOTON_NINGUNO,
    BOTON_ARRIBA,
    BOTON_ABAJO,
    BOTON_SELECT,
    BOTON_EXIT
} boton_state_t;

menu_state_t estado_menu = MENU_INICIO;
boton_state_t estado_boton = BOTON_NINGUNO;
char grados = 0xDF;

#INT_RB
void isr_rb(void) {
    switch(input_b()) {
        case 0b01110000:
            estado_boton = BOTON_ARRIBA;
            break;
        case 0b10110000:
            estado_boton = BOTON_ABAJO;
            break;
        case 0b11010000:
            estado_boton = BOTON_SELECT;
            break;
        case 0b11100000:
            estado_boton = BOTON_EXIT;
            break;
        default:
            estado_boton = BOTON_NINGUNO;
            break;
    }
    delay_ms(20);
}

void mostrar_menu_inicio() {
    switch(estado_menu) {
        case MENU_INICIO:
            lcd_putc('\f');
            lcd_putc("* Temperatura");
            lcd_gotoxy(1,2);
            lcd_putc(" Voltimetro");
            break;
        case MENU_TEMPERATURA:
            lcd_putc('\f');
            lcd_putc(" Temperatura");
            lcd_gotoxy(1,2);
            lcd_putc("* Voltimetro");
            break;
        case MENU_VOLTAGE:
            lcd_putc('\f');
            lcd_putc(" Voltimetro");
            lcd_gotoxy(1,2);
            lcd_putc("* Amperimetro");
            break;
    }
}

void actualizar_estado_menu() {
    switch (estado_boton) {
        case BOTON_ARRIBA:
            if (estado_menu > MENU_INICIO) {
                estado_menu--;
            }
            break;
        case BOTON_ABAJO:
            if (estado_menu < MENU_CORRIENTE) {
                estado_menu++;
            }
            break;
        case BOTON_SELECT:
            estado_boton = BOTON_NINGUNO;
            break;
        case BOTON_EXIT:
            estado_menu = MENU_INICIO;
            estado_boton = BOTON_NINGUNO;
            break;
    }
}

void temperatura() {
    int temperatura_entrada;
    float temperatura_salida;

    set_adc_channel(0);
    estado_boton = BOTON_NINGUNO;
    delay_us(50);
    lcd_putc('\f');
    lcd_putc(" Temperatura: ");

    while (estado_boton != BOTON_EXIT) {
        temperatura_entrada = read_adc();
        temperatura_salida = temperatura_entrada * 0.483;
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.1f %c", temperatura_salida, grados);
        delay_ms(25);
    }

    estado_menu = MENU_INICIO;
}

void voltaje() {
    float volt;
    estado_boton = BOTON_NINGUNO;
    
    set_adc_channel(1);
    delay_us(50);

    lcd_putc("\f Voltaje: ");
    while (estado_boton != BOTON_EXIT) {
        volt = (float) read_adc() * ((5.0 / 1024.0) * 4.1);
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.2f V. ", volt);
        delay_ms(25);
    }

    estado_menu = MENU_INICIO;
}

void corriente() {
    int valor_entrada;
    float v, i;
    float r = 0.1;
    estado_boton = BOTON_NINGUNO;

    set_adc_channel(2);

    delay_us(50);
    lcd_putc("\fCorriente: ");

    while (estado_boton != BOTON_EXIT) {
        valor_entrada = (float) read_adc();
        v = valor_entrada * ((5.0 / 1023.0) * 4.09);
        i = v / r;
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.2f A. ", i);
        delay_ms(25);
    }

    estado_menu = MENU_INICIO;
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

    while (true) {
        switch(estado_menu) {
            case MENU_INICIO:
                mostrar_menu_inicio();
                actualizar_estado_menu();
                break;
            case MENU_TEMPERATURA:
                temperatura();
                break;
            case MENU_VOLTAGE:
                voltaje();
                break;
            case MENU_CORRIENTE:
                corriente();
                break;
        }
    }
}

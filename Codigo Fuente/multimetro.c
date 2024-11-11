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
//End LCD Module Connections

#include <lcd.c>

enum seleccion_opciones_menu {
    SELECCION_VOLTAJE = 1,
    SELECCION_AMPERAJE,
    SELECCION_TEMPERATURA
};

enum estados_menu {
    ESTADO_MENU_INICIO,
    ESTADO_MENU_VOLTAJE,
    ESTADO_MENU_AMPERAJE,
    ESTADO_MENU_TEMPERATURA
};

void creditos();
void initial_config();
void menu_inicio();
void mostrar_corriente();
void mostrar_voltaje();
void mostrar_temperatura();

int seleccion_menu_inicio = SELECCION_VOLTAJE;
int estado_menu = ESTADO_MENU_INICIO;
int refresh = false;

/**
 * @brief Interrupcion de botones, cuando reciba una interrupcion maneja los estados
 * 
 */
#INT_RB
void isr_rb(void) {
    switch(input_b()) {
        case 0b01110000:
            if (estado_menu == ESTADO_MENU_INICIO && (seleccion_menu_inicio > SELECCION_VOLTAJE)) {
                seleccion_menu_inicio--;
                refresh = true;
            }

            break;
        case 0b10110000:
            if ((estado_menu == ESTADO_MENU_INICIO) && (seleccion_menu_inicio < SELECCION_TEMPERATURA)) {
                seleccion_menu_inicio++;
                refresh = true;
            }

            break;
        case 0b11010000:
            if (estado_menu == ESTADO_MENU_INICIO) {
                estado_menu = seleccion_menu_inicio;
                refresh = true;
            }
            break;
        case 0b11100000:
            if (estado_menu != ESTADO_MENU_INICIO) {
                estado_menu = ESTADO_MENU_INICIO;
                refresh = true;
            }
            break;
    }
    //delay_ms(10);
}

void main() 
{
    initial_config();
    creditos();

    while(true) {
        refresh = false;

        switch(estado_menu) {
            case ESTADO_MENU_INICIO:
                menu_inicio();
                break;
            case ESTADO_MENU_VOLTAJE:
                mostrar_voltaje();
                break;
            case ESTADO_MENU_AMPERAJE:
                mostrar_corriente();
                break;
            case ESTADO_MENU_TEMPERATURA:
                mostrar_temperatura();
                break;
        }
    }
}

/**
 * @brief Configuracion inicial del microcontrolador antes de que empiece a ejecutar el resto del programa
 * 
 */
void initial_config() {
    setup_adc_ports(AN0_AN1_AN2_AN3_AN4_AN5);
    setup_adc(ADC_CLOCK_INTERNAL);
    output_b(0);
    set_tris_b(0xF0);
    enable_interrupts(INT_RB);
    enable_interrupts(GLOBAL);
}

/**
 * @brief Muestran los creditos del proyecto
 * 
 */
void creditos() {
    lcd_init();
    lcd_gotoxy(1,1);
    lcd_putc("\fMultimetro Dig.");
    lcd_gotoxy(1,2);
    lcd_putc("EQ 6, Grupo 6");
    delay_ms(100);
    lcd_gotoxy(1,1);
    lcd_putc("\fLopez Carrasco");
    lcd_gotoxy(1,2);
    lcd_putc("Karla");
    delay_ms(100);
    lcd_gotoxy(1,1);
    lcd_putc("\fReyes Valderrama");
    lcd_gotoxy(1,2);
    lcd_putc("Rodrigo Miguel");
    delay_ms(100);
}

/**
 * @brief Muestra en lcd las opciones del menu
 * 
 */
void menu_inicio() {

    switch (seleccion_menu_inicio) {
        case SELECCION_VOLTAJE:
            lcd_putc("\f->> Voltimetro");
            lcd_gotoxy(1,2);
            lcd_putc(" Amperimetro");
            break;
        case SELECCION_AMPERAJE:
            lcd_putc("\f->> Amperimetro");
            lcd_gotoxy(1,2);
            lcd_putc(" Temperatura");
            break;
        case SELECCION_TEMPERATURA:
            lcd_putc("\f Amperimetro");
            lcd_gotoxy(1,2);
            lcd_putc("->> Temperatura");
            break;
    }

    while(!refresh);
}

/**
 * @brief Calcula la temperatura y la muestra en el LCD
 * 
 */
void mostrar_temperatura() {
    int temperatura_entrada;
    float temperatura_salida;
    char grados = 0xDF;

    set_adc_channel(0);
    delay_us(40);
    
    lcd_putc("\f Temperatura: ");
    while (!refresh) {
        temperatura_entrada = read_adc();
        temperatura_salida = temperatura_entrada * 0.483;
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.1f %c", temperatura_salida, grados);
        delay_ms(20);
    }
}

/**
 * @brief Calcula el voltaje y muestra en LCD
 * 
 */
void mostrar_voltaje() {
    float voltaje;

    set_adc_channel(1);
    delay_us(40);

    lcd_putc("\f Voltaje: ");
    while (!refresh) {
        voltaje = (float) read_adc() * ((5.0 / 1024.0) * 4.1);
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.2f [V]", voltaje);
        delay_ms(20);
    }
}

/**
 * @brief Calcula la corriente y muestra en LCD
 * 
 */
void mostrar_corriente() {
    float voltaje, corriente, resistencia = 0.1;
    int valor_entrada;

    set_adc_channel(2);
    delay_us(40);
    
    lcd_putc("\f Corriente: ");
    while (!refresh) {
        valor_entrada = (float) read_adc();
        voltaje = valor_entrada * ((5.0 / 1023.0) * 4.09);
        corriente = voltaje / resistencia;
        lcd_gotoxy(1,2);
        printf(lcd_putc," %2.2f [A]", corriente);
        delay_ms(20);
    }
}

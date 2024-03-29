#include <18F4550.h>
#DEVICE ADC=8
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN
#use delay(clock=48M)
#DEFINE USB_HID_DEVICE  TRUE
#define USB_EP1_TX_ENABLE  USB_ENABLE_INTERRUPT   //turn on EP1 for IN bulk/interrupt transfers
#define USB_EP1_TX_SIZE    8
#define USB_EP1_RX_ENABLE  USB_ENABLE_INTERRUPT   //turn on EP1 for OUT bulk/interrupt transfers
#define USB_EP1_RX_SIZE    8
#include <pic18_usb.h>//Funciones de bajo nivel(hardware) para la serie PIC 18Fxx5x que serviran en usb.c
#include <usb_desc_hid.h>//Aqui es donde van las descripciones de este dispositivo (como lo reconocera windows)
#include <usb.c>        //libreria para el manejo del usb
#define LEDR PIN_C0  //Led rojo para la espera de la conexion USB
#define LEDV PIN_C6  //Led  verde, se enciende cuando el USB esta conectado
#define LED_ON output_low
#define LED_OFF output_high
int8 Salida[8];
int8 Entrada[8];
int8 count=0;
int8 HZ=0;
void main() 
        {
 // disable_interrupts(INT_EXT);
 // disable_interrupts(INT_EXT1);
 // disable_interrupts(INT_EXT2);
  // port_b_pullups(TRUE); si se utiliza el puerto b como entrada (0xFF)Switch a GND
   set_tris_a(0xFF);    
   set_tris_b(0x00); 
   set_tris_c(0x00);
   set_tris_d(0xFF);
   set_tris_e(0x0F);
   setup_adc_ports(AN0_to_AN5);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   setup_timer_0 (RTCC_EXT_H_TO_L); // Sin preescaler ---(RTCC_DIV_2|RTCC_EXT_L_TO_H)--- 
   set_timer0(0);
   setup_timer_2(T2_DIV_BY_1, 127, 1);
   //setup_ccp1(CCP_PWM);
   //setup_ccp2(CCP_PWM);
   set_pwm1_duty(0);
   set_pwm2_duty(0); 
   LED_ON(LEDR);
   LED_OFF(LEDV); 
   usb_init();                      
   usb_task();   //Monitorea el estado de la coneccion conectandose y desconectandose automaticamente
   usb_wait_for_enumeration();  //espera infinitamente hasta que el dispositivo fue enumerado    
   LED_ON(LEDV);
   LED_OFF(LEDR); 
   
   while (TRUE) 
       {  usb_task();
      if (usb_enumerated()) 
       {    for(count=0;count<6;count++)
       {
         set_adc_channel(count);
         delay_ms(10);
         Salida[count]=read_adc();
       }
         Salida[6]=HZ;
         Salida[7]=input_d();
            usb_put_packet(1, Salida, 8 , USB_DTS_TOGGLE);   // salida de datos hacia LABVIEW, ADC,PUSH,CONTADOR DE PULSOS 
         
         if (usb_kbhit(1)) 
       { 
           usb_get_packet(1, Entrada, 3); // entrada de datos provenientes de LABVIEW, SWTICH PWM,1.2 ETC.
           set_pwm1_duty(Entrada[0]);
           set_pwm2_duty(Entrada[1]);
           output_b(Entrada[2]);
       }
       }
       }
       }

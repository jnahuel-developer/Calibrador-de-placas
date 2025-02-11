/* ###################################################################
**     Filename    : Events.c
**     Project     : ProbadorComunicacionEntrePlacas
**     Processor   : MC9S08AC16CFD
**     Component   : Events
**     Version     : Driver 01.02
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2019-02-14, 08:47, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.02
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */


#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

extern bool flagTerminoElTimer;
extern bool flagRecibido;
extern unsigned char	leido;

extern bool flagTerminoElTimer;
extern bool	semaforoADC;


extern unsigned int frecuenciaDeLaPlaca;
extern bool	finalizoElTimer;

extern bool flagRecibido;
extern unsigned char	leido;

// Variable interna para llevar la cuenta de los pulsos
unsigned int	pulsosCaudalimetro;


extern void borrarFlagADC ( void );


ISR(Interrupcion_TIM3_overflow)
{
	// Se borra el flag de la interrupcion
	clrReg8Bits(TPM3SC, 0x80U);
	
	// Se indica que ingreso a la interrupcion
	flagTerminoElTimer = TRUE;
}


ISR(Interrupcion_RecepcionSerie)
{
	// Se debe realizar el mecanismo de borrado del flag
	(void) SCI1S1;
	
	// Se almacena el dato
	leido = SCI1D;

	// Se indica mediante el flag que se recepciono algun dato
	flagRecibido = TRUE;
}


// Interrupcion del ADC
ISR(Interrupcion_ADC)					{	borrarFlagADC();	semaforoADC = TRUE;	}




// INTERRUPCION DEL TIMER 1 - DESBORDA A CADA SEGUNDO PARA SABER LA FRECUENCIA DEL CAUDALIMETRO
ISR(TI1_Interrupt)
{
	  // Se borra el flag del timer
	  clrReg8Bits(TPM1SC, 0x80U);        /* Reset overflow interrupt request flag*/
	  
	  // Se indica que termino el timer para actualizar el valor de la frecuencia
	  finalizoElTimer = TRUE;
	  
	  // Se actualiza el contador del caudalimetro
	  frecuenciaDeLaPlaca = ( pulsosCaudalimetro ) / 10;
	  pulsosCaudalimetro = 0;
}

// INTERRUPCION DEL TIMER 1 CANAL 0 - CUENTA LOS PULSOS DEL CAUDALIMETRO
ISR(TI1_Interrupt_ch0)
{
	  clrReg8Bits(TPM1C0SC, 0x80U);        /* Reset input capture interrupt request flag*/
	  pulsosCaudalimetro++;	
}




/* END Events */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/

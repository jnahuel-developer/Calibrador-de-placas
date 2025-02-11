/* ###################################################################
**     Filename    : Events.h
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
** @file Events.h
** @version 01.02
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Vtpm3ovf.h"
#include "Vsci1rx.h"
#include "Vadc1.h"
#include "Vtpm1ovf.h"
#include "Vtpm1ch0.h"


// Se declara un pin como salida para alternarlo a cada segundo, a modo de verificacion
#define  	SALIDA_VERIFICACION_DIRECCION					PTDDD_PTDDD2
#define  	SALIDA_VERIFICACION_PIN							PTDD_PTDD2


/* END Events */
#endif /* __Events_H*/

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

/* ###################################################################
**     Filename    : main.c
**     Project     : ProbadorComunicacionEntrePlacas
**     Processor   : MC9S08AC16CFD
**     Version     : Driver 01.12
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2019-02-14, 08:47, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.12
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Vtpm3ovf.h"
#include "Vsci1rx.h"
#include "Vadc1.h"
#include "Vtpm1ovf.h"
#include "Vtpm1ch0.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

// Prototipos de las funciones

void DemoraBloqueanteUnSegundo ( void );
void DemoraBloqueanteRestoParaUnSegundo ( void );

void EnviarDatoComunicacionSerie ( void );
void EnviarConfirmacionComunicacionSerie( void );
void EnviarOrdenDeAvanzarComunicacionSerie( void );
void InicializarComunicacionSerie ( void );


void ConfigurarADC ( void );
bool TomarMuestraSimpleADC ( char canal );
uint8_t leerADC ( void );
uint16_t leerADC10bits ( void );
void borrarFlagADC ( void );





void inicializarLCD_4bits( void );
void inicializarPuertoLCD( void );
void demoraInicialLCD( void );
void enviarInstruccion( char dato );
void enviarDato( char dato );
void mandarNibbleAltoLCD_Vacio( char dato );
void mandarNibbleBajoLCD_Vacio( char dato );
bool EscribirMensajeLCD( unsigned char renglon, unsigned char inicioTexto, unsigned char totalDeCaracteres, unsigned char *pTexto);
void EscribirFrecuencia( unsigned int numero );
void EscribirDuty( unsigned int numero );
void EscribirAnchoDePulso( unsigned int numero );
void EscribirTension5V( unsigned int numero );
void EscribirTension( unsigned int numero );
void EscribirMensaje( unsigned char renglonSuperior[8], unsigned char renglonInferior[8] );
void EscribirMensajeRenglonSuperior( unsigned char renglonSuperior[8] );
void EscribirMensajeRenglonInferior( unsigned char renglonInferior[8] );
void EscribirFrecuenciaRenglonSuperior( unsigned int numero );
void EscribirDatoEnviado( unsigned int numero );
void EscribirDatoRecibido( unsigned int numero, char condicion );


bool RevisarOpresionDelPulsador ( char pulsador );
void AvanzarPasosEnLaCalibracion ( char cantidadDePasos );

void HabilitarMedicionDeFrecuenciaYPeriodo ( void );
void HabilitarTimerComoGeneral ( void );





// Defines para los pines de la placa de calibracion
#define		PULSADOR_OK								0
#define  	PULSADOR_OK_DIRECCION					PTCDD_PTCDD1
#define  	PULSADOR_OK_PIN							PTCD_PTCD1
#define  	PULSADOR_OK_PUERTO						PTCD
#define		PULSADOR_OK_MASCARA						PTCD_PTCD1_MASK		// Es para leer solo el pin del pulsador
#define		PULSADOR_PRESIONADO						0

#define		PULSADOR_SIGUIENTE						PULSADOR_OK + 1
#define  	PULSADOR_SIGUIENTE_DIRECCION			PTCDD_PTCDD0
#define  	PULSADOR_SIGUIENTE_PIN					PTCD_PTCD0
#define  	PULSADOR_SIGUIENTE_PUERTO				PTCD
#define		PULSADOR_SIGUIENTE_MASCARA				PTCD_PTCD0_MASK		// Es para leer solo el pin del pulsador


#define  	TEST_POINT_PIN_DIRECCION				PTBDD_PTBDD2
#define  	TEST_POINT_PIN							PTBD_PTBD2


// Defines para los canales del ADC
#define		ADC_CANAL_VALOR_MEDIO					10
#define		ADC_CANAL_VALOR_PP6						11


// Defines para la comunicacion
#define 	PTE_485_DIRECCION					PTFDD_PTFDD5
#define 	PTE_485								PTFD_PTFD5		// Para seleccionar si se habilita la recepcion o la transmicion
#define		TRANSMISOR							1
#define		RECEPTOR							0


// Defines para el timer
#define		HABILITAR_INTERRUPCION_Y_ARRANCAR	0x4A
#define		DESHABILITAR_INTERRUPCION_Y_FRENAR	0x00



// Las demoras de menos de 1mSeg se hacen bloqueantes
#define 	DEMORA_200US				TPM2MOD = 0x00FF;	TPM2CNT = 0;	TPM2SC |= 0x0A;		while( TPM2CNT < 0x00FF )	{};
#define		CUARENTA_MILI_SEGUNDOS		40000
#define		ACTIVADO					1
#define		DESACTIVADO					0
#define		PARTE_ALTA					TRUE
#define		PARTE_BAJA					FALSE
#define 	RENGLON_SUPERIOR			0
#define 	RENGLON_INFERIOR			1


/* Defines para las instrucciones de los LCD */

// Instrucciones para borrar la pantalla
#define 	INSTRUCCION_LCD_BORRAR_PANTALLA		0x01

// Instrucciones para regresar el cursor al inicio
#define 	INSTRUCCION_LCD_REGRESAR_CURSOR		0x20

// Instrucciones para seleccionar el modo
#define 	INSTRUCCION_LCD_MODO				0x04
#define 	INSTRUCCION_LCD_MODO_INCREMENTO		0X02
#define 	INSTRUCCION_LCD_MODO_DECREMENTO		0X00
#define 	INSTRUCCION_LCD_MODO_SHIFT_ON		0X01
#define 	INSTRUCCION_LCD_MODO_SHIFT_OFF		0X00

// Instrucciones para encender o apagar la pantalla
#define 	INSTRUCCION_LCD_PANTALLA			0x08
#define 	INSTRUCCION_LCD_PANTALLA_ON			0x04
#define 	INSTRUCCION_LCD_PANTALLA_OFF		0x00
#define 	INSTRUCCION_LCD_PANTALLA_CURSOR_ON	0x02
#define 	INSTRUCCION_LCD_PANTALLA_CURSOR_OFF	0x00
#define 	INSTRUCCION_LCD_PANTALLA_BLINK_ON	0x01
#define 	INSTRUCCION_LCD_PANTALLA_BLINK_OFF	0x00

// Instrucciones para desplazar el cursor o la pantalla
#define 	INSTRUCCION_LCD_DESPLAZAR			0x10
#define 	INSTRUCCION_LCD_DESPLAZAR_PANTALLA	0x08
#define 	INSTRUCCION_LCD_DESPLAZAR_CURSOR	0x00
#define 	INSTRUCCION_LCD_DESPLAZAR_DERECHA	0x04
#define 	INSTRUCCION_LCD_DESPLAZAR_IZQUIERDA	0x00

// Instrucciones para definir la interfaz
#define 	INSTRUCCION_LCD_INTERFAZ			0x20
#define 	INSTRUCCION_LCD_INTERFAZ_8BITS		0x10
#define 	INSTRUCCION_LCD_INTERFAZ_4BITS		0x00
#define 	INSTRUCCION_LCD_INTERFAZ_2LINEAS	0x08
#define 	INSTRUCCION_LCD_INTERFAZ_1LINEA		0x00
#define 	INSTRUCCION_LCD_INTERFAZ_5X10		0x04
#define 	INSTRUCCION_LCD_INTERFAZ_5X8		0x00

// Instrucciones para definir el renglon sobre el que se escribe
#define 	INSTRUCCION_LCD_RENGLON_SUPERIOR		0x80
#define 	INSTRUCCION_LCD_RENGLON_INFERIOR		INSTRUCCION_LCD_RENGLON_SUPERIOR + 0x40



/* Defines para Display MangoVacio */

// Data direction - Para elegir si son entradas o salidas
#define 	LCD_BACK_LIGHT_DD	PTGDD_PTGDD1 //backlight
#define  	LCD_RS_DD			PTEDD_PTEDD3 //rs
#define 	LCD_E_DD			PTEDD_PTEDD4 //e
#define 	LCD_DB4_DD 			PTEDD_PTEDD5 //db4
#define 	LCD_DB5_DD  		PTEDD_PTEDD6 //db5
#define		LCD_DB6_DD			PTEDD_PTEDD7 //db6
#define 	LCD_DB7_DD			PTGDD_PTGDD0 //db7

// Data - Para escribir o leer los bits de datos
#define 	LCD_BACK_LIGHT		PTGD_PTGD1 //backlight
#define  	LCD_RS				PTED_PTED3		//rs
#define 	LCD_E				PTED_PTED4		//e
#define 	LCD_DB4 			PTED_PTED5		//db4
#define 	LCD_DB5	    		PTED_PTED6		//db5
#define		LCD_DB6				PTED_PTED7		//db6
#define 	LCD_PUERTO			PTED			//es el puerto donde van DB6, DB5 y DB4
#define 	LCD_DB7				PTGD_PTGD0		//db7

// Mascaras para acceder a los bits de datos
#define		BORRAR_DBX_PUERTO_MANGO_VACIO		0x1F		// Para dejar xxx00000 en el PTE que tiene la configuracion DB6-DB5-DB4-xxxxx
#define		MASCARA_DB654_DATO_MANGO_VACIO		0x70		// Para quedarse con 0xxx0000
#define		MASCARA_DB210_DATO_MANGO_VACIO		0x07		// Para quedarse con 00000xxx
#define		MASCARA_DB7_DATO_MANGO_VACIO		0x80		// Para quedarse con x0000000
#define		MASCARA_DB3_DATO_MANGO_VACIO		0x08		// Para quedarse con 0000x000
#define		ROTAR_DB654_MANGO_VACIO				1			// Desaplazar 0xxx0000 a xxx00000
#define		ROTAR_DB210_MANGO_VACIO				5			// Desaplazar 00000xxx a xxx00000


#define		MEDICIONES_A_PROMEDIAR				1000
#define		TENSION_DEL_REGULADOR				502






#define		PLACA_NO_SELECCIONADA				0
#define		PLACA_CONTROL_RF					1
#define		PLACA_CONTROL_UC					2
#define		PLACA_DRIVER_RF						3
#define		PLACA_DRIVER_UC						4
#define		PLACA_CALIBRADA						5


#define		ESTADO_CALIBRACION_INICIAR			0
#define		ESTADO_CALIBRACION_PASO_01			ESTADO_CALIBRACION_INICIAR + 1
#define		ESTADO_CALIBRACION_PASO_02			ESTADO_CALIBRACION_PASO_01 + 1
#define		ESTADO_CALIBRACION_PASO_03			ESTADO_CALIBRACION_PASO_02 + 1
#define		ESTADO_CALIBRACION_PASO_04			ESTADO_CALIBRACION_PASO_03 + 1
#define		ESTADO_CALIBRACION_PASO_05			ESTADO_CALIBRACION_PASO_04 + 1
#define		ESTADO_CALIBRACION_PASO_06			ESTADO_CALIBRACION_PASO_05 + 1
#define		ESTADO_CALIBRACION_PASO_07			ESTADO_CALIBRACION_PASO_06 + 1
#define		ESTADO_CALIBRACION_PASO_08			ESTADO_CALIBRACION_PASO_07 + 1
#define		ESTADO_CALIBRACION_PASO_09			ESTADO_CALIBRACION_PASO_08 + 1
#define		ESTADO_CALIBRACION_PASO_10			ESTADO_CALIBRACION_PASO_09 + 1
#define		ESTADO_CALIBRACION_PASO_11			ESTADO_CALIBRACION_PASO_10 + 1
#define		ESTADO_CALIBRACION_PASO_12			ESTADO_CALIBRACION_PASO_11 + 1
#define		ESTADO_CALIBRACION_PASO_13			ESTADO_CALIBRACION_PASO_12 + 1
#define		ESTADO_CALIBRACION_PASO_14			ESTADO_CALIBRACION_PASO_13 + 1
#define		ESTADO_CALIBRACION_PASO_15			ESTADO_CALIBRACION_PASO_14 + 1
#define		ESTADO_CALIBRACION_PASO_16			ESTADO_CALIBRACION_PASO_15 + 1
#define		ESTADO_CALIBRACION_PASO_17			ESTADO_CALIBRACION_PASO_16 + 1
#define		ESTADO_CALIBRACION_PASO_18			ESTADO_CALIBRACION_PASO_17 + 1
#define		ESTADO_CALIBRACION_PASO_19			ESTADO_CALIBRACION_PASO_18 + 1
#define		ESTADO_CALIBRACION_PASO_20			ESTADO_CALIBRACION_PASO_19 + 1
#define		ESTADO_CALIBRACION_PASO_21			ESTADO_CALIBRACION_PASO_20 + 1
#define		ESTADO_CALIBRACION_PASO_22			ESTADO_CALIBRACION_PASO_21 + 1
#define		ESTADO_CALIBRACION_PASO_23			ESTADO_CALIBRACION_PASO_22 + 1
#define		ESTADO_CALIBRACION_PASO_24			ESTADO_CALIBRACION_PASO_23 + 1
#define		ESTADO_CALIBRACION_PASO_25			ESTADO_CALIBRACION_PASO_24 + 1
#define		ESTADO_CALIBRACION_PASO_26			ESTADO_CALIBRACION_PASO_25 + 1
#define		ESTADO_CALIBRACION_PASO_27			ESTADO_CALIBRACION_PASO_26 + 1
#define		ESTADO_CALIBRACION_PASO_28			ESTADO_CALIBRACION_PASO_27 + 1
#define		ESTADO_CALIBRACION_PASO_29			ESTADO_CALIBRACION_PASO_28 + 1
#define		ESTADO_CALIBRACION_PASO_30			ESTADO_CALIBRACION_PASO_29 + 1
#define		ESTADO_CALIBRACION_PASO_31			ESTADO_CALIBRACION_PASO_30 + 1
#define		ESTADO_CALIBRACION_PASO_32			ESTADO_CALIBRACION_PASO_31 + 1
#define		ESTADO_CALIBRACION_PASO_33			ESTADO_CALIBRACION_PASO_32 + 1
#define		ESTADO_CALIBRACION_PASO_34			ESTADO_CALIBRACION_PASO_33 + 1
#define		ESTADO_CALIBRACION_PASO_35			ESTADO_CALIBRACION_PASO_34 + 1
#define		ESTADO_CALIBRACION_PASO_36			ESTADO_CALIBRACION_PASO_35 + 1
#define		ESTADO_CALIBRACION_PASO_37			ESTADO_CALIBRACION_PASO_36 + 1
#define		ESTADO_CALIBRACION_PASO_38			ESTADO_CALIBRACION_PASO_37 + 1
#define		ESTADO_CALIBRACION_PASO_39			ESTADO_CALIBRACION_PASO_38 + 1
#define		ESTADO_CALIBRACION_PASO_40			ESTADO_CALIBRACION_PASO_39 + 1
#define		ESTADO_CALIBRACION_PASO_41			ESTADO_CALIBRACION_PASO_40 + 1
#define		ESTADO_CALIBRACION_PASO_42			ESTADO_CALIBRACION_PASO_41 + 1
#define		ESTADO_CALIBRACION_PASO_43			ESTADO_CALIBRACION_PASO_42 + 1
#define		ESTADO_CALIBRACION_PASO_44			ESTADO_CALIBRACION_PASO_43 + 1
#define		ESTADO_CALIBRACION_PASO_45			ESTADO_CALIBRACION_PASO_44 + 1
#define		ESTADO_CALIBRACION_PASO_46			ESTADO_CALIBRACION_PASO_45 + 1
#define		ESTADO_CALIBRACION_PASO_47			ESTADO_CALIBRACION_PASO_46 + 1
#define		ESTADO_CALIBRACION_PASO_48			ESTADO_CALIBRACION_PASO_47 + 1
#define		ESTADO_CALIBRACION_PASO_49			ESTADO_CALIBRACION_PASO_48 + 1
#define		ESTADO_CALIBRACION_PASO_50			ESTADO_CALIBRACION_PASO_49 + 1
#define		ESTADO_CALIBRACION_PASO_51			ESTADO_CALIBRACION_PASO_50 + 1
#define		ESTADO_CALIBRACION_PASO_52			ESTADO_CALIBRACION_PASO_51 + 1
#define		ESTADO_CALIBRACION_PASO_53			ESTADO_CALIBRACION_PASO_52 + 1
#define		ESTADO_CALIBRACION_PASO_54			ESTADO_CALIBRACION_PASO_53 + 1
#define		ESTADO_CALIBRACION_PASO_55			ESTADO_CALIBRACION_PASO_54 + 1
#define		ESTADO_CALIBRACION_PASO_56			ESTADO_CALIBRACION_PASO_55 + 1
#define		ESTADO_CALIBRACION_PASO_57			ESTADO_CALIBRACION_PASO_56 + 1
#define		ESTADO_CALIBRACION_PASO_58			ESTADO_CALIBRACION_PASO_57 + 1
#define		ESTADO_CALIBRACION_PASO_59			ESTADO_CALIBRACION_PASO_58 + 1
#define		ESTADO_CALIBRACION_PASO_60			ESTADO_CALIBRACION_PASO_59 + 1
#define		ESTADO_CALIBRACION_PASO_61			ESTADO_CALIBRACION_PASO_60 + 1
#define		ESTADO_CALIBRACION_PASO_62			ESTADO_CALIBRACION_PASO_61 + 1
#define		ESTADO_CALIBRACION_PASO_63			ESTADO_CALIBRACION_PASO_62 + 1
#define		ESTADO_CALIBRACION_PASO_64			ESTADO_CALIBRACION_PASO_63 + 1
#define		ESTADO_CALIBRACION_PASO_65			ESTADO_CALIBRACION_PASO_64 + 1
#define		ESTADO_CALIBRACION_PASO_66			ESTADO_CALIBRACION_PASO_65 + 1
#define		ESTADO_CALIBRACION_PASO_67			ESTADO_CALIBRACION_PASO_66 + 1
#define		ESTADO_CALIBRACION_PASO_68			ESTADO_CALIBRACION_PASO_67 + 1
#define		ESTADO_CALIBRACION_PASO_69			ESTADO_CALIBRACION_PASO_68 + 1
#define		ESTADO_CALIBRACION_PASO_70			ESTADO_CALIBRACION_PASO_69 + 1
#define		ESTADO_CALIBRACION_PASO_71			ESTADO_CALIBRACION_PASO_70 + 1
#define		ESTADO_CALIBRACION_PASO_72			ESTADO_CALIBRACION_PASO_71 + 1
#define		ESTADO_CALIBRACION_PASO_73			ESTADO_CALIBRACION_PASO_72 + 1
#define		ESTADO_CALIBRACION_PASO_74			ESTADO_CALIBRACION_PASO_73 + 1
#define		ESTADO_CALIBRACION_PASO_75			ESTADO_CALIBRACION_PASO_74 + 1
#define		ESTADO_CALIBRACION_PASO_76			ESTADO_CALIBRACION_PASO_75 + 1
#define		ESTADO_CALIBRACION_PASO_77			ESTADO_CALIBRACION_PASO_76 + 1
#define		ESTADO_CALIBRACION_PASO_78			ESTADO_CALIBRACION_PASO_77 + 1
#define		ESTADO_CALIBRACION_PASO_79			ESTADO_CALIBRACION_PASO_78 + 1
#define		ESTADO_CALIBRACION_PASO_80			ESTADO_CALIBRACION_PASO_79 + 1



#define		CONDICION_DATO_NO_RECIBIDO			0
#define		CONDICION_DATO_RECIBIDO_MAL			CONDICION_DATO_NO_RECIBIDO + 1
#define		CONDICION_DATO_RECIBIDO_OK			CONDICION_DATO_RECIBIDO_MAL + 1


#define		CODIGO_RECEPCION_CORRECTA			223
#define		CODIGO_SIGUIENTE_ORDEN				225





	/****************************************************************************************************************/
	/*  												VARIABLES													*/
	/****************************************************************************************************************/

// Se define una variable Flag para usar como sincronizacion con la interrupcion del timer
bool flagTerminoElTimer = FALSE;

// Variables para controlar el ADC
uint16_t				cuentasADC;
bool					semaforoADC;

// Variable para llevar la cuenta de la frecuencia del caudalimetro
unsigned int 			frecuenciaDeLaPlaca = 0;

// Flag para sincronizar con el timer del caudalimetro
bool					finalizoElTimer = FALSE;

// Se define una variable Flag para usar como sincronizacion con la interrupcion de recepcion del puerto serie
bool 					flagRecibido = FALSE;

// Variable para almacenar el dato leido
unsigned char			leido;

// Variable para realizar la comparacion entre lo recibido y lo esperado
unsigned char			comparacion;

// Simple contador para ver que se envie correctamente el dato
char					contador = 0;

// Contador del estado de la calibracion
char					EstadoDeLaCalibracion = ESTADO_CALIBRACION_INICIAR;

// Indicador para mostrar los mensajes
bool					MensajeYaMostrado = FALSE;

// Variable para llevar el control de las acciones enviadas a las placas bajo calibracion, en el caso de comandar acciones desde el calibrador
char					IndicacionDeLaCalibracion;


char					NumeroDeEstadosParaAvanzar;


// Variable para indicar que rama se esta calibrando en la placa de Driver RF
bool					placaDriverRF_SegundaRama = FALSE;










	/****************************************************************************************************************/
	/****************************************************************************************************************/
	/****************************************************************************************************************/
	/*  												PROGRAMA													*/
	/****************************************************************************************************************/
	/****************************************************************************************************************/
	/****************************************************************************************************************/

void main(void)
{
  /* Write your local variable definition here */
	
	bool				SaltarDemora;
	bool				PlacaCalibrada;
	bool				EnviarOrdenPorSerie = FALSE;
	bool				PrimeraPasadaCompleta = FALSE;				// Se usa para esperar a que se muestre el mensaje completo antes de aceptar
	
	
	char				PlacaSeleccionada;
	char				Auxiliar = 0;
	char				DriversRFVerificados = 0;
	
	
	int32_t				periodo;
	int32_t				a;
	int32_t				b;
	int32_t				medicionAuxiliar;
	int32_t				periodoActivo;
	int32_t				duty;
	int32_t				tension;
	int32_t				factorDeCorreccionDelDuty;
	int32_t				indiceMedicion;
	
	
	
	unsigned int 		frecuenciaCon2K = 0;
	unsigned int 		frecuenciaCon100 = 0;
	unsigned int 		frecuenciaPuntoMaximoVapor = 0;
	unsigned int 		frecuenciaPuntoMaximaCavitacion = 0;
	unsigned int 		frecuenciaPuntoMaximaCavitacionMasVapor = 0;
	
	char				auxiliar;
	char				SegundosParaAlternarTextos;
	
	char				mensaje[8];
	
	bool				textoPrincipal = FALSE;
	bool				AlternarTextos;
	     
	
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  
  /* ******************** ******************** CODIGO PROPIO ******************** ******************** */
	
  	// Se inicializa el puerto del LCD
  	inicializarPuertoLCD();
  	
  	// Se definen como entrada los pulsadores
  	PULSADOR_OK_DIRECCION = 0;				// Entrada
  	PULSADOR_SIGUIENTE_DIRECCION = 0;		// Entrada
  	TEST_POINT_PIN_DIRECCION = 1;

  	HabilitarTimerComoGeneral();

	// Se genera una demora de 1 segundo antes de inicializar el LCD, para que se estabilice la tension y no falle
	DemoraBloqueanteUnSegundo();
	
	// Se inicializa el LCD
	inicializarLCD_4bits();
	
	// Se envia la instruccion para borar el LCD
	enviarInstruccion( INSTRUCCION_LCD_BORRAR_PANTALLA );
	
	// Se configura el ADC
	ConfigurarADC();
	
	InicializarComunicacionSerie();
	
	
	// Loop para mostrar las opciones de las placas y calibrarlas
	while( TRUE )
	{
		// Se inicializa el estado de la calibracion
		Auxiliar = 0;
		EscribirMensajeRenglonSuperior( "UC Driv " );
		EscribirMensajeRenglonInferior( "SIG   OK" );
		PlacaSeleccionada = PLACA_DRIVER_UC;
		
		// Loop inicial, para mostrar los nombres de las placas y elegir cual se calibrara
		while( TRUE )
		{
			// Se verifica si se oprimio el boton para seleccionar la siguiente placa
			if( PULSADOR_SIGUIENTE_PIN == PULSADOR_PRESIONADO  )
			{
				// Se cambia la indicacion mostrada en pantalla
				Auxiliar++;
				Auxiliar %= 4;
				switch( Auxiliar )
				{
					case 0:
						// Se indica que se esta en la seccion para calibrar Driver Cavitacion
						EscribirMensajeRenglonSuperior( "UC Driv " );
						PlacaSeleccionada = PLACA_DRIVER_UC;
						break;
						
					case 1:
						// Se indica que se esta en la seccion para calibrar Control Cavitacion
						EscribirMensajeRenglonSuperior( "UC Cont " );
						PlacaSeleccionada = PLACA_CONTROL_UC;
						break;
						
					case 2:
						// Se indica que se esta en la seccion para calibrar Driver RF
						EscribirMensajeRenglonSuperior( "RF Driv " );
						PlacaSeleccionada = PLACA_DRIVER_RF;
						break;
						
					case 3:
						// Se indica que se esta en la seccion para calibrar Control RF
						EscribirMensajeRenglonSuperior( "RF Cont " );
						PlacaSeleccionada = PLACA_CONTROL_RF;
						break;
				}
				EscribirMensajeRenglonInferior( "SIG   OK" );
			}
	
			// Se verifica si se oprimio el boton para calibrar la placa mostrada
			if( PULSADOR_OK_PIN == PULSADOR_PRESIONADO )
			{
				if( PlacaSeleccionada != PLACA_NO_SELECCIONADA )
				{
					// Se reinicia el estado de la calibracion, por las dudas
					EstadoDeLaCalibracion = ESTADO_CALIBRACION_INICIAR;
					
					MensajeYaMostrado = FALSE;
					
					EscribirMensaje( "   OK   ", "        " );
					
					DemoraBloqueanteUnSegundo();
		
					// Se corta el loop inicial
					break;
				}
			}
			
			// Se genera una demora antes de revisar nuevamente el estado de los pulsadores
			DemoraBloqueanteUnSegundo();
		}
	
	
	
		// Se genera un loop perpetuo hasta calibrar la placa seleccionada
		while( PlacaSeleccionada != PLACA_NO_SELECCIONADA )
		{
			// Se realiza la accion correspondiente al estado de la placa elegida
			switch( PlacaSeleccionada )
			{
				// -------------------------------------------------------------------------- //
				// -------------------------------------------------------------------------- //
			
				case PLACA_NO_SELECCIONADA:
					// No se hace nada, pero se contempla el estado, por las dudas
					EscribirMensaje( " -Nada- ", " -Nada- " );
					DemoraBloqueanteUnSegundo();
					break;
					
					
				// -------------------------------------------------------------------------- //
				// -------------------------------------------------------------------------- //
				
				case PLACA_CALIBRADA:
					if( MensajeYaMostrado == FALSE )
					{
						MensajeYaMostrado = TRUE;
						// Se indica que finalizo la calibracion de la placa correctamente
						EscribirMensaje( " - Fin -", "Presiona" );
					}
					else
					{
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							// Se borra el contador
							IndicacionDeLaCalibracion = 0;
							
							// Al finalizar correctamente la calibracion de la placa, se vuelve al menu de seleccion
							PlacaSeleccionada = PLACA_NO_SELECCIONADA;
							
							// Se indica que ya esta OK, y se debe dejar de presionar el boton
							EscribirMensaje( " Soltar ", " boton  " );
							
							// Se genera un loop perpetuo hasta soltar el boton
							while( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								asm( nop );
						}
					}
					break;
					
					
				// -------------------------------------------------------------------------- //
				// -------------------------------------------------------------------------- //
			
				case PLACA_DRIVER_UC:
					// Se continua el estado de la calibracion
					switch( EstadoDeLaCalibracion )
					{
						// -------------------------------------------------------------------------- //
					
						case ESTADO_CALIBRACION_INICIAR:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								// Se indica que se esta en la seccion para calibrar Driver UC
								EscribirMensaje( "Calibrar", "UC Driv " );
								// Se ajusta el factor de correcion para medir bien el Duty en esta terapia
								factorDeCorreccionDelDuty = 60;
								
								PrimeraPasadaCompleta = FALSE;
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 1 );
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
					
						case ESTADO_CALIBRACION_PASO_01:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Aca se atienden todos los estados que solo presentan una informacion y esperan por el boton OK
								switch( IndicacionDeLaCalibracion )
								{
								
									case 0:
										// Se indica que se debe retirar la alimentacion
										EscribirMensaje( "Retirar ", "VCC - M1" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 1:
										// Se indica que se deben conectar las distintas partes del calibrador
										EscribirMensaje( "Conectar", " puntas " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 2:
										// Se indica que se deben conectar las puntas
										EscribirMensaje( "PM1/TP10", " negra  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 3:
										// Se indica donde se conecta cada punta
										EscribirMensaje( "D4 catod", " blanca " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 4:
										// Se indica donde se conecta cada punta
										EscribirMensaje( "  TP8   ", "  roja  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 5:
										// Se indica que hay que verificar que no este el jumper colocado
										EscribirMensaje( "Sacar W1", "jumper  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 6:
										// Se indica que hay que verificar que no haya ninguna resistencia colocada en el puerto M2
										EscribirMensaje( "Retirar ", "R en M2 " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 7:
										// Se indica que se debe verificar que no este conectado el manillar en el puerto de carga
										EscribirMensaje( "Retirar ", "   M3   " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 8:
										// Se indica que se debe colocar la alimentacion
										EscribirMensaje( "Conectar", "VCC - M1" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 9:
										// Se indica que se debe seleccionar la terapia para que funcione bien la placa
										EscribirMensaje( "Selecci.", " HImFU  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 10:
										// Se indican los parametros con los que se debe configurar la terapia
										EscribirMensaje( "Shooting", " 60 seg  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 11:
										// Se indican los parametros con los que se debe configurar la terapia
										EscribirMensaje( " Power  ", "  100%  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 12:
										// Se indican los parametros con los que se debe configurar la terapia
										EscribirMensaje( " ON  70 ", " OFF 30 " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 13:
										// Se indican los parametros con los que se debe configurar la terapia
										EscribirMensaje( "Frequen.", " 60 Hz  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 14: // Salta a "ESTADO_CALIBRACION_PASO_02" -> Verificar 5V
										// Se indica que se debe dar play a la terapia para que funcione bien la placa
										EscribirMensaje( "Iniciar ", "terapia " );
										NumeroDeEstadosParaAvanzar = 1;
										break;
										
									case 15: // Viene luego de "ESTADO_CALIBRACION_PASO_04" - Cuando se verifican los 5V
										// Se indica que hay que rotar el potenciometro PR4 en un sentido, antes de iniciar la calibracion
										EscribirMensaje( "PR4 anti", "horario " );
										NumeroDeEstadosParaAvanzar = 0;
										break;

									case 16: // Salta a "ESTADO_CALIBRACION_PASO_06" -> Ajustar los 2.3V
										EscribirMensaje( "TP6 Pun-", "ta blanc" );
										NumeroDeEstadosParaAvanzar = 3;
										break;
										
									case 17: // Viene luego de "ESTADO_CALIBRACION_PASO_06" - Cuando se ajustan los 2.3V
										// Se indica que se debe retirar la punta de medicion en TP6, ya que no se usa mas
										EscribirMensaje( "Retirar ", "  TP6   " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 18:
										// Se indica que hay que colocar el jumper
										EscribirMensaje( "Poner W1", "jumper  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 19: // Salta a "ESTADO_CALIBRACION_PASO_07" -> Ajustar 3400kHz
										// Se indica que hay que colocar la resistencia de 2K en M2
										EscribirMensaje( "R de 2K ", " en M2  " );
										HabilitarMedicionDeFrecuenciaYPeriodo();
										NumeroDeEstadosParaAvanzar = 6;
										break;
	
									case 20: // Viene luego de "ESTADO_CALIBRACION_PASO_12" - Cuando se ajusta el 50%
											 // Salta a "ESTADO_CALIBRACION_PASO_13" -> Ajustar 2900kHz
										// Se indica que hay que colocar la resistencia de 100 en M2
										EscribirMensaje( "R de 100", " en M2  " );
										NumeroDeEstadosParaAvanzar = 12;
										break;
	
									case 21: // Viene luego de "ESTADO_CALIBRACION_PASO_15" - Cuando se ajustan los 2900kHz
											 // Salta a "ESTADO_CALIBRACION_PASO_16" -> Ajustar 3300kHz
										// Se indica que hay que colocar la resistencia de 2k en M2
										EscribirMensaje( "R de 2k ", " en M2  " );
										NumeroDeEstadosParaAvanzar = 15;
										break;
	
									case 22: // Viene luego de "ESTADO_CALIBRACION_PASO_18" - Cuando se ajustan los 3300kHz
											 // Salta a "ESTADO_CALIBRACION_PASO_19" - Cuando se verifica que haya quedado ajustada la placa
										// Se indica que se va a verificar que haya quedado ajustada la placa
										EscribirMensaje( "Verific.", " ajuste " );
										NumeroDeEstadosParaAvanzar = 18;
										break;
										
									case 23: // Viene luego de "ESTADO_CALIBRACION_PASO_21" - Cuando se no verifico el ajuste de la placa
										// Se indica que la placa quedo correctamente ajustada
										EscribirMensaje( "Ajustar ", "de nuevo" );
										NumeroDeEstadosParaAvanzar = 0;
										IndicacionDeLaCalibracion = 19;		// Al presionar OK, se pasa al estado 20
										break;
										
									case 24: // Viene luego de "ESTADO_CALIBRACION_PASO_21" - Cuando se verifico el ajuste de la placa
										// Se indica que la placa quedo correctamente ajustada
										EscribirMensaje( "Placa   ", "ajustada " );
										NumeroDeEstadosParaAvanzar = 0;
										break;

									case 25: // Viene luego de "ESTADO_CALIBRACION_PASO_21" - Cuando se verifica que haya quedado calibrada
										// Se indica que se debe retirar la resistencia de M2
										EscribirMensaje( "Retirar ", "R en M2 " );
										HabilitarTimerComoGeneral();			// Se lo deshabilit hasta que sea necesario volver a medir
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 26:
										// Se indica que hay que colocar el potenciometro de 2k en M2
										EscribirMensaje( "Potencio", "metro M2" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 27:
										// Se indica que se debe conectar el manillar en el puerto M3
										EscribirMensaje( "Conectar", "   M3   " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 28: // Puede venir luego de "ESTADO_CALIBRACION_PASO_28" - Cuando no estan bien las frecuencias de la pastilla
										// Se indica que se debe limpiar la pastilla
										EscribirMensaje( " Limpiar ", "pastilla" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
	
									case 29: // Salta a "ESTADO_CALIBRACION_PASO_22" - Cuando se busca el punto de maximo vapor
										// Se indica que se debe colocar un poco de agua sobre la pastilla para generar vapor
										EscribirMensaje( "Colocar ", " agua   " );
										auxiliar = 3;
										NumeroDeEstadosParaAvanzar = 21;
										break;
	
									case 30: // Viene luego de "ESTADO_CALIBRACION_PASO_23" - Cuando se encontro el punto de maximo vapor
										// Se indica que se debe limpiar la pastilla, mas que nada para eliminar el agua residual
										EscribirMensaje( " Limpiar ", "pastilla" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 31:
										// Se indica que se debe tener cuidado al limpiar la pastilla, porque se rayan facilmente
										EscribirMensaje( "CUIDADO!", "NO RAYAR" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 32:
										// Se indica que se debe colocar una capa de gel antes de apoyar el vaso
										EscribirMensaje( "Colocar ", "  gel   " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 33: // Salta a "ESTADO_CALIBRACION_PASO_24" - Cuando se busca el punto de maxima cavitacion
										// Se indica que se debe colocar el vaso
										EscribirMensaje( "Colocar ", "el vaso " );
										HabilitarMedicionDeFrecuenciaYPeriodo();
										auxiliar = 3;
										NumeroDeEstadosParaAvanzar = 23;
										break;
	
									case 34: // Viene luego de "ESTADO_CALIBRACION_PASO_32" - Cuando se ajusto la resistecia R28 de control para que de menos de 0,5V
										// Se indica que se deben anotar los datos en la planilla
										EscribirMensaje( " Volcar ", " datos  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 35:
										// Se indica que se debe dar stop a la terapia para que funcione bien la placa
										EscribirMensaje( "Detener ", "terapia " );
										HabilitarTimerComoGeneral();			// Se lo deshabilit hasta que sea necesario volver a medir
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 36:
										// Se indica que se debe retirar la alimentacion
										EscribirMensaje( "Retirar ", "VCC - M1" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 37:
										// Se indica que se deben retirar las puntas, porque no se usan mas
										EscribirMensaje( "Retirar ", " puntas " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 38:
										// Se indica que se debe retirar el jumper
										EscribirMensaje( "Sacar W1", "jumper  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 39:
										// Se indica que se debe armar la resistencia que se coloca en el puerto M2
										EscribirMensaje( " Medir  ", "potencio" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 40:
										// Se indica que se debe armar la resistencia que se coloca en el puerto M2
										EscribirMensaje( " Armar  ", "resisten" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 41:
										// Se indica que se debe colocar la resistencia armada
										EscribirMensaje( "Colocar ", "R en M2 " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 42:
										// Se indica que se debe colocar la chapa en la placa
										EscribirMensaje( "Colocar ", "chapa   " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 43:
										// Se indica que se debe colocar la alimentacion
										EscribirMensaje( "Colocar ", "VCC - M1" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 44:
										// Se indica que se debe retirar la alimentacion
										EscribirMensaje( "Iniciar ", "terapia " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 45:
										// Se indica que se debe dar play a la terapia y ver que la placa funcione bien
										EscribirMensaje( " Probar ", " placa  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 46:
										// Se indica que se deben anotar los datos en la planilla
										EscribirMensaje( " Volcar ", " datos  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 47:
										// Se indica que se debe imprimir y colocar la etiqueta a las placas. Fin de la calibracion
										EscribirMensaje( "Colocar ", "etiqueta" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 48:
										// Se pasa al estado de finalizacion
										MensajeYaMostrado = FALSE;
										PlacaSeleccionada = PLACA_CALIBRADA;
										break;

								}
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									IndicacionDeLaCalibracion++;
									AvanzarPasosEnLaCalibracion( NumeroDeEstadosParaAvanzar );
									if( EnviarOrdenPorSerie == TRUE )
									{
										EnviarOrdenPorSerie = FALSE;
										EnviarOrdenDeAvanzarComunicacionSerie();
									}
								}
							}
							break;
	
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_02:
							// Se indica que se debe verificar que la placa tenga 5V
							EscribirMensaje( " Ver 5V ", "        " );
							EstadoDeLaCalibracion++;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_03:
							// Se miden los 5V hasta que se presione el boton OK y la medicion sea correcta
							
							// Se toman varias muestras y se promedian, para eliminar un poco de ruido
							for( indiceMedicion = 0, medicionAuxiliar = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
							{
								// Se registra la tension de la punta blanca, en este caso, usada para medir los 5V
								( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_PP6 );
								
								// Se acumula el valor medido
								medicionAuxiliar += cuentasADC;
							}
							
							// Se promedian las mediciones
							medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
							
							// Calculo de la tension
							b = medicionAuxiliar;							// Se toma la medicion del ADC
							a = b * TENSION_DEL_REGULADOR;					// Se la pasa a tension
							tension = a / 1023;								// Se lo divide por el maximo de la escala para tener la tension
			
							// Se muestra el valor de tension registrado
							EscribirTension5V( tension );
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( tension >= 490 )
								{
									// En esta condicion, esta bien calibrado y se pasa al siguiente estado
									AvanzarPasosEnLaCalibracion( -2 );
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( " No hay ", "   5V   " );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( " Ver 5V ", "        " );
								}
							}
							
							break;
	
						// -------------------------------------------------------------------------- //
	
						case ESTADO_CALIBRACION_PASO_04:
							// Se indica el valor al que se debe ajustar y donde medirlo
							EscribirMensaje( "TP6=2.3V", "        " );
							// Se inicia el contador
							auxiliar = 2;
							EstadoDeLaCalibracion += 2;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_05:
							// Se alterna la indicacion del valor que debe dar y que preset se debe ajustar
							auxiliar--;
							if( !auxiliar )
							{
								auxiliar = 2;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensajeRenglonSuperior( "TP6=2.3V" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensajeRenglonSuperior( "  PR3   " );
								}
							}
							// Se realiza la medicion
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_06:
							
							// Se toman varias muestras y se promedian, para eliminar un poco de ruido
							for( indiceMedicion = 0, medicionAuxiliar = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
							{
								// Se registra la tension del TP6
								( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_PP6 );
								
								// Se acumula el valor medido
								medicionAuxiliar += cuentasADC;
							}
							
							// Se promedian las mediciones
							medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
							
							// Calculo de la tension
							b = medicionAuxiliar;							// Se toma la medicion del ADC
							a = b * TENSION_DEL_REGULADOR;					// Se la pasa a tension
							tension = a / 1023;								// Se lo divide por el maximo de la escala para tener la tension
			
							// Se muestra el valor de tension registrado
							EscribirTension( tension );
							
							// Se toma por default el camino para volver al paso anterior
							EstadoDeLaCalibracion--;
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( tension >= 228 )
								{
									if( tension <= 232 )
									{
										// En esta condicion, esta bien calibrado y se pasa al siguiente estado
										AvanzarPasosEnLaCalibracion( -4 );
									}
									else
									{
										// Aqui esta por encima del valor necesario
										EscribirMensaje( "  Mal   ", "ajustado" );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
										EscribirMensaje( "TP6>2.3V", "        " );
									}
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( "  Mal   ", "ajustado" );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( "TP6<2.3V", "        " );
								}
							}
							
							break;
							
						// -------------------------------------------------------------------------- //
	
						case ESTADO_CALIBRACION_PASO_07:
							// Se indica el valor al que se debe ajustar y donde medirlo
							EscribirMensaje( "TP8=3400", "        " );
							// Se inicia el contador
							auxiliar = 2;
							EstadoDeLaCalibracion += 2;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_08:
							// Se alterna la indicacion del valor que debe dar y que preset se debe ajustar
							auxiliar--;
							if( !auxiliar )
							{
								auxiliar = 2;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensajeRenglonSuperior( "TP8=3400" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensajeRenglonSuperior( "  PR2   " );
								}
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_09:
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaDeLaPlaca );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirFrecuencia( 0 );
							}
	
							// Se toma por default el camino para volver al paso anterior
							EstadoDeLaCalibracion--;
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( frecuenciaDeLaPlaca >= 3390 )
								{
									if( frecuenciaDeLaPlaca <= 3410 )
									{
										// Se indica que quedo bien calibrado
										AvanzarPasosEnLaCalibracion( 2 );
									}
									else
									{
										// Aqui esta por encima del valor necesario
										EscribirMensaje( "  Mal   ", "ajustado" );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
										EscribirMensaje( "TP8>3400", "        " );
									}
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( "  Mal   ", "ajustado" );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( "TP8<3400", "        " );
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_10:
							// Se indica el valor al que se debe ajustar y donde medirlo
							EscribirMensaje( "TP1=50% ", "        " );
							// Se inicia el contador
							auxiliar = 2;
							EstadoDeLaCalibracion += 2;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_11:
							// Se alterna la indicacion del valor que debe dar y que preset se debe ajustar
							auxiliar--;
							if( !auxiliar )
							{
								auxiliar = 2;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensajeRenglonSuperior( "TP1=50% " );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensajeRenglonSuperior( "  PR1   " );
								}
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_12:
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se borra el contador auxiliar
								medicionAuxiliar = 0;
								
								// Se toman varias muestras y se promedian, para eliminar un poco de ruido
								for( indiceMedicion = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
								{
									// Se registra la tension del TP6
									( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_MEDIO );
									
									// Se acumula el valor medido
									medicionAuxiliar += cuentasADC;
								}
								
								medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
	
								// Calculo del Duty
								b = medicionAuxiliar;							// Se toma la medicion del ADC
								a = b * 1000;									// Se la pasa a porcentual
								duty = a / 1023;								// Se lo divide por el maximo de la escala para tener el porcentaje
								duty += factorDeCorreccionDelDuty;				// Se corrige la desviacion de medicion propia de la placa
									
								// Para cavitacion, se debe mostrar el Duty de la senial
								EscribirDuty( duty );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirDuty( 0 );
							}
							
							// Se toma por default el camino para volver al paso anterior
							EstadoDeLaCalibracion--;
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( duty >= 480 )
								{
									if( duty <= 520 )
									{
										// Se indica que quedo bien calibrado
										AvanzarPasosEnLaCalibracion( -10 );
									}
									else
									{
										// Aqui esta por encima del valor necesario
										EscribirMensaje( "  Mal   ", "ajustado" );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
										EscribirMensaje( "TP1>50% ", "        " );
									}
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( "  Mal   ", "ajustado" );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( "TP1<50% ", "        " );
								}
							}
							break;
							
							
						/* ********************************************************************** */
						/* ********************************************************************** */
						// Bucle perpetuo hasta que queden los siguientes valores sin modificar ningun preset
						// Con R de 2k  en M2 => TP8 = 3300 sin tocar PR2
						// Con R de 100 en M2 => TP8 = 2900 sin tocar PR4
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_13:
							// Se indica el valor al que se debe ajustar y donde medirlo
							EscribirMensaje( "TP8=2900", "        " );
							// Se inicia el contador
							auxiliar = 2;
							EstadoDeLaCalibracion += 2;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_14:
							// Se alterna la indicacion del valor que debe dar y que preset se debe ajustar
							auxiliar--;
							if( !auxiliar )
							{
								auxiliar = 2;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensajeRenglonSuperior( "TP8=2900" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensajeRenglonSuperior( "  PR4   " );
								}
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_15:
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaDeLaPlaca );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirFrecuencia( 0 );
							}
	
							// Por default, se va al paso previo, salvo que se accione el boton
							EstadoDeLaCalibracion--;
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( frecuenciaDeLaPlaca >= 2890 )
								{
									if( frecuenciaDeLaPlaca <= 2910 )
									{
										// Se indica que quedo bien calibrado
										AvanzarPasosEnLaCalibracion( -13 );
									}
									else
									{
										// Aqui esta por encima del valor necesario
										EscribirMensaje( "  Mal   ", "ajustado" );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
										EscribirMensaje( "TP8>2900", "        " );
									}
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( "  Mal   ", "ajustado" );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( "TP8<2900", "        " );
								}
							}
							break;
	
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_16:
							// Se indica el valor al que se debe ajustar y donde medirlo
							EscribirMensaje( "TP8=3300", "        " );
							// Se inicia el contador
							auxiliar = 2;
							EstadoDeLaCalibracion += 2;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_17:
							// Se alterna la indicacion del valor que debe dar y que preset se debe ajustar
							auxiliar--;
							if( !auxiliar )
							{
								auxiliar = 2;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensajeRenglonSuperior( "TP8=3300" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensajeRenglonSuperior( "  PR2   " );
								}
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_18:
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaDeLaPlaca );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirFrecuencia( 0 );
							}
	
							// Por default, se va al paso previo, salvo que se accione el boton
							EstadoDeLaCalibracion--;
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( frecuenciaDeLaPlaca >= 3290 )
								{
									if( frecuenciaDeLaPlaca <= 3310 )
									{
										// Se indica que quedo bien calibrado
										AvanzarPasosEnLaCalibracion( -16 );
									}
									else
									{
										// Aqui esta por encima del valor necesario
										EscribirMensaje( "  Mal   ", "ajustado" );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
										EscribirMensaje( "TP8>3300", "        " );
									}
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( "  Mal   ", "ajustado" );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( "TP8<3300", "        " );
								}
							}
							break;
	
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_19:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								// Se indica que hay que colocar la resistencia de 100 en M2
								EscribirMensaje( "R de 100", " en M2  " );
							}
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Se toma el valor de la frecuencia con la resistencia de 100
								frecuenciaCon100 = frecuenciaDeLaPlaca;
								
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaCon100 );
								
								// Se genera una demora para observar el resultado en pantalla
								DemoraBloqueanteUnSegundo();
								
								EstadoDeLaCalibracion++;
								
								if( frecuenciaCon100 <= 2910 )
								{
									if( frecuenciaCon100 >= 2890 )
										EscribirMensaje( "  BIEN  ", "        " );
									else
										EscribirMensaje( "  MAL   ", "        " );
								}
								else
									EscribirMensaje( "  MAL   ", "        " );
								
								// Se genera una demora para observar el resultado en pantalla
								DemoraBloqueanteUnSegundo();
								
								MensajeYaMostrado = FALSE;
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_20:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								// Se indica que hay que colocar la resistencia de 2k en M2
								EscribirMensaje( "R de 2k ", " en M2  " );
							}
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Se toma el valor de la frecuencia con la resistencia de 2k
								frecuenciaCon2K = frecuenciaDeLaPlaca;
								
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaCon2K );
								
								// Se genera una demora para observar el resultado en pantalla
								DemoraBloqueanteUnSegundo();
								
								EstadoDeLaCalibracion++;
								
								if( frecuenciaCon2K >= 3290 )
								{
									if( frecuenciaCon2K <= 3310 )
										EscribirMensaje( "  BIEN  ", "        " );
									else
										EscribirMensaje( "  MAL   ", "        " );
								}
								else
									EscribirMensaje( "  MAL   ", "        " );
								
								// Se genera una demora para observar el resultado en pantalla
								DemoraBloqueanteUnSegundo();
								
								MensajeYaMostrado = FALSE;
							}
							break;
						
						// -------------------------------------------------------------------------- //
	
						case ESTADO_CALIBRACION_PASO_21:
							// Se revisa que este bien ajustado. Sino, se vuelve a los pasos previos
							PlacaCalibrada = FALSE;
							if( frecuenciaCon2K >= 3290 )
							{
								if( frecuenciaCon2K <= 3310 )
								{
									if( frecuenciaCon100 >= 2890 )
									{
										if( frecuenciaCon100 <= 2910 )
										{
											PlacaCalibrada = TRUE;
										}
									}
								}
							}
							
							if( PlacaCalibrada == TRUE )
							{
								// Si esta bien ajustado, se indica que esta bien y se puede continuar
								IndicacionDeLaCalibracion = 24;
								EstadoDeLaCalibracion -= 20;
								MensajeYaMostrado = FALSE;
							}
							else
							{
								// Si no estan bien ajustados, se indica que se debe repetir el procedimiento
								// Se vuelve al paso de colocar la resistencia de 2K y ajustar de nuevo las frecuencias
								IndicacionDeLaCalibracion = 23;
								EstadoDeLaCalibracion -= 20;
								MensajeYaMostrado = FALSE;
							}
							break;
	
						// -------------------------------------------------------------------------- //
								
						case ESTADO_CALIBRACION_PASO_22:
							// Se indica que se debe ajustar el potenciometro hasta lograr el punto de maximo vapor
							auxiliar++;
							auxiliar %= 4;
							switch( auxiliar )
							{
								case 0:
									EscribirMensajeRenglonSuperior( " Buscar " );
									break;
								case 1:
									EscribirMensajeRenglonSuperior( " punto  " );
									break;
								case 2:
									EscribirMensajeRenglonSuperior( " maximo " );
									break;
								case 3:
									EscribirMensajeRenglonSuperior( " vapor  " );
									PrimeraPasadaCompleta = TRUE;
									break;
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
								
						case ESTADO_CALIBRACION_PASO_23:
							
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaDeLaPlaca );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirFrecuencia( 0 );
							}
	
							EstadoDeLaCalibracion--;
							
							// Solo se puede dar OK y continuar, despues de que se haya mostrado el mensaje completo
							if( PrimeraPasadaCompleta == TRUE )
							{
								// Si se presiono el pulsador, se continua, salvo que este mal calibrado
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									PrimeraPasadaCompleta = FALSE;
									frecuenciaPuntoMaximoVapor = frecuenciaDeLaPlaca;
									AvanzarPasosEnLaCalibracion( -21 );
								}
							}
							break;
	
						// -------------------------------------------------------------------------- //
								
						case ESTADO_CALIBRACION_PASO_24:
							// Se indica que se debe buscar el punto de maxima cavitacion
							auxiliar++;
							auxiliar %= 4;
							switch( auxiliar )
							{
								case 0:
									EscribirMensajeRenglonSuperior( " Buscar " );
									break;
								case 1:
									EscribirMensajeRenglonSuperior( " punto  " );
									break;
								case 2:
									EscribirMensajeRenglonSuperior( " maxima " );
									break;
								case 3:
									EscribirMensajeRenglonSuperior( "cavitaci" );
									PrimeraPasadaCompleta = TRUE;
									break;
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
								
						case ESTADO_CALIBRACION_PASO_25:
							
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaDeLaPlaca );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirFrecuencia( 0 );
							}
							
							// Se demora hasta completar un segundo
							DemoraBloqueanteUnSegundo();
							
							EstadoDeLaCalibracion--;
							
							// Solo se puede dar OK y continuar, despues de que se haya mostrado el mensaje completo
							if( PrimeraPasadaCompleta == TRUE )
							{
								// Si se presiono el pulsador, se continua, salvo que este mal calibrado
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									PrimeraPasadaCompleta = FALSE;
									frecuenciaPuntoMaximaCavitacion = frecuenciaDeLaPlaca;
									AvanzarPasosEnLaCalibracion( 2 );
									auxiliar = 5;
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_26:
							// Se indica que se debe buscar el punto de maxima cavitacion, donde tambien debe generar vapor
							auxiliar++;
							auxiliar %= 6;
							switch( auxiliar )
							{
								case 0:
									EscribirMensajeRenglonSuperior( " Buscar " );
									break;
								case 1:
									EscribirMensajeRenglonSuperior( " punto  " );
									break;
								case 2:
									EscribirMensajeRenglonSuperior( " maxima " );
									break;
								case 3:
									EscribirMensajeRenglonSuperior( "cavitaci" );
									break;
								case 4:
									EscribirMensajeRenglonSuperior( "  mas   " );
									break;
								case 5:
									EscribirMensajeRenglonSuperior( " vapor  " );
									PrimeraPasadaCompleta = TRUE;
									break;
							}
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
								
						case ESTADO_CALIBRACION_PASO_27:
							
							// Se toma un valor de base, para no mostrar nada cuando se mide ruido
							if( frecuenciaDeLaPlaca > 100 )
							{
								// Se muestra el valor de la frecuencia medida
								EscribirFrecuencia( frecuenciaDeLaPlaca );
							}
							else
							{
								// Si no se midio un valor de frecuencia, se muestra cero
								EscribirFrecuencia( 0 );
							}
							
							EstadoDeLaCalibracion--;
						
							// Solo se puede dar OK y continuar, despues de que se haya mostrado el mensaje completo
							if( PrimeraPasadaCompleta == TRUE )
							{
								// Si se presiono el pulsador, se continua, salvo que este mal calibrado
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									PrimeraPasadaCompleta = FALSE;
									frecuenciaPuntoMaximaCavitacionMasVapor = frecuenciaDeLaPlaca;
									AvanzarPasosEnLaCalibracion( 2 );
								}
							}
							
							break;
							
						// -------------------------------------------------------------------------- //
									
						case ESTADO_CALIBRACION_PASO_28:
							
							// Se pregunta durante 5 segundos, si las condiciones estan bien para terminar la calibracion
							EscribirMensaje( "Aceptar?", " 5 seg  " );
							for( indiceMedicion = 5; indiceMedicion > 0; indiceMedicion-- )
							{
								// Se muestran los segundos restantes
								mensaje[0] = ' ';
								mensaje[1] = indiceMedicion + '0';
								mensaje[2] = ' ';
								mensaje[3] = 's';
								mensaje[4] = 'e';
								mensaje[5] = 'g';
								mensaje[6] = ' ';
								mensaje[7] = ' ';
								( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 8, mensaje );
								
								// Se genera una demora auxiliar para evitar falsas tomas del pulsador
								DemoraBloqueanteUnSegundo();
								
								// Si se presiono el pulsador, se continua
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									break;						
								}
							}
							
							// Si el for se rompio antes de la condicion de 5 intentos, se presiono el boton
							if( indiceMedicion > 0 )
								EstadoDeLaCalibracion++;
							else
							{
								// Sino, se vuelve al punto de colocar agua y buscar el maximo de vapor
								EstadoDeLaCalibracion -= 27;
								IndicacionDeLaCalibracion = 28;
							}
							
							break;
							
						// -------------------------------------------------------------------------- //	
							
						case ESTADO_CALIBRACION_PASO_29:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se imprime la indicacion del valor de la frecuencia para el punto de maximo vapor
								EscribirMensaje( "F vapor ", "        " );
								
								// Se muestra el valor de frecuencia para el punto de maximo vapor
								EscribirFrecuencia( frecuenciaPuntoMaximoVapor );
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 3 );
									auxiliar = 5;
								}
								else
								{
									if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
									{
										EstadoDeLaCalibracion++;
										EscribirMensaje( "SIGUIENT", "        " );
										MensajeYaMostrado = FALSE;
									}
								}
							}
							break;
								
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_30:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se imprime la indicacion del valor de la frecuencia para el punto de maxima cavitacion
								EscribirMensaje( "F cavita", "        " );
								
								// Se muestra el valor de frecuencia para el punto de maxima cavitacion
								EscribirFrecuencia( frecuenciaPuntoMaximaCavitacion );
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 2 );
									auxiliar = 5;
								}
								else
								{
									if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
									{
										EstadoDeLaCalibracion++;
										EscribirMensaje( "SIGUIENT", "        " );
										MensajeYaMostrado = FALSE;
									}
								}
							}
							break;
									
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_31:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se imprime la indicacion del valor de la frecuencia para el punto de maxima cavitacion con vapor
								EscribirMensaje( "F ambas ", "        " );
								
								// Se muestra el valor de frecuencia para el punto de maxima cavitacion con vapor
								EscribirFrecuencia( frecuenciaPuntoMaximaCavitacionMasVapor );
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 1 );
									auxiliar = 5;
								}
								else
								{
									if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
									{
										EstadoDeLaCalibracion -= 2;
										EscribirMensaje( "SIGUIENT", "        " );
										MensajeYaMostrado = FALSE;
									}
								}
							}
							break;
	
						// -------------------------------------------------------------------------- //
								
						case ESTADO_CALIBRACION_PASO_32:
							// Se indica que se debe ajustar la resistencia de control para no sobrepasar los 5A de consumo
							auxiliar++;
							auxiliar %= 5;
							switch( auxiliar )
							{
								case 0:
									EscribirMensaje( " Placa  ", "control " );
									break;
								case 1:
									EscribirMensaje( "Ajustar ", "  R28   " );
									break;
								case 2:
									EscribirMensaje( "Distribu", " fuente " );
									break;
								case 3:
									EscribirMensaje( " Medir  ", "   P1   " );
									break;
								case 4:
									EscribirMensaje( "Debe ser", " X<0,5V " );
									PrimeraPasadaCompleta = TRUE;
									break;
							}
							if( PrimeraPasadaCompleta == TRUE )
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									// Al finalizar correctamente la calibracion de la placa, se vuelve al menu de seleccion
									AvanzarPasosEnLaCalibracion( - ( ESTADO_CALIBRACION_PASO_32 - 1 ) );
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
					}
					break;
					
					
// ---------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------- //

					
// ---------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------- //


// ---------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------- //
			
				case PLACA_DRIVER_RF:
					// Se continua el estado de la calibracion
					switch( EstadoDeLaCalibracion )
					{
					// -------------------------------------------------------------------------- //
				
					case ESTADO_CALIBRACION_INICIAR:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							// Se indica que se esta en la seccion para calibrar Driver RF
							EscribirMensaje( "Calibrar", "RF Driv " );
							// Se ajusta el factor de correcion para medir bien el Ancho de pulso en esta terapia
							factorDeCorreccionDelDuty = 5;
							
							PrimeraPasadaCompleta = FALSE;
						}
						else
						{
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								AvanzarPasosEnLaCalibracion( 1 );
							}
						}
						break;
						
					// -------------------------------------------------------------------------- //
				
					case ESTADO_CALIBRACION_PASO_01:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							
							// Aca se atienden todos los estados que solo presentan una informacion y esperan por el boton OK
							switch( IndicacionDeLaCalibracion )
							{

								case 0:
									// Se indica que se debe programar la placa
									EscribirMensaje( "Programa", " SPDRF  " );
									NumeroDeEstadosParaAvanzar = 0;
									break;

								case 1:
									// Se indica que se deben conectar las puntas
									EscribirMensaje( "Conectar", " puntas " );
									NumeroDeEstadosParaAvanzar = 0;
									break;

								case 2:
									// Se indica donde se conecta cada punta
									EscribirMensaje( "  PP3   ", " negra  " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 3:
									// Se indica donde se conecta cada punta
									EscribirMensaje( "U6 pin 8", " blanca " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 4: // Salta a "ESTADO_CALIBRACION_PASO_02" -> Verificar 5V
									// Se indica que se debe conectar la alimentacion
									EscribirMensaje( "Colocar ", "M1 - VCC" );
									NumeroDeEstadosParaAvanzar = ESTADO_CALIBRACION_PASO_02 - 1;
									break;

								case 5: // Viene luego de "ESTADO_CALIBRACION_PASO_03" - Cuando se verifican los 5V
									// Se indica que se debe retirar la punta para medir 5V, porque no se usa mas
									EscribirMensaje( "Retirar ", "punta 5V" );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 6: // Salta a "ESTADO_CALIBRACION_PASO_04" -> Verificar la comunicacion con la placa
									// Se indica que se debe conectar la comunicacion con el calibrador
									EscribirMensaje( "Conectar", "M5 - Com" );
									NumeroDeEstadosParaAvanzar = ESTADO_CALIBRACION_PASO_04 - 1;
									break;

								case 7: // Viene luego de "ESTADO_CALIBRACION_PASO_05" - Cuando se verifica la comunicacion
									// Se indica donde se conecta cada punta
									EscribirMensaje( "Jumpers ", "A=x  B=x" );
									placaDriverRF_SegundaRama = FALSE;
									HabilitarMedicionDeFrecuenciaYPeriodo();
									NumeroDeEstadosParaAvanzar = 0;
									break;

								case 8: // Salta a "ESTADO_CALIBRACION_PASO_06" -> Ajustar los 445ns en la rama 1
									// Se indica que se deben colocar los jumpers
									if( placaDriverRF_SegundaRama == FALSE )
										EscribirMensaje( "  PP2   ", "  roja  " );
									else
										EscribirMensaje( "  PP1   ", "  roja  " );
									NumeroDeEstadosParaAvanzar = ESTADO_CALIBRACION_PASO_06 - 1;
									break;

								case 9: // Viene luego de "ESTADO_CALIBRACION_PASO_08" - Cuando se ajustaron los 445ns en la rama 1
									// Se indica que se debe colocar el punto luego de ajustar el preset
									EscribirMensaje( "Colocar ", " punto  " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 10:
									// Se indica que se debe colocar el tester en la escala de 200V para poder medir las salidas
									EscribirMensaje( " Tester ", "200V ---" );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 11:
									// Se indica que se deben conectar la resistencia de carga en la salida
									if( placaDriverRF_SegundaRama == FALSE )
										EscribirMensaje( "Conectar", "   M3   " );
									else
										EscribirMensaje( "Conectar", "   M2   " );
									NumeroDeEstadosParaAvanzar = 0;
									break;

								case 12: // Salta a "ESTADO_CALIBRACION_PASO_09" -> Consultar si la placa es de 1T en la primer pasada
									// Se indica el rango de valores dentro de los cuales se acepta como buena a la salida de la placa
									EscribirMensaje( "Aceptado", "38<XX<45" );
									if( placaDriverRF_SegundaRama == FALSE )
										NumeroDeEstadosParaAvanzar = ESTADO_CALIBRACION_PASO_09 - 1;
									else
										NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 13: // Viene luego de "ESTADO_CALIBRACION_PASO_08" - Cuando se ajustaron los 445ns en la rama 1
									// Se indica que se debe retirar la alimentacion
									EscribirMensaje( "Retirar ", "M1 - VCC" );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 14:
									// Se indica que se deben retirar las puntas
									EscribirMensaje( "Retirar ", " puntas " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 15:
									// Se indica que se debe retirar el jumper
									EscribirMensaje( "Retirar ", "   W1   " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 16:
									// Se indica que se debe definir la letra de la placa
									EscribirMensaje( "Definir ", " placa  " );
									if( placaDriverRF_SegundaRama == TRUE )						// Es una placa de 2 ramas
										NumeroDeEstadosParaAvanzar = ESTADO_CALIBRACION_PASO_10 - 1;
									else
										NumeroDeEstadosParaAvanzar = 0;
									break;

								case 17:
									// Para la placa C, se debe colocar ambos jumpers
									EscribirMensaje( "Placa C ", "A=X  ---" );
									NumeroDeEstadosParaAvanzar = 0;
									break;

								case 18:
									// Se indica que se deben anotar los datos en la planilla
									EscribirMensaje( " Volcar ", " datos  " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 19:
									// Se indica que se debe reprogramar la placa con el software de produccion
									EscribirMensaje( " Repro- ", " gramar " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 20:
									// Se indica cual es la version actual del software de produccion
									EscribirMensaje( " Driver ", " RF 3.9 " );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 21:
									// Se indica que se debe imprimir y colocar la etiqueta a las placas. Fin de la calibracion
									EscribirMensaje( "Colocar ", "etiqueta" );
									NumeroDeEstadosParaAvanzar = 0;
									break;
									
								case 22:
									// Se pasa al estado de finalizacion
									MensajeYaMostrado = FALSE;
									PlacaSeleccionada = PLACA_CALIBRADA;
									break;
							}
						}
						else
						{
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								IndicacionDeLaCalibracion++;
								AvanzarPasosEnLaCalibracion( NumeroDeEstadosParaAvanzar );
								if( EnviarOrdenPorSerie == TRUE )
								{
									EnviarOrdenPorSerie = FALSE;
									EnviarOrdenDeAvanzarComunicacionSerie();
								}
							}
						}
						break;

					// -------------------------------------------------------------------------- //
						
					case ESTADO_CALIBRACION_PASO_02:
						// Se indica que se debe verificar que la placa tenga 5V
						EscribirMensaje( " Ver 5V ", "        " );
						EstadoDeLaCalibracion++;
						break;
						
					// -------------------------------------------------------------------------- //
						
					case ESTADO_CALIBRACION_PASO_03:
						// Se miden los 5V hasta que se presione el boton OK y la medicion sea correcta
						
						// Se toman varias muestras y se promedian, para eliminar un poco de ruido
						for( indiceMedicion = 0, medicionAuxiliar = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
						{
							// Se registra la tension de la punta blanca, en este caso, usada para medir los 5V
							( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_PP6 );
							
							// Se acumula el valor medido
							medicionAuxiliar += cuentasADC;
						}
						
						// Se promedian las mediciones
						medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
						
						// Calculo de la tension
						b = medicionAuxiliar;							// Se toma la medicion del ADC
						a = b * TENSION_DEL_REGULADOR;					// Se la pasa a tension
						tension = a / 1023;								// Se lo divide por el maximo de la escala para tener la tension
		
						// Se muestra el valor de tension registrado
						EscribirTension5V( tension );
						
						// Si se presiono el pulsador, se continua, salvo que este mal calibrado
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							// Al presionar el boton, se verifica que este bien calibrado para seguir
							if( tension >= 490 )
							{
								// En esta condicion, esta bien calibrado y se pasa al siguiente estado
								AvanzarPasosEnLaCalibracion( - ( ESTADO_CALIBRACION_PASO_03 - 1 ) );
							}
							else
							{
								// Aqui esta por debajo del valor necesario
								EscribirMensaje( " No hay ", "   5V   " );
								// Se genera una demora para observar el resultado en pantalla
								DemoraBloqueanteUnSegundo();
								EscribirMensaje( " Ver 5V ", "        " );
							}
						}
						
						break;
						
					// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_04:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							// Se indica que se debe presionar OK para probar la comunicacion
							EscribirMensaje( "OK-Enviar", "        " );
						}
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							EstadoDeLaCalibracion++;
							MensajeYaMostrado = FALSE;
							EscribirMensaje( "        ", "        " );
							SaltarDemora = TRUE;
						}
						break;
						
					// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_05:

						// Se borra el flag de recepcion anterior
						flagRecibido = FALSE;

						// Se envia un dato
						EnviarDatoComunicacionSerie();

						// Se muestra en el display el dato enviado, para poder comparar lo recibido desde la otra placa
						EscribirDatoEnviado( contador );

						// Se genera una demora para esperar la recepcion del dato desde la otra placa
						DemoraBloqueanteUnSegundo();

						// Se verifica la recepcion del dato
						if( flagRecibido == FALSE )
						{
							// No se recibio nada
							EscribirDatoRecibido( 0, CONDICION_DATO_NO_RECIBIDO );
							// Al no verificar la comunicacion, se vuelve al paso previo
							EstadoDeLaCalibracion--;
						}
						else
						{
							// Si se recibio un dato, se verifica que sea correcto
							comparacion = 250 - leido;
							if( comparacion == contador )
							{
								// Bien
								EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_OK );
								// Se genera una pequenia demora para poder ver el mensaje en pantalla
								DemoraBloqueanteUnSegundo();
								// Se envia la confirmacion hacia la placa de haber recibido correctamente el dato
								EnviarConfirmacionComunicacionSerie();
								// Al verificar la comunicacion, se avanza al siguiente paso
								AvanzarPasosEnLaCalibracion( - ( ESTADO_CALIBRACION_PASO_05 - 1 ) );
								DriversRFVerificados = 0;
								EscribirMensaje( "Comunica", "verifica" );
							}
							else
							{
								// Se recibio algo, pero no es lo correcto
								EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_MAL );
								// Al no verificar la comunicacion, se vuelve al paso previo
								EstadoDeLaCalibracion--;
							}
						}
						
						// Se genera una pequenia demora para poder ver el mensaje en pantalla
						DemoraBloqueanteRestoParaUnSegundo();

						// Se marca el flag para indicar que se debe reimprimir el mensaje inicial del proximo estado
						MensajeYaMostrado = FALSE;
						
						break;
						
					// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_06:
						// Se indica el valor al que se debe ajustar y donde medirlo
						if( placaDriverRF_SegundaRama == TRUE )
							EscribirMensaje( "PP1=445n", "        " );
						else
							EscribirMensaje( "PP2=445n", "        " );
						EstadoDeLaCalibracion += 2;
						auxiliar = 3;
						SaltarDemora = TRUE;
						break;
						
					// -------------------------------------------------------------------------- //
						
					case ESTADO_CALIBRACION_PASO_07:
						// Se alterna la indicacion del valor que debe dar y que preset se debe ajustar
						auxiliar--;
						if( !auxiliar )
						{
							auxiliar = 3;
							// Cada tanto se cambia la indicacion en pantalla
							if( textoPrincipal == TRUE )
							{
								textoPrincipal = FALSE;
								// Se indica el valor al que se debe ajustar y donde medirlo
								if( placaDriverRF_SegundaRama == TRUE )
									EscribirMensajeRenglonSuperior( "PP1=445n" );
								else
									EscribirMensajeRenglonSuperior( "PP2=445n" );
							}
							else
							{
								textoPrincipal = TRUE;
								// Se indica el preset que se debe ajustar
								if( placaDriverRF_SegundaRama == TRUE )
									EscribirMensajeRenglonSuperior( "   P2   " );
								else
									EscribirMensajeRenglonSuperior( "   P1   " );
							}
						}
						EstadoDeLaCalibracion++;
						SaltarDemora = TRUE;
						break;
						
					// -------------------------------------------------------------------------- //
						
					case ESTADO_CALIBRACION_PASO_08:
						// Se toma un valor de base, para no mostrar nada cuando se mide ruido
						if( frecuenciaDeLaPlaca > 100 )
						{
							// Se borra el contador auxiliar
							medicionAuxiliar = 0;
							
							// Se toman varias muestras y se promedian, para eliminar un poco de ruido
							for( indiceMedicion = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
							{
								// Se registra la tension del TP6
								( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_MEDIO );
								
								// Se acumula el valor medido
								medicionAuxiliar += cuentasADC;
							}
							
							medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
							
							// Calculo del Periodo
							b = medicionAuxiliar;							// Se toma la medicion del ADC
							a = b * 1000;									// Se la pasa a porcentual
							duty = a / 1023;								// Se lo divide por el maximo de la escala para tener el porcentaje
							duty += factorDeCorreccionDelDuty;				// Se corrige la desviacion de medicion propia de la placa
								
							// Para las placas de RF, se debe mostrar el ancho del pulso
							periodo = 1000000 / frecuenciaDeLaPlaca;
							a = periodo * duty;
							a /= 1000;
							periodoActivo = a;
							EscribirAnchoDePulso( periodoActivo );
						}
						else
						{
							// Si no se midio un valor de frecuencia, se muestra cero
							EscribirAnchoDePulso( 0 );
							DemoraBloqueanteUnSegundo();
						}
						
						EstadoDeLaCalibracion--;
						
						// Si se presiono el pulsador, se continua, salvo que este mal calibrado
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							// Al presionar el boton, se verifica que este bien calibrado para seguir
							if( periodoActivo >= 440 )
							{
								if( periodoActivo <= 450 )
								{
									// En esta condicion, esta bien calibrado y se pasa al siguiente estado
									// Si esta bien ajustado, se indica que esta bien y se puede continuar
									IndicacionDeLaCalibracion = 9;
									EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
									EscribirMensaje( "   OK   ", "        " );
									MensajeYaMostrado = FALSE;
									DemoraBloqueanteUnSegundo();
								}
								else
								{
									// Aqui esta por encima del valor necesario
									EscribirMensaje( "  Mal   ", "ajustado" );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									if( placaDriverRF_SegundaRama == TRUE )
										EscribirMensaje( "PP1>445n", "        " );
									else
										EscribirMensaje( "PP2>445n", "        " );
									DemoraBloqueanteUnSegundo();
								}
							}
							else
							{
								// Aqui esta por debajo del valor necesario
								EscribirMensaje( "  Mal   ", "ajustado" );
								// Se genera una demora para observar el resultado en pantalla
								DemoraBloqueanteUnSegundo();
								if( placaDriverRF_SegundaRama == TRUE )
									EscribirMensaje( "PP1<445n", "        " );
								else
									EscribirMensaje( "PP2<445n", "        " );
								DemoraBloqueanteUnSegundo();
							}
						}
						break;

						// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_09:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							// Si la placa es de 1 trafo, solo es necesario calibrar una rama
							EscribirMensaje( "1T - Sig", "2T -  OK" );
						}
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							placaDriverRF_SegundaRama = TRUE;
							IndicacionDeLaCalibracion = 8;
							EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
							EscribirMensaje( "   2T   ", "        " );
							MensajeYaMostrado = FALSE;
						}
						if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
						{
//								HabilitarTimerComoGeneral();
							IndicacionDeLaCalibracion = 13;
							EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
							EscribirMensaje( "   1T   ", "        " );
							MensajeYaMostrado = FALSE;
						}
						break;

					// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_10:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							// Para la placa A, se deben colocar ambos jumpers
							EscribirMensaje( "Placa A ", "A=X  B=X" );
						}
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							IndicacionDeLaCalibracion = 18;
							EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
							EscribirMensaje( "   OK   ", "        " );
							MensajeYaMostrado = FALSE;
						}
						if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
						{
							EstadoDeLaCalibracion++;
							MensajeYaMostrado = FALSE;
						}
						break;
						
					// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_11:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							// Para la placa B, se debe colocar el jumper en B
							EscribirMensaje( "Placa B ", "---  B=X" );
						}
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							IndicacionDeLaCalibracion = 18;
							EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
							EscribirMensaje( "   OK   ", "        " );
							MensajeYaMostrado = FALSE;
						}
						if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
						{
							EstadoDeLaCalibracion++;
							MensajeYaMostrado = FALSE;
						}
						break;

					// -------------------------------------------------------------------------- //

					case ESTADO_CALIBRACION_PASO_12:
						if( MensajeYaMostrado == FALSE )
						{
							MensajeYaMostrado = TRUE;
							// Para la placa D, se deben retirar ambos jumpers
							EscribirMensaje( "Placa D ", "---  ---" );
						}
						if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
						{
							IndicacionDeLaCalibracion = 18;
							AvanzarPasosEnLaCalibracion( - ( ESTADO_CALIBRACION_PASO_12 - 1 ) );
						}
						if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
						{
							EstadoDeLaCalibracion -= 2;
							MensajeYaMostrado = FALSE;
						}
						break;
						
						// -------------------------------------------------------------------------- //
							
						// -------------------------------------------------------------------------- //
						// -------------------------------------------------------------------------- //
						// -------------------------------------------------------------------------- //
					}
					break;
					
					
					
					
// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //
				
				case PLACA_CONTROL_RF:
					// Se continua el estado de la calibracion
					switch( EstadoDeLaCalibracion )
					{
						
						// -------------------------------------------------------------------------- //	
							
						case ESTADO_CALIBRACION_INICIAR:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								// Se indica que se esta en la seccion para calibrar Control RF
								EscribirMensaje( "Calibrar", "RF Cont " );
							}
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Se inicializan todas las variables
								IndicacionDeLaCalibracion = 0;
								EnviarOrdenPorSerie = FALSE;
								AlternarTextos = FALSE;
								SegundosParaAlternarTextos = 3;
								PlacaCalibrada = FALSE;
								
								// Se avanza al primer paso de la calibracion
								AvanzarPasosEnLaCalibracion( 1 );
							}
							break;
								
						// -------------------------------------------------------------------------- //
					
						case ESTADO_CALIBRACION_PASO_01:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Aca se atienden todos los estados que solo presentan una informacion y esperan por el boton OK
								switch( IndicacionDeLaCalibracion )
								{
									case 0:
										// Se indica que se debe programar la placa de control con un software especial
										EscribirMensaje( "Programa", "SPCRFcM " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 1:
										// Se indica que se debe retirar la alimentacion
										EscribirMensaje( "Retirar ", "M9 - VCC" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 2:
										// Se indica que se deben conectar las distintas partes del calibrador
										EscribirMensaje( "Conectar", "probador" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 3:
										// Se indica que se deben conectar las puntas
										EscribirMensaje( "Conectar", " puntas " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 4:
										// Se indica donde se conecta cada punta
										EscribirMensaje( "U1 pin 4", " negra  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 5:
										// Se indica donde se conecta cada punta
										EscribirMensaje( "U1 pin 8", " blanca " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 6: // Salta a "ESTADO_CALIBRACION_PASO_02" -> Verificar 5V
										// Se indica que se debe retirar la alimentacion
										EscribirMensaje( "Conectar", "M9 - VCC" );
										NumeroDeEstadosParaAvanzar = 1;
										break;
										
									case 7: // Viene luego de "ESTADO_CALIBRACION_PASO_03" - Cuando se verifican los 5V
										// Se indica que se debe retirar la punta para medir 5V, porque no se usa mas
										EscribirMensaje( "Retirar ", " puntas " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 8: // Salta a "ESTADO_CALIBRACION_PASO_04" - Verificar la comunicacion con la CPU
										// Se indica que se debe conectar la comunicacion con el calibrador
										EscribirMensaje( "Conectar", "P3 - Com" );
										NumeroDeEstadosParaAvanzar = 3;
										break;
										
									case 9: // Viene luego de "ESTADO_CALIBRACION_PASO_08" - Cuando se verifica la comunicacion con las 4 drivers
										// Se indica el puerto que se va a activar, para estar atento al led correspondiente
										EscribirMensaje( "Activar ", "  M11   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 10:
										// Se indica el puerto que se va a activar, para estar atento al led correspondiente
										EscribirMensaje( "Activar ", "   M5   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 11:
										// Se indica el puerto que se va a activar, para estar atento al led correspondiente
										EscribirMensaje( "Activar ", "   M4   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 12:
										// Se indica el puerto que se va a activar, para estar atento al led correspondiente
										EscribirMensaje( "Activar ", "   M2   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 13:
										// Se indica el puerto que se va a desactivar, para estar atento al led correspondiente
										EscribirMensaje( "Desactiv", "  M11   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 14:
										// Se indica el puerto que se va a desactivar, para estar atento al led correspondiente
										EscribirMensaje( "Desactiv", "   M5   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 15:
										// Se indica el puerto que se va a desactivar, para estar atento al led correspondiente
										EscribirMensaje( "Desactiv", "   M4   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 16:
										// Se indica el puerto que se va a desactivar, para estar atento al led correspondiente
										EscribirMensaje( "Desactiv", "   M2   " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
										
									// Registrar el sensor de temperatura
									case 17:
										// Se indica que se va a registrar el sensor de temperatura
										EscribirMensaje( " Medir  ", "temperat" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 18:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( "Colocar ", " jumper " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 19:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( "Led M11 ", "encendid" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 20:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( "Retirar ", " jumper " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 21:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( "Led M11 ", "apagado " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 22:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( "Colocar ", " jumper " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 23:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( "Led M11 ", "destello" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
										
									// Registrar el sensor de presion, mediante el preset
									case 24:
										// Se indica que se va a registrar el sensor de temperatura
										EscribirMensaje( " Medir  ", "presion " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 25:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( " Preset ", "horario " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 26:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( " Led M2 ", "apagado " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 27:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( " Preset ", "centrado" );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 28:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( " Led M2 ", "encendid" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 29:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( " Preset ", "anti hor" );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 30:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( " Led M2 ", "apagado " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 31:
										// Se indica que se debe colocar el jumper para ver que mida bien el sensor de temperatura
										EscribirMensaje( " Preset ", "centrado" );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 32:
										// Se indica que se debe haber encendido el led en M11 si esta bien
										EscribirMensaje( " Led M2 ", "destello" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
										
									// Calibrar la fuente de RF
									case 33:
										// Se indica que se va a calibrar el preset para la fuente RF
										EscribirMensaje( "Ajustar ", " fuente " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 34:
										// Se indica que se debe colocar el tester en la escala de 200mV
										EscribirMensaje( " Tester ", "200m ---" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 35: // Salta a "ESTADO_CALIBRACION_PASO_09" - Calibrar la fuente de RF
										// Se indica que se debe conectar el tester
										EscribirMensaje( "Conectar", " tester " );
										EnviarOrdenPorSerie = TRUE;
										NumeroDeEstadosParaAvanzar = 8;
										auxiliar = 3;
										break;
										
										
									// La placa esta calibrada. Restan acciones administrativas
									case 36: // Vuelve de "ESTADO_CALIBRACION_PASO_12" - Cuando se calibra la fuente de RF
										// Se indica que se debe colocar el punto para marcar la placa como calibrada
										EscribirMensaje( "Colocar ", " punto  " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 37:
										// Se indica que se debe colocar la alimentacion
										EscribirMensaje( "Retirar ", "M9 - VCC" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 38:
										// Se indica que se debe colocar la alimentacion
										EscribirMensaje( "Retirar ", "puntas " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 39:
										// Se indica que se debe reprogramar la placa con el software de produccion
										EscribirMensaje( " Repro- ", " gramar " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 40:
										// Se indica cual es la version actual del software de produccion
										EscribirMensaje( "Control ", "RF 4.60 " );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 41:
										// Se indica que se debe imprimir y colocar la etiqueta a las placas. Fin de la calibracion
										EscribirMensaje( "Colocar ", "etiqueta" );
										NumeroDeEstadosParaAvanzar = 0;
										break;
										
									case 42:
										// Se pasa al estado de finalizacion
										MensajeYaMostrado = FALSE;
										PlacaSeleccionada = PLACA_CALIBRADA;
										break;

								}
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									IndicacionDeLaCalibracion++;
									AvanzarPasosEnLaCalibracion( NumeroDeEstadosParaAvanzar );
									if( EnviarOrdenPorSerie == TRUE )
									{
										EnviarOrdenPorSerie = FALSE;
										EnviarOrdenDeAvanzarComunicacionSerie();
									}
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_02:
							// Se indica que se debe verificar que la placa tenga 5V
							EscribirMensaje( " Ver 5V ", "        " );
							EstadoDeLaCalibracion++;
							SaltarDemora = TRUE;
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_03:
							// Se miden los 5V hasta que se presione el boton OK y la medicion sea correcta
							
							// Se toman varias muestras y se promedian, para eliminar un poco de ruido
							for( indiceMedicion = 0, medicionAuxiliar = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
							{
								// Se registra la tension del TP6
								( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_PP6 );
								
								// Se acumula el valor medido
								medicionAuxiliar += cuentasADC;
							}
							
							// Se promedian las mediciones
							medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
							
							// Calculo de la tension
							b = medicionAuxiliar;							// Se toma la medicion del ADC
							a = b * TENSION_DEL_REGULADOR;					// Se la pasa a tension
							tension = a / 1023;								// Se lo divide por el maximo de la escala para tener la tension
			
							// Se muestra el valor de tension registrado
							EscribirTension5V( tension );
							
							// Si se presiono el pulsador, se continua, salvo que este mal calibrado
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								// Al presionar el boton, se verifica que este bien calibrado para seguir
								if( tension >= 490 )
								{
									// En esta condicion, esta bien calibrado y se pasa al siguiente estado
									AvanzarPasosEnLaCalibracion( -2 );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
								}
								else
								{
									// Aqui esta por debajo del valor necesario
									EscribirMensaje( " No hay ", "   5V   " );
									// Se genera una demora para observar el resultado en pantalla
									DemoraBloqueanteUnSegundo();
									EscribirMensaje( " Ver 5V ", "        " );
								}
							}
							
							break;
							
						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_04:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								// Se indica que se debe presionar OK para probar la comunicacion
								EscribirMensaje( "OK-Enviar", "        " );
							}
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								EstadoDeLaCalibracion++;
								MensajeYaMostrado = FALSE;
								EscribirMensaje( "        ", "        " );
								SaltarDemora = TRUE;
							}
							break;
							
						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_05:

							// Se borra el flag de recepcion anterior
							flagRecibido = FALSE;

							// Se envia un dato
							EnviarDatoComunicacionSerie();

							// Se muestra en el display el dato enviado, para poder comparar lo recibido desde la otra placa
							EscribirDatoEnviado( contador );

							// Se genera una demora para esperar la recepcion del dato desde la otra placa
							DemoraBloqueanteUnSegundo();

							// Se verifica la recepcion del dato
							if( flagRecibido == FALSE )
							{
								// No se recibio nada
								EscribirDatoRecibido( 0, CONDICION_DATO_NO_RECIBIDO );
								// Al no verificar la comunicacion, se vuelve al paso previo
								EstadoDeLaCalibracion--;
							}
							else
							{
								// Si se recibio un dato, se verifica que sea correcto
								comparacion = 250 - leido;
								if( comparacion == contador )
								{
									// Bien
									EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_OK );
									// Se genera una pequenia demora para poder ver el mensaje en pantalla
									DemoraBloqueanteUnSegundo();
									// Se envia la confirmacion hacia la placa de haber recibido correctamente el dato
									EnviarConfirmacionComunicacionSerie();
									// Al verificar la comunicacion, se avanza al siguiente paso
									EstadoDeLaCalibracion++;
									DriversRFVerificados = 0;
									EscribirMensaje( "   CPU  ", "verifica" );
								}
								else
								{
									// Se recibio algo, pero no es lo correcto
									EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_MAL );
									// Al no verificar la comunicacion, se vuelve al paso previo
									EstadoDeLaCalibracion--;
								}
							}
							
							// Se genera una pequenia demora para poder ver el mensaje en pantalla
							DemoraBloqueanteRestoParaUnSegundo();

							// Se marca el flag para indicar que se debe reimprimir el mensaje inicial del proximo estado
							MensajeYaMostrado = FALSE;
							
							break;
							
						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_06:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se indica que se debe conectar la comunicacion con el calibrador
								switch( DriversRFVerificados )
								{
									case 0:
										EscribirMensaje( "Conectar", "P4A- Com" );
										break;
									case 1:
										EscribirMensaje( "Conectar", "P4B- Com" );
										break;
									case 2:
										EscribirMensaje( "Conectar", "P4C- Com" );
										break;
									case 3:
										EscribirMensaje( "Conectar", "P4D- Com" );
										break;
								}
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 1 );
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_07:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								// Se indica que se debe presionar OK para probar la comunicacion
								EscribirMensaje( "OK-Enviar", "        " );
							}
							if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
							{
								EstadoDeLaCalibracion++;
								MensajeYaMostrado = FALSE;
								EscribirMensaje( "        ", "        " );
								SaltarDemora = TRUE;
							}
							break;
							
						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_08:

							// Se borra el flag de recepcion anterior
							flagRecibido = FALSE;

							// Se envia un dato
							EnviarDatoComunicacionSerie();

							// Se muestra en el display el dato enviado, para poder comparar lo recibido desde la otra placa
							EscribirDatoEnviado( contador );

							// Se genera una demora para esperar la recepcion del dato desde la otra placa
							DemoraBloqueanteUnSegundo();

							// Se verifica la recepcion del dato
							if( flagRecibido == FALSE )
							{
								// No se recibio nada
								EscribirDatoRecibido( 0, CONDICION_DATO_NO_RECIBIDO );
								// Al no verificar la comunicacion, se vuelve al paso previo
								EstadoDeLaCalibracion--;
							}
							else
							{
								// Si se recibio un dato, se verifica que sea correcto
								comparacion = 250 - leido;
								if( comparacion == contador )
								{
									// Bien
									EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_OK );
									// Se genera una pequenia demora para poder ver el mensaje en pantalla
									DemoraBloqueanteUnSegundo();
									// Se envia la confirmacion hacia la placa de haber recibido correctamente el dato
									EnviarConfirmacionComunicacionSerie();
									// Al verificar la comunicacion, se pasa al siguiente Driver
									EstadoDeLaCalibracion -= 2;
									EscribirMensaje( "   OK   ", "        " );
									// Se continua el loop hasta verificar la comunicacion con los 4 puertos
									DriversRFVerificados++;
									if( DriversRFVerificados == 4 )
									{
										EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
										EscribirMensaje( "Drivers ", "verifica" );
										MensajeYaMostrado = FALSE;
										DemoraBloqueanteUnSegundo();
									}
								}
								else
								{
									// Se recibio algo, pero no es lo correcto
									EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_MAL );
									// Al no verificar la comunicacion, se vuelve al paso previo
									EstadoDeLaCalibracion--;
								}
							}
							
							DemoraBloqueanteRestoParaUnSegundo();
							
							break;

						// -------------------------------------------------------------------------- //

						case ESTADO_CALIBRACION_PASO_09:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se indica que se debe ajustar el preset R11 hasta lograr que el tester marque -5,7mV
								AlternarTextos = TRUE;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensaje( "Ajustar ", "  R11   " );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensaje( " Medir  ", " -5.7mV " );
								}
								EnviarOrdenPorSerie = TRUE;
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 1 );
									EnviarOrdenDeAvanzarComunicacionSerie();
									SegundosParaAlternarTextos = 3;
									textoPrincipal = FALSE;
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_10:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se indica que se debe ajustar el preset R11 hasta lograr que el tester marque -5,7mV
								AlternarTextos = TRUE;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensaje( "Toleranc", "+/-0.2mV" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensaje( " Medir  ", "  0.2mV " );
								}
								EnviarOrdenPorSerie = TRUE;
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 1 );
									EnviarOrdenDeAvanzarComunicacionSerie();
									SegundosParaAlternarTextos = 3;
									textoPrincipal = FALSE;
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_11:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se indica que se debe ajustar el preset R11 hasta lograr que el tester marque -5,7mV
								AlternarTextos = TRUE;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensaje( "Toleranc", "+/-0.2mV" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensaje( " Medir  ", " -2.7mV " );
								}
								EnviarOrdenPorSerie = TRUE;
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									AvanzarPasosEnLaCalibracion( 1 );
									EnviarOrdenDeAvanzarComunicacionSerie();
									SegundosParaAlternarTextos = 3;
									textoPrincipal = FALSE;
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
							
						case ESTADO_CALIBRACION_PASO_12:
							if( MensajeYaMostrado == FALSE )
							{
								MensajeYaMostrado = TRUE;
								
								// Se indica que se debe ajustar el preset R11 hasta lograr que el tester marque -5,7mV
								AlternarTextos = TRUE;
								// Cada tanto se cambia la indicacion en pantalla
								if( textoPrincipal == TRUE )
								{
									textoPrincipal = FALSE;
									// Se indica el valor al que se debe ajustar y donde medirlo
									EscribirMensaje( "Toleranc", "+/-0.2mV" );
								}
								else
								{
									textoPrincipal = TRUE;
									// Se indica el preset que se debe ajustar
									EscribirMensaje( " Medir  ", "  3.0mV " );
								}
								EnviarOrdenPorSerie = TRUE;
							}
							else
							{
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									// Se considera que la primera vez no queda calibrada, solo para que se repasen los valores, por las dudas
									if( PlacaCalibrada == FALSE )
									{
										// Se marca que ya se calibro y se vuelve al paso de revisar los -5.7mV
										PlacaCalibrada = TRUE;
										AvanzarPasosEnLaCalibracion( -3 );										
									}
									else
									{
										AvanzarPasosEnLaCalibracion( - ( ESTADO_CALIBRACION_PASO_12 - 1 ) );
									}
									EnviarOrdenDeAvanzarComunicacionSerie();
									SegundosParaAlternarTextos = 3;
									textoPrincipal = FALSE;
								}
							}
							break;
							
						// -------------------------------------------------------------------------- //
								
					}
					break;

// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //
	
	
// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //

	
// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //
					
					case PLACA_CONTROL_UC:
						// Se continua el estado de la calibracion
						switch( EstadoDeLaCalibracion )
						{
							
							// -------------------------------------------------------------------------- //	
								
							case ESTADO_CALIBRACION_INICIAR:
								if( MensajeYaMostrado == FALSE )
								{
									MensajeYaMostrado = TRUE;
									// Se indica que se esta en la seccion para calibrar Control RF
									EscribirMensaje( "Calibrar", "UC Cont " );
								}
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									// Se inicializan todas las variables
									IndicacionDeLaCalibracion = 0;
									EnviarOrdenPorSerie = FALSE;
									AlternarTextos = FALSE;
									SegundosParaAlternarTextos = 3;
									PlacaCalibrada = FALSE;
									
									// Se avanza al primer paso de la calibracion
									AvanzarPasosEnLaCalibracion( 1 );
								}
								break;
								
							// -------------------------------------------------------------------------- //
						
							case ESTADO_CALIBRACION_PASO_01:
								if( MensajeYaMostrado == FALSE )
								{
									MensajeYaMostrado = TRUE;
									
									// Aca se atienden todos los estados que solo presentan una informacion y esperan por el boton OK
									switch( IndicacionDeLaCalibracion )
									{
										case 0:
											// Se indica que se debe programar la placa de control con un software especial
											EscribirMensaje( "Programa", " SPCUC   " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 1:
											// Se indica que se debe retirar la alimentacion
											EscribirMensaje( "Retirar ", "M3 - VCC" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 2:
											// Se indica que se debe retirar la alimentacion hacia la placa Driver
											EscribirMensaje( "Retirar ", "M8 - Drv" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 3:
											// Se indica que se deben conectar las puntas
											EscribirMensaje( "Conectar", " puntas " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 4:
											// Se indica donde se conecta cada punta
											EscribirMensaje( "  PP6   ", " negra  " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 5:
											// Se indica donde se conecta cada punta
											EscribirMensaje( "  PP7   ", " blanca " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 6: // Salta a "ESTADO_CALIBRACION_PASO_02" -> Verificar 5V
											// Se indica que se debe retirar la alimentacion
											EscribirMensaje( "Conectar", "M9 - VCC" );
											NumeroDeEstadosParaAvanzar = 1;
											break;
											
										case 7: // Viene luego de "ESTADO_CALIBRACION_PASO_03" - Cuando se verifican los 5V
											// Se indica que se debe retirar la punta para medir 5V, porque no se usa mas
											EscribirMensaje( "Retirar ", " puntas " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 8: // Salta a "ESTADO_CALIBRACION_PASO_04" - Verificar la comunicacion
											// Se indica que se debe conectar la comunicacion con el calibrador
											EscribirMensaje( "Conectar", "M1 - Com" );
											NumeroDeEstadosParaAvanzar = 3;
											break;
											
										case 9: // Viene luego de "ESTADO_CALIBRACION_PASO_05" - Cuando se verifica la comunicacion con la placa
											// Se indica el puerto que se va a activar, para estar atento al led correspondiente
											EscribirMensaje( "Activar ", " Cooler " );
											EnviarOrdenPorSerie = TRUE;
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 10:
											// Se indica el puerto que se va a desactivar, para estar atento al led correspondiente
											EscribirMensaje( "Desactiv", " Cooler " );
											EnviarOrdenPorSerie = TRUE;
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 11:
											// Se indica que se debe conectar el manillar para probar los leds y el pin en corto del conector
											EscribirMensaje( "Conectar", "leds -M6" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 12:
											// Se indica que se debe conectar el manillar para probar los leds y el pin en corto del conector
											EscribirMensaje( "Conectar", "Jump -M5" );
											NumeroDeEstadosParaAvanzar = 0;
											EnviarOrdenPorSerie = TRUE;
											break;
											
										case 13:
											// Se indica que se debe haber encendido el led en M6 si esta bien
											EscribirMensaje( " Led M6 ", "encendid" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 14:
											// Se indica que se debe conectar el manillar para probar los leds y el pin en corto del conector
											EscribirMensaje( "Retirar ", "Jump -M5" );
											NumeroDeEstadosParaAvanzar = 0;
											EnviarOrdenPorSerie = TRUE;
											break;
											
										case 15:
											// Se indica que se debe haber encendido el led en M6 si esta bien
											EscribirMensaje( " Led M6 ", "apagado " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 16:
											// Se indica que se debe colocar el tester en la escala de 200V de continua
											EscribirMensaje(  " Tester ", "200V ---" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 17:
											// Se indica que se debe conectar el tester entre los pines 1 y 3
											EscribirMensaje(  " Cables ", "Rojo-Neg" );
											NumeroDeEstadosParaAvanzar = 0;
											EnviarOrdenPorSerie = TRUE;
											break;
											
										case 18:
											// Se indica que se debe medir 12V, que es la tension de control
											EscribirMensaje(  " Medir  ", "  12V   " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 19: // Salta a "ESTADO_CALIBRACION_PASO_06" - Saber si tiene el punto o no, para ver hacia que lado se debe girar
											// Se indica que se debe girar el preset hacia un extremo
											EscribirMensaje(  "Ver R28 ", " punto  " );
											NumeroDeEstadosParaAvanzar = 5;
											break;
											
										case 20: // Viene de "ESTADO_CALIBRACION_PASO_06" - Se gira el preset para obtener la minima tension 
											if( textoPrincipal == TRUE )
											{
												// En este caso, se debe girar en sentido horario para obtener el minimo
												EscribirMensaje( " Girar  ", "Horario " );
											}
											else
											{
												// En este caso, se debe girar en sentido anti horario para obtener el minimo
												EscribirMensaje( " Girar  ", "Antihora" );
											}
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 21:
											// Se indica que se debe conectar el tester entre los pines 3 y 5
											EscribirMensaje(  " Cables ", "Blan-Neg" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 22:
											// Se indica que se debe medir casi 8V, que es la tension de driver al minimo
											EscribirMensaje( "Aceptado", " 6<XX<8 " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 23:
											// Se indica que se debe medir casi 8V, que es la tension de driver al minimo
											EscribirMensaje( "Invertir", "  R28   " );
											NumeroDeEstadosParaAvanzar = 0;
											EnviarOrdenPorSerie = TRUE;
											break;

										case 24:
											// Se indica que se debe medir casi entre 24V y 40V
											EscribirMensaje( "Aceptado", "24<XX<42" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 25:
											// Se indica que se debe dejar al minimo de tension, por las dudas
											EscribirMensaje( "Invertir", "  R28   " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 26:
											// Se indica que se debe retirar la alimentacion
											EscribirMensaje( "Retirar ", "M3 - VCC" );
											NumeroDeEstadosParaAvanzar = 0;
											break;

										case 27:
											// Se indica que se debe reprogramar la placa con el software de produccion
											EscribirMensaje( " Repro- ", " gramar " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 28:
											// Se indica cual es la version actual del software de produccion
											EscribirMensaje( "Control ", " UC 1.7 " );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 29:
											// Se indica que se debe imprimir y colocar la etiqueta a las placas. Fin de la calibracion
											EscribirMensaje( "Colocar ", "etiqueta" );
											NumeroDeEstadosParaAvanzar = 0;
											break;
											
										case 30:
											// Se pasa al estado de finalizacion
											MensajeYaMostrado = FALSE;
											PlacaSeleccionada = PLACA_CALIBRADA;
											break;
	
									}
								}
								else
								{
									if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
									{
										IndicacionDeLaCalibracion++;
										AvanzarPasosEnLaCalibracion( NumeroDeEstadosParaAvanzar );
										if( EnviarOrdenPorSerie == TRUE )
										{
											EnviarOrdenPorSerie = FALSE;
											EnviarOrdenDeAvanzarComunicacionSerie();
										}
									}
								}
								break;
								
							// -------------------------------------------------------------------------- //
	
							case ESTADO_CALIBRACION_PASO_02:
								// Se indica que se debe verificar que la placa tenga 5V
								EscribirMensaje( " Ver 5V ", "        " );
								EstadoDeLaCalibracion++;
								SaltarDemora = TRUE;
								break;
								
							// -------------------------------------------------------------------------- //
								
							case ESTADO_CALIBRACION_PASO_03:
								// Se miden los 5V hasta que se presione el boton OK y la medicion sea correcta
								
								// Se toman varias muestras y se promedian, para eliminar un poco de ruido
								for( indiceMedicion = 0, medicionAuxiliar = 0; indiceMedicion < MEDICIONES_A_PROMEDIAR; indiceMedicion++ )
								{
									// Se registra la tension del TP6
									( void ) TomarMuestraSimpleADC( ADC_CANAL_VALOR_PP6 );
									
									// Se acumula el valor medido
									medicionAuxiliar += cuentasADC;
								}
								
								// Se promedian las mediciones
								medicionAuxiliar /= MEDICIONES_A_PROMEDIAR;
								
								// Calculo de la tension
								b = medicionAuxiliar;							// Se toma la medicion del ADC
								a = b * TENSION_DEL_REGULADOR;					// Se la pasa a tension
								tension = a / 1023;								// Se lo divide por el maximo de la escala para tener la tension
				
								// Se muestra el valor de tension registrado
								EscribirTension5V( tension );
								
								// Si se presiono el pulsador, se continua, salvo que este mal calibrado
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									// Al presionar el boton, se verifica que este bien calibrado para seguir
									if( tension >= 490 )
									{
										// En esta condicion, esta bien calibrado y se pasa al siguiente estado
										AvanzarPasosEnLaCalibracion( -2 );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
									}
									else
									{
										// Aqui esta por debajo del valor necesario
										EscribirMensaje( " No hay ", "   5V   " );
										// Se genera una demora para observar el resultado en pantalla
										DemoraBloqueanteUnSegundo();
										EscribirMensaje( " Ver 5V ", "        " );
									}
								}
								
								break;
								
							// -------------------------------------------------------------------------- //
	
							case ESTADO_CALIBRACION_PASO_04:
								if( MensajeYaMostrado == FALSE )
								{
									MensajeYaMostrado = TRUE;
									// Se indica que se debe presionar OK para probar la comunicacion
									EscribirMensaje( "OK-Enviar", "        " );
								}
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									EstadoDeLaCalibracion++;
									MensajeYaMostrado = FALSE;
									EscribirMensaje( "        ", "        " );
									SaltarDemora = TRUE;
								}
								break;
								
							// -------------------------------------------------------------------------- //
	
							case ESTADO_CALIBRACION_PASO_05:
	
								// Se borra el flag de recepcion anterior
								flagRecibido = FALSE;
	
								// Se envia un dato
								EnviarDatoComunicacionSerie();
	
								// Se muestra en el display el dato enviado, para poder comparar lo recibido desde la otra placa
								EscribirDatoEnviado( contador );
	
								// Se genera una demora para esperar la recepcion del dato desde la otra placa
								DemoraBloqueanteUnSegundo();
	
								// Se verifica la recepcion del dato
								if( flagRecibido == FALSE )
								{
									// No se recibio nada
									EscribirDatoRecibido( 0, CONDICION_DATO_NO_RECIBIDO );
									// Al no verificar la comunicacion, se vuelve al paso previo
									EstadoDeLaCalibracion--;
								}
								else
								{
									// Si se recibio un dato, se verifica que sea correcto
									comparacion = 250 - leido;
									if( comparacion == contador )
									{
										// Bien
										EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_OK );
										// Se genera una pequenia demora para poder ver el mensaje en pantalla
										DemoraBloqueanteUnSegundo();
										// Se envia la confirmacion hacia la placa de haber recibido correctamente el dato
										EnviarConfirmacionComunicacionSerie();
										// Al verificar la comunicacion, se avanza al siguiente paso
										AvanzarPasosEnLaCalibracion( -4 );
										DriversRFVerificados = 0;
										EscribirMensaje( "Comunica", "verifica" );
									}
									else
									{
										// Se recibio algo, pero no es lo correcto
										EscribirDatoRecibido( comparacion, CONDICION_DATO_RECIBIDO_MAL );
										// Al no verificar la comunicacion, se vuelve al paso previo
										EstadoDeLaCalibracion--;
									}
								}
								
								// Se genera una pequenia demora para poder ver el mensaje en pantalla
								DemoraBloqueanteRestoParaUnSegundo();
	
								// Se marca el flag para indicar que se debe reimprimir el mensaje inicial del proximo estado
								MensajeYaMostrado = FALSE;
								
								break;
								
							// -------------------------------------------------------------------------- //
								
							case ESTADO_CALIBRACION_PASO_06:
								if( MensajeYaMostrado == FALSE )
								{
									MensajeYaMostrado = TRUE;
									// Se indica que se debe presionar OK para probar la comunicacion
									EscribirMensaje(  "OK  = SI", "SIG = NO" );
								}
								if( RevisarOpresionDelPulsador( PULSADOR_OK ) == TRUE )
								{
									EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
									textoPrincipal = TRUE;
									MensajeYaMostrado = FALSE;
								}
								if( RevisarOpresionDelPulsador( PULSADOR_SIGUIENTE ) == TRUE )
								{
									EstadoDeLaCalibracion = ESTADO_CALIBRACION_PASO_01;
									textoPrincipal = FALSE;
									MensajeYaMostrado = FALSE;
								}
								break;
								
							// -------------------------------------------------------------------------- //
								
						}
						break;
							
// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// -------------------------------------------------------------------------- //
						
					
			}
			
			
			
			
			// En algunos pasos no se requiere la demora hacia la siguiente indicacion
			if( SaltarDemora == TRUE )
			{
				// Se borra el flag por las dudas
				SaltarDemora = FALSE;
			}
			else
			{
				// Se genera una demora antes de volver al paso de la calibracion y esperar por los pulsadores
				DemoraBloqueanteRestoParaUnSegundo();
				
				// Si esta activado el flag para cambiar de texto, se lleva un conteo de 3 segundos
				if( AlternarTextos == TRUE )
				{
					SegundosParaAlternarTextos--;
					if( !SegundosParaAlternarTextos )
					{
						SegundosParaAlternarTextos = 3;
						MensajeYaMostrado = FALSE;
					}
				}
			}
		}
	}
	
  
  /* ******************** ******************** CODIGO PROPIO ******************** ******************** */
  
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */








/* *** FUNCIONES PARA EL TIMER *** */

void DemoraBloqueanteUnSegundo ( void )
{
	// Se resetea el contador
	TPM3CNT = 0;
	
	// Se borra el flag
	finalizoElTimer = FALSE;
	
	// Se genera una demora antes de volver al paso de la calibracion y esperar por los pulsadores
	while( finalizoElTimer == FALSE )		{	asm( nop );		}

	// Se borra el flag
	finalizoElTimer = FALSE;
}




void DemoraBloqueanteRestoParaUnSegundo ( void )
{
	// Se borra el flag
	finalizoElTimer = FALSE;
	
	// Se genera una demora antes de volver al paso de la calibracion y esperar por los pulsadores
	while( finalizoElTimer == FALSE )		{	asm( nop );		}

	// Se borra el flag
	finalizoElTimer = FALSE;
}


















/****************************************************************************************************************/
/*				  								FUNCIONES PARA EL ADC											*/
/****************************************************************************************************************/


/****************************************************************/
/* ConfigurarADC												*/
/*  															*/
/*  Setea el modulo para usarlo en baja velocidad, con el bus	*/
/*  principal dividido 16 (8x2), tiempos largos para la toma de	*/
/*	muestras, disparado por software y 10 bits					*/
/*  															*/
/*  Recibe: Nada												*/
/*  Devuelve: Nada												*/
/****************************************************************/
void ConfigurarADC ( void )
{
	ADC1CFG = 0xF9;		// Baja velocidad, dividido 8, tiempos largos, 10 bits y dividido 2
	ADC1SC2 = 0;		// Disparable por software, sin comparacion
	APCTL1 = 0x00;		// No se usan los canales bajos
	APCTL2 = 0x08;		// Se habilitan los pines para el canal 10 (valor medio) y el canal 11 (PP6)
}



/****************************************************************/
/* TomarMuestraSimpleADC										*/
/*  															*/
/*  Se inicia una conversion simple en el canal seleccionado y  */
/*  se espera por la interrupcion de la conversion.				*/
/*	El resultado se almacena en la variable global "cuentasADC"	*/
/*  															*/
/*  Recibe: El canal del cual se deba leer el valor				*/
/*  Devuelve: FALSE, si hay algun problema						*/
/****************************************************************/
bool TomarMuestraSimpleADC( char canal )
{
	switch( canal )
	{
		case ADC_CANAL_VALOR_MEDIO:
			ADC1SC1 = 0x4A;
			break;
		case ADC_CANAL_VALOR_PP6:
			ADC1SC1 = 0x4B;
			break;
	}
	
	
	// Se borra el flag de la interrupcion
	semaforoADC = FALSE;
	
	// Luego se espera a que termine la conversion
	while( TRUE )
	{
		if( semaforoADC == TRUE )
		{
			semaforoADC = FALSE;
			break;
		}
	}
	
	// Se toma el valor de la conversion
	cuentasADC = leerADC10bits();
	
	return( TRUE );
}



uint16_t leerADC10bits ( void )			{	return( ADC1R );	}


// Funcion para borrar el flag de la interrupcion
void borrarFlagADC ( void )				{	clrReg8Bits( ADC1SC1, 0x80U );	}





/****************************************************************************************************************/
/*				  								FUNCIONES PARA EL LCD											*/
/****************************************************************************************************************/


/************************************************************/
/* inicializarLCD_4bits										*/
/*  														*/
/*  Inicializa el LCD para usar una interfaz de 4 bits.		*/
/*  Se agrega una demora de 120mSeg luego de cada			*/
/*	instruccion para darle tiempo al LCD a procesarlas.		*/
/*  														*/
/*  Recibe: Nada											*/
/*  Devuelve: Nada											*/
/************************************************************/
void inicializarLCD_4bits( void )
{
	// Variable auxiliar para hacer mas legibles los codigos
	char instruccion;
	
	// Demora de 120mSeg para esperar que se inicie el LCD
	demoraInicialLCD();

	// Demora de 120mSeg para esperar que se inicie el LCD
	demoraInicialLCD();
	demoraInicialLCD();
	
	// Se inicializa un bus de 4 bits
	LCD_RS = DESACTIVADO;
	DEMORA_200US;
	LCD_E = DESACTIVADO;
	DEMORA_200US;
	instruccion = INSTRUCCION_LCD_INTERFAZ + INSTRUCCION_LCD_INTERFAZ_4BITS;
	mandarNibbleAltoLCD_Vacio( instruccion );
	DEMORA_200US;
	LCD_E = ACTIVADO;
	DEMORA_200US;
	DEMORA_200US;
	DEMORA_200US;
	LCD_E = DESACTIVADO;
	demoraInicialLCD();
	
	// Se selecciona un bus de 4 bits, la cantidad de lineas y la fuente del LCD
	instruccion = INSTRUCCION_LCD_INTERFAZ + INSTRUCCION_LCD_INTERFAZ_4BITS;
	instruccion += INSTRUCCION_LCD_INTERFAZ_2LINEAS;
	instruccion += INSTRUCCION_LCD_INTERFAZ_5X8;
	enviarInstruccion( instruccion );
	demoraInicialLCD();
	demoraInicialLCD();
	
	// Se enciende la pantalla. Por default se usa el cursor apagado y sin parpadeo
	enviarInstruccion( INSTRUCCION_LCD_PANTALLA + INSTRUCCION_LCD_PANTALLA_ON + INSTRUCCION_LCD_PANTALLA_CURSOR_OFF + INSTRUCCION_LCD_PANTALLA_BLINK_OFF );
	demoraInicialLCD();
	demoraInicialLCD();
	
	// Se selecciona un modo incremental
	enviarInstruccion( INSTRUCCION_LCD_MODO + INSTRUCCION_LCD_MODO_INCREMENTO + INSTRUCCION_LCD_MODO_SHIFT_OFF );
	demoraInicialLCD();
	demoraInicialLCD();
	
	// Se borra la pantalla
	enviarInstruccion( INSTRUCCION_LCD_BORRAR_PANTALLA );
	demoraInicialLCD();
	demoraInicialLCD();

}



/************************************************************/
/* inicializarPuertoLCD										*/
/*  														*/
/*  Define los pines hacia el LCD como salidas y los pone a */
/*  0. Ademas, enciende el backlight para el mango de vacio */
/*  														*/
/*  Recibe: Nada											*/
/*  Devuelve: Nada											*/
/************************************************************/
void inicializarPuertoLCD()
{
	// Se inicializan los puertos como salida
	PTEDD = 0xE0;
	PTGDD = 0x01;
	LCD_RS_DD	= 1;
	LCD_E_DD	= 1;
	
	// Se ponen a 0 todas las salidas, por las dudas.
	LCD_RS	= 0;
	LCD_E	= 0;
	LCD_DB4	= 0;
	LCD_DB5 = 0;
	LCD_DB6	= 0;
	LCD_DB7	= 0;
}



/****************************************************************/
/* demoraInicialLCD												*/
/*  															*/
/*  Demora de 120mSeg para que el LCD procese las ordenes		*/
/*  de inicializacion.											*/
/*  															*/
/*  Recibe: Nada												*/
/*  Devuelve: Nada												*/
/****************************************************************/
void demoraInicialLCD()
{
	// Se borra el flag de sincronizacion
	flagTerminoElTimer = FALSE;
	
	// Se carga el valor de la demora deseada en los registros del timer
	TPM3MOD = CUARENTA_MILI_SEGUNDOS;
	
	// Se resetean los contadores
	TPM3CNT = 0;
	
	// Se habilita el modulo (con el bus interno) y se habilita la interrupcion
	TPM3SC = HABILITAR_INTERRUPCION_Y_ARRANCAR;
	
	// Se entra en modo de espera hasta que desborde el timer
	while( flagTerminoElTimer == FALSE )
		asm(wait);
	
	// Se desactiva el modulo y las interrupciones
	clrReg8Bits(TPM3SC, DESHABILITAR_INTERRUPCION_Y_FRENAR);
	
	
	
	
	
	
	// Se borra el flag de sincronizacion
	flagTerminoElTimer = FALSE;
	
	// Se carga el valor de la demora deseada en los registros del timer
	TPM3MOD = CUARENTA_MILI_SEGUNDOS;
	
	// Se resetean los contadores
	TPM3CNT = 0;
	
	// Se habilita el modulo (con el bus interno) y se habilita la interrupcion
	TPM3SC = HABILITAR_INTERRUPCION_Y_ARRANCAR;
	
	// Se entra en modo de espera hasta que desborde el timer
	while( flagTerminoElTimer == FALSE )
		asm(wait);
	
	// Se desactiva el modulo y las interrupciones
	clrReg8Bits(TPM3SC, DESHABILITAR_INTERRUPCION_Y_FRENAR);
	
	
}



/* ***  	FUNCIONES DE COMUNICACION		  *** */

/****************************************************************/
/* enviarInstruccion											*/
/*  															*/
/*  Se encarga de mandar el dato sobre el bus del puerto.    	*/
/*  Primero se manda la parte alta del dato y luego la baja. 	*/
/*  															*/
/*  Recibe: Nada												*/
/*  Devuelve: Nada												*/
/****************************************************************/
void enviarInstruccion( char dato )
{
	// Primero se deben deshabilitar las lineas de RS y E para poder colocar el dato en el puerto y luego habilitar la linea de E
	LCD_RS = DESACTIVADO;			// Es una instruccion
	LCD_E = DESACTIVADO;			// para que no tome las modificaciones que se van a realizar sobre el puerto de datos
	DEMORA_200US;
	
	// Se escribe la parte alta de los datos sobre el puerto
	mandarNibbleAltoLCD_Vacio( dato );
	LCD_E = ACTIVADO;				// Se habilita la linea de E para que el LCD tome la parte del dato
	DEMORA_200US;
	LCD_E = DESACTIVADO;			// Se deshabilita la linea de E
	
	// Se escribe la parte baja de los datos sobre el puerto
	mandarNibbleBajoLCD_Vacio( dato );
	
	LCD_E = ACTIVADO;				// Se habilita la linea de E para que el LCD tome la parte del dato
	DEMORA_200US;
	LCD_E = DESACTIVADO;			// Se deshabilita la linea de E
	
	// Se contempla una demora final, para darle tiempo al LCD a reponerse para los siguientes comandos
	DEMORA_200US;
}



/****************************************************************/
/* enviarDato													*/
/*  															*/
/*  Se encarga de mandar el dato sobre el bus del puerto.    	*/
/*  Primero se manda la parte alta del dato y luego la baja. 	*/
/*  															*/
/*  Recibe: Nada												*/
/*  Devuelve: Nada												*/
/****************************************************************/
void enviarDato( char dato )
{
	// Primero se deben deshabilitar las lineas de RS y E para poder colocar el dato en el puerto y luego habilitar la linea de E
	LCD_RS = ACTIVADO;				// Es un dato
	LCD_E = DESACTIVADO;			// para que no tome las modificaciones que se van a realizar sobre el puerto de datos
	DEMORA_200US;
	
	// Se escribe la parte alta de los datos sobre el puerto
	mandarNibbleAltoLCD_Vacio( dato );
	LCD_E = ACTIVADO;				// Se habilita la linea de E para que el LCD tome la parte del dato
	DEMORA_200US;
	LCD_E = DESACTIVADO;			// Se deshabilita la linea de E
	
	// Se escribe la parte baja de los datos sobre el puerto
	mandarNibbleBajoLCD_Vacio( dato );
	LCD_E = ACTIVADO;				// Se habilita la linea de E para que el LCD tome la parte del dato
	DEMORA_200US;
	LCD_E = DESACTIVADO;			// Se deshabilita la linea de E
	
	// Se contempla una demora final, para darle tiempo al LCD a reponerse para los siguientes comandos
	DEMORA_200US;
}





/****************************************************************/
/* mandarNibbleAltoLCD_Vacio									*/
/*  															*/
/*  Envia la parte alta del dato utilizando la configuracion de */
/*  pines que tiene el LCD en el mango de vacio.				*/
/*  															*/
/*  Recibe: El dato a enviar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void mandarNibbleAltoLCD_Vacio( char dato )
{
	// Variables auxiliares
	char auxiliar;

	// Se escribe la parte alta de los datos sobre el puerto
	auxiliar = dato & MASCARA_DB654_DATO_MANGO_VACIO;	// Se enmascaran los datos de DB6, DB5 y DB4 
	auxiliar <<= ROTAR_DB654_MANGO_VACIO;				// Se lo desplaza una unidad para que coincidan con los pines PTED7, PTED6 y PTED5
	LCD_PUERTO &= BORRAR_DBX_PUERTO_MANGO_VACIO;		// Se borran los pines del puerto para escribir los datos
	LCD_PUERTO |= auxiliar;								// Se escriben los datos en el puerto. Resta escribir DB7
	auxiliar = dato & MASCARA_DB7_DATO_MANGO_VACIO;
	if( auxiliar > 0 )									// Se escribe el dato DB7
		LCD_DB7 = 1;
	else
		LCD_DB7 = 0;
	
	DEMORA_200US;
}



/****************************************************************/
/* mandarNibbleBajoLCD_Vacio									*/
/*  															*/
/*  Envia la parte baja del dato utilizando la configuracion de */
/*  pines que tiene el LCD en el mango de vacio.				*/
/*  															*/
/*  Recibe: El dato a enviar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void mandarNibbleBajoLCD_Vacio( char dato )
{
	// Variables auxiliares
	char auxiliar;

	// Se escribe la parte baja de los datos sobre el puerto
	auxiliar = dato & MASCARA_DB210_DATO_MANGO_VACIO;	// Se enmascaran los datos de DB2, DB1 y DB0 
	auxiliar <<= ROTAR_DB210_MANGO_VACIO;				// Se lo desplaza cinco unidades para que coincidan con los pines PTED7, PTED6 y PTED5
	LCD_PUERTO &= BORRAR_DBX_PUERTO_MANGO_VACIO;		// Se borran los pines del puerto para escribir los datos
	LCD_PUERTO |= auxiliar;								// Se escriben los datos en el puerto. Resta escribir DB3
	auxiliar = dato & MASCARA_DB3_DATO_MANGO_VACIO;
	if( auxiliar > 0 )									// Se escribe el dato DB7
		LCD_DB7 = 1;
	else
		LCD_DB7 = 0;
	
	DEMORA_200US;
}



/****************************************************************/
/* EscribirMensajeLCD											*/
/*  															*/
/*  Imprime en el LCD el texto suministrado, en el renglon 		*/
/*  solicitado y con el offset requerido.						*/
/*  															*/
/*  Recibe: 													*/
/*	 - renglon: Para indicar el numero del renglon, siendo 1 el */
/*  		superior y numerando en orden cresciente los demas  */
/*   - inicioTexto: En que posicion del renglo empieza el texto */
/*   - totalDeCaracteres: Cuantos se van a imprimir del total	*/
/*  		del texto suministrado								*/
/*   - *pTexto: Un puntero al vector donde se almacena el		*/
/*  		mensaje a mostrar									*/
/*  Devuelve: 													*/
/*	 - True: Todos los parametros estaba bien y no hubo			*/
/*			problemas											*/
/*	 - False: Algun parametro estaba mal						*/
/****************************************************************/
bool EscribirMensajeLCD( unsigned char renglon, unsigned char inicioTexto, unsigned char totalDeCaracteres, unsigned char *pTexto)
{
	// Variables auxiliares
	char caracter = 0;
	char instruccion;
	
	// Se verifica que el parametro no este fuera de rango
	if( totalDeCaracteres == 0 )
		return( FALSE );
	
	// Se manda la orden para posicionarse segun el renglon que se vaya a utilizar
	switch( renglon )
	{
		case RENGLON_SUPERIOR:
			instruccion = INSTRUCCION_LCD_RENGLON_SUPERIOR;
			break;
		case RENGLON_INFERIOR:
			instruccion = INSTRUCCION_LCD_RENGLON_INFERIOR;
			break;
		
		default:
			return( FALSE );
	}
	
	// Se le agrega el inicio del cursor
	if( inicioTexto < 8 )
		instruccion += inicioTexto;
	
	// Se envia la instruccion para posicionar el cursor
	enviarInstruccion( instruccion );
	
	// Se envia el mensaje
	for( caracter = 0; caracter < totalDeCaracteres; caracter++ )
		enviarDato( pTexto[ caracter ] );
	
}



/****************************************************************/
/* EscribirFrecuenciaRenglonSuperior							*/
/*  															*/
/*  Muestra el numero suministrado en el LCD					*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirFrecuenciaRenglonSuperior( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[7];

	// Se toman las unidades
	pTexto[3] = numero % 10;
	pTexto[3] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[2] = numero % 10;
	pTexto[2] += '0';
	
	// Se toman las centenas
	numero /= 10;
	pTexto[1] = numero % 10;
	pTexto[1] += '0';
	
	// Se toman las unidades de mil
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se escribe el indicador
	pTexto[4] = 'k';
	pTexto[5] = 'H';
	pTexto[6] = 'z';
	
	// Se imprime el numero
	( void ) EscribirMensajeLCD( RENGLON_SUPERIOR, 0, 7, pTexto );
}



/****************************************************************/
/* EscribirFrecuencia											*/
/*  															*/
/*  Muestra el numero suministrado en el LCD					*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirFrecuencia( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[7];

	// Se toman las unidades
	pTexto[3] = numero % 10;
	pTexto[3] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[2] = numero % 10;
	pTexto[2] += '0';
	
	// Se toman las centenas
	numero /= 10;
	pTexto[1] = numero % 10;
	pTexto[1] += '0';
	
	// Se toman las unidades de mil
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se escribe el indicador
	pTexto[4] = 'k';
	pTexto[5] = 'H';
	pTexto[6] = 'z';
	
	// Se imprime el numero
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 7, pTexto );
}



/****************************************************************/
/* EscribirDuty													*/
/*  															*/
/*  Muestra el duty de la senial en el LCD						*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirDuty( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[5];
	
	// Se imprime el mensaje para saber si esta bien calibrada o no
	if( numero > 480 )
	{
		if( numero < 520 )
		{
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, " OK" );
		}
		else
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );
	}
	else
	{
		( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );
	}

	// Se escribe el separador
	pTexto[4] = '%';
	pTexto[2] = '.';
	
	// Se toman los decimales
	pTexto[3] = numero % 10;
	pTexto[3] += '0';
	
	// Se toman las unidades
	numero /= 10;
	pTexto[1] = numero % 10;
	pTexto[1] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se imprime el numero
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 5, pTexto );

}



/****************************************************************/
/* EscribirTension												*/
/*  															*/
/*  Muestra la tension de la senial en el LCD					*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirTension5V( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[5];
	
	// Se imprime el mensaje para saber si esta bien calibrada o no
	if( numero > 490 )
	{
		( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, " OK" );
	}
	else
	{
		( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );
	}

	// Se escribe el separador
	pTexto[4] = 'V';
	pTexto[1] = '.';
	
	// Se toman los decimales
	pTexto[3] = numero % 10;
	pTexto[3] += '0';
	
	// Se toman las unidades
	numero /= 10;
	pTexto[2] = numero % 10;
	pTexto[2] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se imprime el numero
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 5, pTexto );

}



/****************************************************************/
/* EscribirTension												*/
/*  															*/
/*  Muestra la tension de la senial en el LCD					*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirTension( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[5];
	
	// Se imprime el mensaje para saber si esta bien calibrada o no
	if( numero > 227 )
	{
		if( numero < 233 )
		{
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, " OK" );
		}
		else
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );
	}
	else
	{
		( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );
	}

	// Se escribe el separador
	pTexto[4] = 'V';
	pTexto[1] = '.';
	
	// Se toman los decimales
	pTexto[3] = numero % 10;
	pTexto[3] += '0';
	
	// Se toman las unidades
	numero /= 10;
	pTexto[2] = numero % 10;
	pTexto[2] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se imprime el numero
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 5, pTexto );

}



/****************************************************************/
/* EscribirAnchoDePulso											*/
/*  															*/
/*  Muestra el duty de la senial en el LCD						*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirAnchoDePulso( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[3];
	
	// Se imprime un mensaje indicando si el ajuste esta bien o no
	if( numero >= 440 )
		if( numero <= 450 )
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "OK " );
		else
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );
	else
		( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, "MAL" );

	// Se toman las unidades
	pTexto[2] = numero % 10;
	pTexto[2] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[1] = numero % 10;
	pTexto[1] += '0';
	
	// Se toman las centenas
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se imprime el numero
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 3, pTexto );
}



/****************************************************************/
/* 	EscribirDatoEnviado											*/
/*  															*/
/*  Muestra el dato enviado en el LCD							*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirDatoEnviado( unsigned int numero )
{
	// Variables auxiliares
	unsigned char 	pTexto[3];
	
	// Se toman las unidades
	pTexto[2] = numero % 10;
	pTexto[2] += '0';
	
	// Se toman las decenas
	numero /= 10;
	pTexto[1] = numero % 10;
	pTexto[1] += '0';
	
	// Se toman las centenas
	numero /= 10;
	pTexto[0] = numero % 10;
	pTexto[0] += '0';
	
	// Se imprime el numero
	EscribirMensajeRenglonSuperior( "Env     " );
	( void ) EscribirMensajeLCD( RENGLON_SUPERIOR, 5, 3, pTexto );
}



/****************************************************************/
/* 	EscribirDatoRecibido										*/
/*  															*/
/*  Muestra el dato recibido en el LCD							*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirDatoRecibido( unsigned int numero, char condicion )
{
	// Variables auxiliares
	unsigned char 	pTexto[3];
	
	if( condicion == CONDICION_DATO_NO_RECIBIDO )
	{
		EscribirMensajeRenglonSuperior( " Ver M5 " );
		EscribirMensajeRenglonInferior( " No rec " );
	}
	else
	{
		if( condicion == CONDICION_DATO_RECIBIDO_MAL )
		{
			// Se toman las unidades
			pTexto[2] = numero % 10;
			pTexto[2] += '0';
			
			// Se toman las decenas
			numero /= 10;
			pTexto[1] = numero % 10;
			pTexto[1] += '0';
			
			// Se toman las centenas
			numero /= 10;
			pTexto[0] = numero % 10;
			pTexto[0] += '0';
			
			// Se imprime el numero
			EscribirMensajeRenglonInferior( "Err     " );
			( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, pTexto );
		}
		else
		{
			if( condicion == CONDICION_DATO_RECIBIDO_OK )
			{
				// Se toman las unidades
				pTexto[2] = numero % 10;
				pTexto[2] += '0';
				
				// Se toman las decenas
				numero /= 10;
				pTexto[1] = numero % 10;
				pTexto[1] += '0';
				
				// Se toman las centenas
				numero /= 10;
				pTexto[0] = numero % 10;
				pTexto[0] += '0';
				
				// Se imprime el numero
				EscribirMensajeRenglonInferior( "OK      " );
				( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 5, 3, pTexto );
			}
		}
	}
}



/****************************************************************/
/* EscribirMensajeRenglonSuperior								*/
/*  															*/
/*  Muestra el mensaje en el renglon superior del LCD			*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirMensajeRenglonSuperior( unsigned char renglonSuperior[8] )
{
	// Se imprime un mensaje
	( void ) EscribirMensajeLCD( RENGLON_SUPERIOR, 0, 8, renglonSuperior );
}



/****************************************************************/
/* EscribirMensajeRenglonInferior								*/
/*  															*/
/*  Muestra el mensaje en el renglon inferior del LCD			*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirMensajeRenglonInferior( unsigned char renglonInferior[8] )
{
	// Se imprime un mensaje
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 8, renglonInferior );
}



/****************************************************************/
/* EscribirMensaje												*/
/*  															*/
/*  Muestra el mensaje en el LCD								*/
/*  															*/
/*  Recibe: El numero a mostrar									*/
/*  Devuelve: Nada												*/
/****************************************************************/
void EscribirMensaje( unsigned char renglonSuperior[8], unsigned char renglonInferior[8] )
{
	// Se imprime un mensaje
	( void ) EscribirMensajeLCD( RENGLON_SUPERIOR, 0, 8, renglonSuperior );
	( void ) EscribirMensajeLCD( RENGLON_INFERIOR, 0, 8, renglonInferior );
}







/****************************************************************************************************************/
/*				  								FUNCIONES PARA LOS BOTONES										*/
/****************************************************************************************************************/

bool RevisarOpresionDelPulsador ( char pulsador )
{
	// Variables auxiliares
	char				revisionesDelPulsador = 0;
	uint16_t			cuentaFinalDelTimer;
	
	switch( pulsador )
	{
		case PULSADOR_OK:
			for( revisionesDelPulsador = 0; revisionesDelPulsador < 5; revisionesDelPulsador++ )
			{
				// Si el pulsador esta presionado, se devuelve TRUE
				if( PULSADOR_OK_PIN == PULSADOR_PRESIONADO  )
					return( TRUE );
				
				// Se genera una demora de algunos milisegundos
				cuentaFinalDelTimer = TPM1CNT + 600;		// 600 cuentas = 20ms
				if( cuentaFinalDelTimer < TPM1MOD )
					while( cuentaFinalDelTimer >= TPM1CNT )
						asm( nop );
			}
			break;
			
		case PULSADOR_SIGUIENTE:
			for( revisionesDelPulsador = 0; revisionesDelPulsador < 5; revisionesDelPulsador++ )
			{
				// Si el pulsador esta presionado, se devuelve TRUE
				if( PULSADOR_SIGUIENTE_PIN == PULSADOR_PRESIONADO  )
					return( TRUE );
				
				// Se genera una demora de algunos milisegundos
				cuentaFinalDelTimer = TPM1CNT + 600;		// 600 cuentas = 20ms
				if( cuentaFinalDelTimer < TPM1MOD )
					while( cuentaFinalDelTimer >= TPM1CNT )
						asm( nop );
			}
			break;
	}
	
	// Si estaba mal la indicacion del pulsador, o no se oprimio, se devuelve false
	return( FALSE );
}





/****************************************************************************************************************/
/*									FUNCIONES PARA LA COMUNICACION SERIE										*/
/****************************************************************************************************************/

void InicializarComunicacionSerie( void )
{
	// Se define como salida el pin para habilitar el integrado RS485
	PTE_485_DIRECCION = 1;
	
	// Se lo deja como receptor, por las dudas
	PTE_485 = RECEPTOR;
	
	// Se selecciona un baudaje de 9600 (en realidad no da extacto, al ser fBus / ( 16 * SCI2BD ) )
	SCI1BDH = 0;
	SCI1BDL = 25;
	
	// SCI1C1 - SCI2 Control Register 1;
	SCI1C1_PT = 0;			// Parity Type = Even parity
	SCI1C1_PE = 0;			// Parity Enable = No hardware generation
	SCI1C1_ILT = 0;			// Idle Line Type Select = Idle character bit count starts after start bit
	SCI1C1_WAKE = 0;		// Receiver Wakeup Method Select = Idle-Line wakeup
	SCI1C1_M = 0;			// 9-Bit or 8-Bit Mode Select = 8 bits
	SCI1C1_RSRC = 0;		// Receiver Source Select = Don't use
	SCI1C1_SCISWAI = 0;		// SCI Stops in Wait Mode = Don't freeze SCI clocks in wait mode
	SCI1C1_LOOPS = 0;		// Loop Mode Select = No loop
	
	// SCI1C2 - SCI2 Control Register 2;
	SCI1C2_SBK = 0;			// Send Break = Normal transmitter operation
	SCI1C2_RWU = 0;			// Receiver Wakeup Control = Normal SCI receiver operation
	SCI1C2_RE = 1;			// Receiver Enable = Receiver on
	SCI1C2_TE = 1;			// Transmitter Enable = Transmitter on 
	SCI1C2_ILIE = 0;		// Idle Line Interrupt Enable (for IDLE) = Hardware interrupts from IDLE disabled
	SCI1C2_RIE = 1;			// Receiver Interrupt Enable (for RDRF) = Hardware interrupts from RDFR enabled
	SCI1C2_TCIE = 0;		// Transmission Complete Interrupt Enable (for TC) = Hardware interrupts requested when TC flag is 1 
	SCI1C2_TIE = 0;			// Transmit Interrupt Enable (for TDRE) = Hardware interrupts from TDRE disabled
	
	// SCI1C3 - SCI2 Control Register 3;
	SCI1C3_PEIE = 0;		// Parity Error Interrupt Enable = PF interrupts disabled 
	SCI1C3_FEIE = 0;		// Framing Error Interrupt Enable = FE interrupts disabled
	SCI1C3_NEIE = 0;		// Noise Error Interrupt Enable = NF interrupts disabled
	SCI1C3_ORIE = 0;		// Overrun Interrupt Enable = OR interrupts disabled
	SCI1C3_TXINV = 0;		// Transmit Data Inversion = Transmit data not inverted
	SCI1C3_TXDIR = 0;		// TxD Pin Direction in Single-Wire Mode = Don't use
	SCI1C3_T8 = 0;			// Ninth Data Bit for Transmitter = Don't use
	SCI1C3_R8 = 0;			// Ninth Data Bit for Receiver = Don't use
	
	// Se lo deja como transmisor
	PTE_485 = TRANSMISOR;
}



void EnviarDatoComunicacionSerie( void )
{
	// Variable auxiliar
	char	flag;
	
	// Se habilita el integrado de la comunicacion para transmitir
	PTE_485 = TRANSMISOR;
	
	// Se borra el flag de transmicion completa (Por las dudas)
	(void) SCI1S1;
	
	// Se aguarda a que el buffer de pre transmicion este disponible para un nuevo caracter
	flag = 0;
	while( flag == 0 )
	{
		flag = SCI1S1;
		flag &= 0x80;
	}
	
	// Se incrementa el contador
	contador++;
	
	// Se lo limita, para tener codigos especiales y poder usarlos como comandos 
	if( contador > 200 )
		contador = 0;
	
	// Se escribe el dato
	SCI1D = contador;
	
	// Se espera a que se termine la transmicion para cambiar al modo de recepcion
	flag = 0;
	while( flag == 0 )
	{
		flag = SCI1S1;
		flag &= 0x40;
	}
	
	// Se habilita el integrado de la comunicacion para recibir
	PTE_485 = RECEPTOR;
}









void EnviarConfirmacionComunicacionSerie( void )
{
	// Variable auxiliar
	char	flag;
	
	// Se habilita el integrado de la comunicacion para transmitir
	PTE_485 = TRANSMISOR;
	
	// Se borra el flag de transmicion completa (Por las dudas)
	(void) SCI1S1;
	
	// Se aguarda a que el buffer de pre transmicion este disponible para un nuevo caracter
	flag = 0;
	while( flag == 0 )
	{
		flag = SCI1S1;
		flag &= 0x80;
	}
	
	// Se escribe el dato
	SCI1D = CODIGO_RECEPCION_CORRECTA;
	
	// Se espera a que se termine la transmicion para cambiar al modo de recepcion
	flag = 0;
	while( flag == 0 )
	{
		flag = SCI1S1;
		flag &= 0x40;
	}
	
	// Se habilita el integrado de la comunicacion para recibir
	PTE_485 = RECEPTOR;
}




void EnviarOrdenDeAvanzarComunicacionSerie( void )
{
	// Variable auxiliar
	char	flag;
	
	// Se habilita el integrado de la comunicacion para transmitir
	PTE_485 = TRANSMISOR;
	
	// Se borra el flag de transmicion completa (Por las dudas)
	(void) SCI1S1;
	
	// Se aguarda a que el buffer de pre transmicion este disponible para un nuevo caracter
	flag = 0;
	while( flag == 0 )
	{
		flag = SCI1S1;
		flag &= 0x80;
	}
	
	// Se escribe el dato
	SCI1D = CODIGO_SIGUIENTE_ORDEN;
	
	// Se espera a que se termine la transmicion para cambiar al modo de recepcion
	flag = 0;
	while( flag == 0 )
	{
		flag = SCI1S1;
		flag &= 0x40;
	}
	
	// Se habilita el integrado de la comunicacion para recibir
	PTE_485 = RECEPTOR;
}








/****************************************************************************************************************/
/*				  								FUNCIONES GENERALES												*/
/****************************************************************************************************************/

void AvanzarPasosEnLaCalibracion ( char cantidadDePasos )
{
	EstadoDeLaCalibracion += cantidadDePasos;
	EscribirMensaje( "   OK   ", "        " );
	MensajeYaMostrado = FALSE;
}







/****************************************************************************************************************/
/*				  								FUNCIONES PARA EL TIMER											*/
/****************************************************************************************************************/

void HabilitarMedicionDeFrecuenciaYPeriodo ( void )
{
	setReg8( TPM1SC, 	0x00U);				// Apagar el modulo en general del timer 1
	setReg8( TPM1C0SC, 	0x00U); 	 		// Resetear los datos del canal 0 en particular
	setReg8( TPM1CNTH, 	0x00U);				// Borrar el contador de pulsos del timer 1
	setReg16( TPM1MOD, 	30310);				// Se carga la cuenta para que genere una interrupcion a cada segundo
	setReg8( TPM1C0SC, 	0x44U); 			// Se setea el timer para hacer un input capture y que genere una interrupcion cuando desborde
	setReg8( TPM1SC, 	0x4fU);             // Se setea el canal para tomar los pulsos positivos y generar una interrupcion ante cada uno
}



void HabilitarTimerComoGeneral ( void )
{
	setReg8( TPM1SC, 	0x00U);				// Apagar el modulo en general del timer 1
	setReg8( TPM1CNTH, 	0x00U);				// Borrar el contador de pulsos del timer 1
	setReg16( TPM1MOD, 	30310);				// Se carga la cuenta para que genere una interrupcion a cada segundo
	setReg8( TPM1SC, 	0x4fU);             // Se setea el canal para generar una interrupcion ante cada uno
}






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

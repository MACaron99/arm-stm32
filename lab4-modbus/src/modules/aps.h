
#ifndef APS_H_
#define APS_H_



uint16_t  aps_output_voltage_get( void );
int8_t  aps_is_power_good( void );

void  aps_output_voltage_check( void );
void  aps_init( void );


#endif /* APS_H_ */

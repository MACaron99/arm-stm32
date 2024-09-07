
#include <system.h>

#include "pi.h"


#define RESOLUTION                  (  13  )


static
s16 fmul32( s16 value, s16 c )
{
	s32 temp = value;

	temp  *= c;
	temp >>= RESOLUTION;

	return temp;
}


s16 pi_control( pi_t *pi, s16 error, u16 period )
{
	s16  new_p;   /* новое значение пропорциональной составляющей */
	s16  new_i;   /* новое значение интегральной составляющей     */
	s32  rem_i;   /* остаток от интегральной составляющей         */
	s16  u;       /* выходная величина                            */

	/* Расчет новых значений PID */
	new_p = fmul32(error, pi->kp);
	new_i = fmul32(error + pi->acc_i, pi->ki * period);

	/* Расчет выходной величины */
	u = new_p + new_i;

	/* Проверка границ */
	if( u > pi->u_max )
	{
		u = pi->u_max;

		rem_i = u - new_p;

		if( rem_i > 0 )
		{
			pi->acc_i = ((rem_i << RESOLUTION) / (pi->ki * period)) - error;
		}
		else
		{
			pi->acc_i = 0;
		}
	}
	else
	if( u < pi->u_min )
	{
		u = pi->u_min;

		rem_i = pi->u_min - new_p;

		if( rem_i < 0 )
		{
			pi->acc_i = ((rem_i << RESOLUTION) / (pi->ki * period)) - error;
		}
		else
		{
			pi->acc_i = 0;
		}
	}
	else
	{
		pi->acc_i += error;
	}

	return u;
}


#ifndef PI_H_
#define PI_H_

typedef struct
{
	s16  kp;     /*  Пропорциональный коэффициент          */
	s16  ki;     /*  Интегральный коэффициент              */
	s16  u_min;  /*  Минимальное значение воздействия      */
	s16  u_max;  /*  Максимальное значение воздействия     */
	s32  acc_i;  /*  Текущее значение аккумулятора ошибки  */

} pi_t;

s16 pi_control( pi_t *pid, s16 error, u16 period );

#endif /* PI_H_ */

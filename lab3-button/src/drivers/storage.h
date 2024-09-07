/*
 * store.h
 *
 *  Created on: 6 апр. 2018 г.
 *      Author: V.Dudnik
 */

#ifndef DRIVERS_STORAGE_H_
#define DRIVERS_STORAGE_H_


u32  storage_read(u16 param);
void storage_write(u16 param, u32 value);
void storage_init(void (*save_cb)(void));


#endif /* DRIVERS_STORAGE_H_ */

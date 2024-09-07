//*****************************************************************************
//
// File Name    : 'sys_error.c'
// Title        :
// Author       : I.Otrokha
// Created      : 02.04.2011
// Revised      :
// Version      :
// Target MCU   :
// Editor Tabs  :
//
// Description  : provide exception mechanism.
//
//
// License      : Impuls ltd.
//
//
//*****************************************************************************

#include "sys_error.h"


static err_t system_error;


err_t
sys_error_get( void )
{
	return system_error;
}


void
sys_error_set( err_t error )
{
	system_error = error;
}

void
sys_error_clear( void )
{
	system_error = 0;
}

void
sys_error_clear_warning( void )
{
	if( system_error < ERR_CRITICAL )
		system_error = 0;
}

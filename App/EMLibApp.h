#ifndef _EMLIBGUIAPP_H_
#define _EMLIBGUIAPP_H_

#include <stdint.h>
#include "emSwLib.h"

/*** FUNCTIONS ***/

//! \brief This routine will Initialize the EM Library at the start of the
//!         GUI example and on reset
//!
//! This loop will initialize the EM library based on the EM_userConfig.c/.h
//!
//! \return none
void initEMLibGUIApp(void);

#endif /* _EMLIBGUIAPP_H_ */


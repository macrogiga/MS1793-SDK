/**************************************************************************//**
* @file HAL_device.h
* @brief CMSIS Cortex-M Peripheral Access Layer for MindMotion
*        microcontroller devices
*
* This is a convenience header file for defining the part number on the
* build command line, instead of specifying the part specific header file.
*
* Example: Add "-DMM32X031" to your build options, to define part
*          Add "#include "HAL_device.h" to your source files
*
*
* @version 1.5.0
*
*
*****************************************************************************/

#ifndef __HAL_device_H
#define __HAL_device_H


#define MM32X031
#if defined(MM32X103)
#include "MM32x103.h"
#include "system_mm32x103.h"
#elif defined(MM32X031)
#include "MM32x031.h"

#else
#error "HAL_device.h: PART NUMBER undefined"
#endif
#endif /* __HAL_device_H */
/*-------------------------(C) COPYRIGHT 2016 MindMotion ----------------------*/

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "includes.h"

#define MAX_DEV     64

#define DEV_REG_FAIL    1
#define DEV_REG_SUCCESS 0

#define DEV_OPEN_FAIL   (MAX_DEV + 1)
#define DEV_NOT_EXIST   (MAX_DEV + 2)
#define DEV_INVALID     (MAX_DEV + 3)
#define DEV_NOT_OPENED  (MAX_DEV + 4)

#define DEV_SUCCESS     0
#define DEV_FAIL        1

typedef struct{
    INT32U ( *read ) ( 
                     INT8U * buf,       // Read buffer 
                     INT32U size,       // Read size 
                     INT32U param,      // read parameter
                     void* pParam );    // parameter sturcture pointer

    INT32U ( *write )(
                     INT8U * buf,       // Write buffer 
                     INT32U size,       // Write size 
                     INT32U param,      // Write parameter
                     void* pParam );    // parameter sturcture pointer

    INT32U ( *open ) ( 
                       INT32U param,        // Open param
                       void* pParam );      // Open param structure pointer

    INT32U ( *close ) ( void );       // Device number

    INT8U* dev_name;                        // Store device's name

    INT32U isOpen;                          // dev open status
} dev_t;



INT32U dev_reg ( dev_t dev );           // Register DEV

INT32U dev_read ( INT32U dev, 
                  INT8U* buf,
                  INT32U size,
                  INT32U param,
                  void* pParam );

INT32U dev_write ( INT32U dev,
                   INT8U* buf,
                   INT32U size,
                   INT32U param,
                   void* pParam );

INT32U dev_open( INT8U* pDevName, 
                 INT32U param,
                 void* pParam );

INT32U dev_close( INT32U dev );








#endif

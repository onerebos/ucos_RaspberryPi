#include "device.h"

/*   This file setup a device user interface to register device into 
 *   the whole system. It provides read/write/open/close/ioctl trying 
 *   to simulate similar mechanizm like linux device driver
 */



/* typedef Struct{ */
    /* INT32U ( *read ) (*/ 
                     /* INT8U * buf,       // Read buffer  */
                     /* INT32U size,       // Read size  */
                     /* INT32U param,      // read parameter */
                     /* void* pParam );    // parameter sturcture pointer */

    /* INT32U ( *write )(*/ 
                     /* INT8U * buf,       // Write buffer  */
                     /* INT32U size,       // Write size  */
                     /* INT32U param,      // Write parameter */
                     /* void* pParam );    // parameter sturcture pointer */

    /* INT32U ( *open ) (*/ 
                       /* INT32U param,        // Open param */
                       /* void* pParam );      // Open param structure pointer */

    /* INT32U ( *close ) ( INT32U dev );       // Device number */

    /* INT8U* dev_name;                        // Store device's name */
    
    /* INT32U isOpen;                          // Open status */

/* } dev_t; */

dev_t devListAll [MAX_DEV];

void dev_init( void )
{
    memset( (INT8U*) devListAll, sizeof(devListAll), 0 );
}


INT32U dev_reg ( dev_t dev )           // Register DEV
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
 
    OS_ENTER_CRITICAL();

    dev_t* pFreeDevSlot = 0;
    INT32U i;

    for( i = 0; i < MAX_DEV ; i ++ )
    {
        if( NULL == devListAll[i].dev_name )
        {
            pFreeDevSlot = &( devListAll[i] );
            break;
        }
    }

    if( NULL == pFreeDevSlot )
    {
        OS_EXIT_CRITICAL();
        return DEV_REG_FAIL;
    }

    if( NULL != dev.dev_name  &&
        NULL != dev.open    &&
        NULL != dev.read    &&
        NULL != dev.close   &&
        NULL != dev.write )
    {
        *pFreeDevSlot = dev;
    }

    OS_EXIT_CRITICAL();

    return DEV_SUCCESS;
}

INT32U dev_read ( INT32U dev, 
                  INT8U* buf,
                  INT32U size,
                  INT32U param,
                  void* pParam )
{
    if( dev >=  MAX_DEV )
    {
        return DEV_INVALID;
    }

    if( NULL == devListAll[ dev ].dev_name )
    {
        return DEV_NOT_EXIST;
    }

    return devListAll[ dev ].read( buf, size, param, pParam );
    
}

INT32U dev_write ( INT32U dev,
                   INT8U* buf,
                   INT32U size,
                   INT32U param,
                   void* pParam )
{

    if( dev >=  MAX_DEV )
    {
        return DEV_INVALID;
    }

    if( NULL == devListAll[ dev ].dev_name )
    {
        return DEV_NOT_EXIST;
    }

    return devListAll[ dev ].write( buf, size, param, pParam );
 
}

INT32U dev_open( INT8U* pDevName, 
                 INT32U param,
                 void* pParam )
{
    INT32U i;
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
 
    OS_ENTER_CRITICAL(); 
    
    for( i = 0; i < MAX_DEV ; i ++ )
    {
       if( ! strcmp( pDevName, devListAll[i].dev_name ) )
       {
           if( TRUE == devListAll[i].isOpen )
           {
               OS_EXIT_CRITICAL();
               return DEV_FAIL;
           }
           OS_EXIT_CRITICAL();

           devListAll[i].open( param, pParam );
           return i;
        }
    }

    OS_EXIT_CRITICAL();
    return DEV_FAIL;
}

INT32U dev_close( INT32U dev )
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
 
    OS_ENTER_CRITICAL();

    if( NULL == devListAll[dev].dev_name )
    {
        return DEV_NOT_EXIST;
    }

    if( FALSE == devListAll[ dev ].isOpen )
    {
        return DEV_NOT_OPENED;
    }

    devListAll[ dev ].isOpen = TRUE;
    devListAll[ dev ].close();

    OS_EXIT_CRITICAL();

    return DEV_SUCCESS;

}


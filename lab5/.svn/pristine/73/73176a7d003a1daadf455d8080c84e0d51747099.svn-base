#ifndef _LCOM_VC_MACROS_H
#define _LCOM_VC_MACROS_H
#include <lcom/lcf.h>

#pragma pack(1)
typedef struct /*the information block is 512 bytes in size*/
{
    char         VbeSignature[4];      
    BCD          VbeVersion[2];           
    phys_bytes   OemStringPtr;    /*Far-pointers are 4 byte addresses*/
    uint32_t     Capabilities;      /* is a BCD value*/  
    phys_bytes   VideoModePtr;     /*Far-pointers are 4 byte addresses*/     
    uint16_t     TotalMemory;        /* field indicates the maximum amount of memory physically                                      installed and available to the frame buffer in 64KB units - must multiply by 64*/  
    BCD          OemSoftwareRev[2];          
    phys_bytes   OemVendorNamePtr;     /*Far-pointers are 4 byte addresses*/
    phys_bytes   OemProductNamePtr;    /*Far-pointers are 4 byte addresses*/   
    phys_bytes   OemProductRevPtr;    /*Far-pointers are 4 byte addresses*/
    char         Reserved[222];          
    char         OemData[256];       /*256 byte data area that is used to return OEM information*/
} VbeInfoBlock;
#pragma options align=reset

#define VBE                0x4F00
#define SET_VBE_MODE       0x2    /**< @brief Set VBE mode, VBE call, function 02 */
#define GET_VBE_MODE_INFO  0x1   /**< @brief Return VBE Mode Information, VBE call, function 01 */
#define GET_VBE_CTRL_INFO  0x0   /**< @brief Return VBE Controller Info, VBE call,function 00 */

#define MODE_105          0x105

#define SET_LINEAR_MODE   BIT(14)     /**< @brief set bit 14: linear framebuffer*/
#define VC_INT            0x10

#define VC_AH_SUCCESS     0x01

#define DIRECT_COLOR_MODEL2  7
#define DIRECT_COLOR_MODEL1  6

#endif // _LCOM_VC_MACROS_H

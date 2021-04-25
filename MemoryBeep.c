
#include <Token.h>
#include "Efi.h"
#include <AmiLib.h>
#include <AmiCspLib.h>
#include <Setup.h>
#include <Ppi\Smbus2.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/SmbusLib.h>

#include <Library/AmiBeepLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h> 

EFI_STATUS DetectSPDData(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
);

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = 
{
    {
        (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
        &gEfiPeiSmbus2PpiGuid,
        DetectSPDData
    }
};


EFI_STATUS
EFIAPI
DetectSPDData
(
IN EFI_PEI_SERVICES **PeiServices,
IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
IN VOID *Ppi
)
{
	EFI_STATUS			Status = EFI_SUCCESS;
	RETURN_STATUS       SPD1Status,SPD2Status; 
	UINT8               i;
	UINT32              Data32;

	SPD1Status = RETURN_DEVICE_ERROR;
	SPD1Status = RETURN_DEVICE_ERROR;
	
	SmBusReadDataByte (0xA0 | 0x0000, &SPD1Status);
	SmBusReadDataByte (0xA4 | 0x0000, &SPD2Status);
	
	if((RETURN_SUCCESS != SPD1Status) && (RETURN_SUCCESS != SPD2Status))
	           {
	               IoWrite8(0x80,0xBB);
	                              
	               //Config GPP_B14 as Speaker function
	               Data32 = MmioRead32(0xFD000000 + 0xAF0530);
	               Data32 &= ~(BIT11 + BIT10);
	               Data32 |= BIT10;
	               MmioWrite32(0xFD000000 + 0xAF0530, Data32);
	                                          
	               while(TRUE)
	               {
	                 for (i=0; i<5; i++) { AmiBeep(29366*2,400000); MicroSecondDelay(100000);}
	                 MicroSecondDelay(1000000);
	               }
	           }
 return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
MemoryBeepEntryPoint (
  IN EFI_PEI_FILE_HANDLE       		*FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
	EFI_STATUS			Status = EFI_SUCCESS;
	
	Status = (*PeiServices)->NotifyPpi(PeiServices, mNotifyList);
	
	return Status;
}

/**
 * @file  FchData.c
 * @brief Allocate the RT region for FCH runtime data
 *
 */
/**
 * Copyright 2024 American Megatrends LLC., All rights reserved.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Sil-api.h>
#include <FCHClass-api.h>
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <FchCore/FchHwAcpi/FchHwAcpiReg.h>
#include <CommonLib/Mmio.h>

/**
 * FchAllocateRuntimeBlock
 *
 * @brief   Allocate runtime memory block and copy IP data
 *
 */
EFI_STATUS
FchAllocateRuntimeBlock (
  EFI_PHYSICAL_ADDRESS  *FchRtBlock
)
{
  FCHCLASS_INPUT_BLK *FchData;
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS RtAddr;

  FchData = (FCHCLASS_INPUT_BLK *)SilFindStructure (SilId_FchClass,  0);

  DEBUG ((DEBUG_INFO, "FchDxeDataInit: FCHCLASS_INPUT_BLK at: 0x%X\n", FchData));
  ASSERT (FchData != NULL);
  if (FchData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  RtAddr = 0xFFFFFFFF;
  Status = gBS->AllocatePages (AllocateMaxAddress, EfiReservedMemoryType, 1, &RtAddr);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;

  gBS->CopyMem ((VOID*)(UINTN)RtAddr, &FchData->FchRunTime, sizeof (FCH_RUNTIME));

  *FchRtBlock = RtAddr;

  return EFI_SUCCESS;
}

/**
 * ProgramAddressInCmos
 *
 * @brief   Program given address in extended CMOS offset 8 to be used by AML
 *
 */
EFI_STATUS
ProgramAddressInCmos (
  UINT32  Address32
)
{
  xUSLMemWrite8((VOID*)(UINTN)(ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x08), (UINT8) ((Address32 >> 0) & 0xFF));
  xUSLMemWrite8((VOID*)(UINTN)(ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x09), (UINT8) ((Address32 >> 8) & 0xFF));
  xUSLMemWrite8((VOID*)(UINTN)(ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0A), (UINT8) ((Address32 >> 16) & 0xFF));
  xUSLMemWrite8((VOID*)(UINTN)(ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0B), (UINT8) ((Address32 >> 24) & 0xFF));
  
  return EFI_SUCCESS;
}


EFI_STATUS
FchDxeDataInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  FchRtBlock;

  DEBUG ((DEBUG_INFO, "FCH DXE Init.\n"));

  Status = FchAllocateRuntimeBlock (&FchRtBlock);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;

  ASSERT (FchRtBlock < 0x100000000);

  Status = ProgramAddressInCmos ((UINT32)FchRtBlock);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


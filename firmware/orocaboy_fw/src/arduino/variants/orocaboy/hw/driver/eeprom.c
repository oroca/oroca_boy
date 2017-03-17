/*
 *  eeprom.c
 *
 *  Created on: 2017. 3. 16.
 *      Author: Baram
 */

#include "eeprom.h"
#include "variant.h"





static bool IsInit = false;




/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Define of the return value */
typedef enum {
  EE_OK = 0,
  EE_ERROR,
  EE_NO_VALID_PAGE,
  EE_ERASE_ERROR,
  EE_FORMAT_ERROR,
  EE_WRITE_ERROR,
  EE_ERROR_NOVALID_PAGE,
  EE_NO_DATA,
  EE_INVALID_VIRTUALADRESS,
  EE_TRANSFER_ERROR,

  /* Internal return code */
  EE_PAGE_NOTERASED,
  EE_PAGE_ERASED,
  EE_PAGE_FULL,
} EE_Status;

/* Variables' number */
#define NB_OF_VAR             ((uint8_t)200)

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
EE_Status EE_Init(void);
EE_Status EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
EE_Status EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*
   This definition part is used to describe the page setting and
   can be custom
*/
#define PAGE_SIZE               (uint32_t)FLASH_PAGE_SIZE  /* Page size */

#define PAGE0_NUMBER            (uint32_t)126
#define PAGE0_BANKNUMBER        FLASH_BANK_1

#define PAGE1_NUMBER            (uint32_t)127
#define PAGE1_BANKNUMBER        FLASH_BANK_1

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS     (uint32_t)(FLASH_BASE + PAGE0_NUMBER*PAGE_SIZE)

#define PAGE1_BASE_ADDRESS     (uint32_t)(FLASH_BASE + PAGE1_NUMBER*PAGE_SIZE)

/*
   End of the page definition
*/

/* Page stat header */
#define EE_PAGESTAT_ERASED  ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define EE_PAGESTAT_VALID   ((uint64_t)0x0000000000000000)
#define EE_PAGESTAT_RECEIVE ((uint64_t)0x00000000AAAAAAAA)

/* Page stat : size of the data in flash is 64 bits */
#define EE_DATA_SIZE             8
#define EE_DATA_TYPE             uint64_t
#define EE_DATA_SHIFT            32
#define EE_MASK_VIRTUALADRESS    (uint64_t)0x0000FFFF00000000
#define EE_MASK_DATA             (uint64_t)0xFFFF000000000000
#define EE_MASK_FULL             (uint64_t)0xFFFFFFFFFFFFFFFF

/* Type of find requested :
       READ -> page in valid state
       WRTIE --> page in recpetion state or valid state
       ERASE --> page in erase state */
typedef enum {
   FIND_READ_PAGE,
   FIND_WRITE_PAGE,
   FIND_ERASE_PAGE
} EE_Find_type;

/* No valid page define */
#define EE_NO_VALID_PAGE         ((uint32_t)0xFFFFFFFF)

/* defintion of the different type of page transfer
        NORMAL  -> copie data pag source to page destination
        RECOVER -> resolve confict when one page reception and a second is valid */
typedef enum {
  EE_TRANSFER_NORMAL,
  EE_TRANSFER_RECOVER
} EE_Transfer_type;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static EE_Status EE_Format(void);
static uint32_t EE_FindPage(EE_Find_type Operation);
static EE_Status EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data);
static EE_Status EE_PageTransfer(uint16_t VirtAddress, uint16_t Data, EE_Transfer_type type);
static EE_Status EE_VerifyPageFullyErased(uint32_t Address, uint32_t PageSize);
static EE_Status EE_PageErase(uint32_t Page, uint16_t BankNb);
static uint32_t EE_GetPageNumber(uint32_t Address);
static uint32_t EE_GetBankNumber(uint32_t Address);

/* Virtual address defined by the user: 0xFFFF value is prohibited */
static uint16_t VirtAddVarTab[NB_OF_VAR];




int eepromInit()
{
  uint16_t i;


  for( i=0; i<NB_OF_VAR; i++ )
  {
    VirtAddVarTab[i] = i;
  }


  HAL_FLASH_Unlock();

  if( EE_Init() == EE_OK )
  {
    IsInit = true;
  }

  return 0;
}



uint8_t eepromReadByte(int addr)
{
  uint16_t read_value;


  if( IsInit == false ) return 0;

  EE_ReadVariable((uint16_t)addr,  &read_value);

  return (uint8_t)read_value;
}


void eepromWriteByte(int index, uint8_t data_in)
{
  if( IsInit == false ) return;

  EE_WriteVariable(index, (uint16_t)data_in);
}


uint16_t eepromGetLength(void)
{
  if( IsInit == false ) return 0;

  return NB_OF_VAR;
}





/**
  * @brief  Restore the pages to a known good state in case of page's status
  *   corruption after a power loss.
  * @param  None.
  * @retval - EE_OK in cas of succes
  *         - EE error code in case of error
  */
EE_Status EE_Init(void)
{
  EE_DATA_TYPE pagestatus0, pagestatus1, addressvalue;

  /* check the variable definition */
  for(uint32_t varidx = 0; varidx < NB_OF_VAR; varidx++)
  {
    if(VirtAddVarTab[varidx] == 0xFFFF)
    {
      return EE_INVALID_VIRTUALADRESS;
    }
  }

  /* Get Page0 status */
  pagestatus0 = (*(__IO EE_DATA_TYPE*)PAGE0_BASE_ADDRESS);
  /* Get Page1 status */
  pagestatus1 = (*(__IO EE_DATA_TYPE*)PAGE1_BASE_ADDRESS);

  /* Check for invalid header states and repair if necessary */
  switch (pagestatus0)
  {
  case EE_PAGESTAT_ERASED:
    {
      if (pagestatus1 == EE_PAGESTAT_VALID) /* Page0 erased, Page1 valid */
      {
        /* Erase Page0 */
        if(EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS, PAGE_SIZE) == EE_PAGE_NOTERASED)
        {
          if(EE_PageErase(PAGE0_NUMBER, PAGE0_BANKNUMBER) != EE_OK)
          {
            return EE_ERASE_ERROR;
          }
        }
      }
      else if (pagestatus1 == EE_PAGESTAT_RECEIVE) /* Page0 erased, Page1 receive */
      {
        /* Erase Page0 */
        if(EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS, PAGE_SIZE) == EE_PAGE_NOTERASED)
        {
          if(EE_PageErase(PAGE0_NUMBER,PAGE0_BANKNUMBER) != EE_OK)
          {
            return EE_ERASE_ERROR;
          }
        }

        /* Mark Page1 as valid */
        /* If program operation was failed, a Flash error code is returned */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE1_BASE_ADDRESS, EE_PAGESTAT_VALID) != HAL_OK)
        {
          return EE_WRITE_ERROR;
        }
      }
      else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        /* If erase/program operation was failed, a Flash error code is returned */
        if (EE_Format() != EE_OK)
        {
          return EE_FORMAT_ERROR;
        }
      }
    }
    break;

  case EE_PAGESTAT_RECEIVE:
    {
      if (pagestatus1 == EE_PAGESTAT_VALID) /* Page0 receive, Page1 valid */
      {
        /* Get the first udpated value from the reception page */
        addressvalue = (*(__IO EE_DATA_TYPE*)(PAGE0_BASE_ADDRESS + EE_DATA_SIZE));
        /* Restart the interrupted page transfer */
        if(EE_PageTransfer((addressvalue & EE_MASK_VIRTUALADRESS) >> EE_DATA_SHIFT, (uint16_t)((addressvalue & EE_MASK_DATA) >> (EE_DATA_SHIFT+16)), EE_TRANSFER_RECOVER) != EE_OK)
        {
          return EE_TRANSFER_ERROR;
        }
      }
      else if (pagestatus1 == EE_PAGESTAT_ERASED) /* Page0 receive, Page1 erased */
      {
        /* Erase Page1 */
        if(EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS, PAGE_SIZE) == EE_PAGE_NOTERASED)
        {
          /* If erase operation was failed, a Flash error code is returned */
          if(EE_PageErase(PAGE1_NUMBER,PAGE1_BANKNUMBER) != EE_OK)
          {
            return EE_ERASE_ERROR;
          }
        }
        /* Mark Page0 as valid */
        /* If program operation was failed, a Flash error code is returned */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE0_BASE_ADDRESS, EE_PAGESTAT_VALID) != HAL_OK)
        {
          return EE_WRITE_ERROR;
        }
      }
      else /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        /* If erase/program operation was failed, a Flash error code is returned */
        if (EE_Format() != EE_OK)
        {
          return EE_FORMAT_ERROR;
        }
      }
    }
    break;

  case EE_PAGESTAT_VALID:
    {
      if (pagestatus1 == EE_PAGESTAT_VALID) /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        /* If erase/program operation was failed, a Flash error code is returned */
        if (EE_Format() != EE_OK)
        {
          return EE_FORMAT_ERROR;
        }
      }
      else if (pagestatus1 == EE_PAGESTAT_ERASED) /* Page0 valid, Page1 erased */
      {
        /* Erase Page1 */
        if(EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS, PAGE_SIZE) == EE_PAGE_NOTERASED)
        {
          /* If erase operation was failed, a Flash error code is returned */
          if(EE_PageErase(PAGE1_NUMBER,PAGE1_BANKNUMBER) != EE_OK)
          {
            return EE_ERASE_ERROR;
          }
        }
      }
      else /* Page0 valid, Page1 receive */
      {
        /* Get the first udpated value from the reception page */
        addressvalue = (*(__IO EE_DATA_TYPE*)(PAGE1_BASE_ADDRESS + EE_DATA_SIZE));
        /* Restart the interrupted page transfer */
        if(EE_PageTransfer((addressvalue & EE_MASK_VIRTUALADRESS) >> EE_DATA_SHIFT, (uint16_t)((addressvalue & EE_MASK_DATA) >> (EE_DATA_SHIFT+16)), EE_TRANSFER_RECOVER) != EE_OK)
        {
          return EE_TRANSFER_ERROR;
        }
      }
    }
    break;

  default:  /* Any other state -> format eeprom */
    {
      /* Erase both Page0 and Page1 and set Page0 as valid page */
      /* If erase/program operation was failed, a Flash error code is returned */
      if (EE_Format() != EE_OK)
      {
        return EE_FORMAT_ERROR;
      }
    }
    break;
  }

  return EE_OK;
}

/**
  * @brief  Verify if specified page is fully erased.
  * @param  Address: page address
  *   This parameter can be one of the following values:
  *     @arg PAGE0_BASE_ADDRESS: Page0 base address
  *     @arg PAGE1_BASE_ADDRESS: Page1 base address
  * @param  PageSize: page size
  * @retval page fully erased status:
  *           - EE_PAGE_NOTERASED : if Page not erased
  *           - EE_PAGE_ERASED    : if Page erased
  */
EE_Status EE_VerifyPageFullyErased(uint32_t Address, uint32_t PageSize)
{
  EE_Status readstatus = EE_PAGE_ERASED;
  uint32_t counter = 0;

  /* Check each active page address starting from end */
  while (counter < PageSize)
  {
    /* Compare the read address with the virtual address */
    if ((*(__IO EE_DATA_TYPE*)(Address+counter)) != EE_PAGESTAT_ERASED)
    {
      /* In case variable value is read, reset readstatus flag */
      readstatus = EE_PAGE_NOTERASED;
      break;
    }
    /* Next address location */
    counter = counter + EE_DATA_SIZE;
  }

  /* Return readstatus value: (0: Page not erased, 1: Page erased) */
  return readstatus;
}

/**
  * @brief  Returns the last stored variable data, if found, which correspond to
  *   the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - EE_OK: if variable was found
  *           - EE_NO_DATA: if the variable was not found
  *           - EE_ERROR_NOVALID_PAGE: if no valid page was found.
  */
EE_Status EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data)
{
  EE_DATA_TYPE addressvalue;
  uint32_t counter = PAGE_SIZE - EE_DATA_SIZE;

  /* Get active Page for read operation */
  uint32_t validpageadresse = EE_FindPage(FIND_READ_PAGE);

  /* Check if there is no valid page */
  if (validpageadresse == EE_NO_VALID_PAGE)
  {
    return  EE_ERROR_NOVALID_PAGE;
  }

  /* Check each active page address starting from end */
  while (counter >= EE_DATA_SIZE)
  {
    /* Get the current location content to be compared with virtual address */
    addressvalue = (*(__IO EE_DATA_TYPE*)(validpageadresse + counter));
    if(addressvalue != EE_PAGESTAT_ERASED)
    {
      /* Compare the read address with the virtual address */
      if ((addressvalue & EE_MASK_VIRTUALADRESS) == ((EE_DATA_TYPE)VirtAddress << EE_DATA_SHIFT))
      {
        /* Get content of Address-2 which is variable value */
        *Data = (uint16_t)((addressvalue & EE_MASK_DATA) >> (EE_DATA_SHIFT + 16));
        /* In case variable value is read, reset readstatus flag */
        return EE_OK;
      }
    }
    /* Next address location */
    counter -= EE_DATA_SIZE;
  }

  /* Return readstatus value: (EE_OK: variable exist, EE_ERROR: variable doesn't exist) */
  return EE_NO_DATA;
}

/**
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - EE_OK: on success
  *           - EE error code: if an error occurs
  */
EE_Status EE_WriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  EE_Status status;

  /* Write the variable virtual address and value in the EEPROM */
  status = EE_VerifyPageFullWriteVariable(VirtAddress, Data);
  if( status == EE_PAGE_FULL)
  {
    /* In case the EEPROM active page is full */
    /* Perform Page transfer */
    return EE_PageTransfer(VirtAddress, Data, EE_TRANSFER_NORMAL);
  }

  /* Return last operation status */
  return status;
}

/**
  * @brief  Erases PAGE and PAGE1 and writes VALID_PAGE header to PAGE
  * @param  None
  * @retval Success or error status:
  *           - EE_OK: on success
  *           - EE error code: if an error occurs
  */
static EE_Status EE_Format(void)
{
  /* Erase Page0 */
  if(EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS, PAGE_SIZE) == EE_PAGE_NOTERASED)
  {
    if (EE_PageErase(PAGE0_NUMBER, PAGE0_BANKNUMBER) != EE_OK)
    {
      return EE_ERASE_ERROR;
    }
  }

  /* If program operation was failed, a Flash error code is returned */
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE0_BASE_ADDRESS, EE_PAGESTAT_VALID) != HAL_OK)
  {
    return EE_WRITE_ERROR;
  }

  /* Erase Page1 */
  if(EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS, PAGE_SIZE) == EE_PAGE_NOTERASED)
  {
    if (EE_PageErase(PAGE1_NUMBER, PAGE1_BANKNUMBER) != EE_OK)
    {
      return EE_ERASE_ERROR;
    }
  }

  return EE_OK;
}

/**
  * @brief  Find Page
  * @param  type: type of page to requested.
  *   This parameter can be one of the following values:
  *     @arg FIND_READ_PAGE: return the read page address
  *     @arg FIND_WRITE_PAGE: return the write page address
  *     @arg FIND_ERASE_PAGE: return the erase page address
  * @retval :
  *           - Page @: on success
  *           - EE_NO_VALID_PAGE : if an error occurs
  */
static uint32_t EE_FindPage(EE_Find_type Operation)
{
  EE_DATA_TYPE pagestatus0, pagestatus1;

  /* Get Page0 actual status */
  pagestatus0 = (*(__IO EE_DATA_TYPE*)PAGE0_BASE_ADDRESS);

  /* Get Page1 actual status */
  pagestatus1 = (*(__IO EE_DATA_TYPE*)PAGE1_BASE_ADDRESS);

  /* Write or read operation */
  if (Operation == FIND_WRITE_PAGE)
  {
    /* ---- Write operation ---- */
    if (pagestatus1 == EE_PAGESTAT_VALID)
    {
      /* Page0 receiving data */
      if (pagestatus0 == EE_PAGESTAT_RECEIVE)
      {
        return PAGE0_BASE_ADDRESS;         /* Page0 valid */
      }
      else
      {
        return PAGE1_BASE_ADDRESS;         /* Page1 valid */
      }
    }
    else if (pagestatus0 == EE_PAGESTAT_VALID)
    {
      /* Page1 receiving data */
      if (pagestatus1 == EE_PAGESTAT_RECEIVE)
      {
        return PAGE1_BASE_ADDRESS;         /* Page1 valid */
      }
      else
      {
        return PAGE0_BASE_ADDRESS;         /* Page0 valid */
      }
    }
    else
    {
      return EE_NO_VALID_PAGE;   /* No valid Page */
    }
  }
  else if (Operation == FIND_READ_PAGE)
  {
    /* ---- Read operation ---- */
    if (pagestatus0 == EE_PAGESTAT_VALID)
    {
      return PAGE0_BASE_ADDRESS;           /* Page0 valid */
    }
    else if (pagestatus1 == EE_PAGESTAT_VALID)
    {
      return PAGE1_BASE_ADDRESS;           /* Page1 valid */
    }
    else
    {
      return EE_NO_VALID_PAGE ;  /* No valid Page */
    }
  }
  else if (Operation == FIND_ERASE_PAGE)
  {
    /* ---- Return the erased page */
    if(pagestatus0 == EE_PAGESTAT_ERASED)
    {
      return PAGE0_BASE_ADDRESS;
    }
    if(pagestatus1 == EE_PAGESTAT_ERASED)
    {
      return PAGE1_BASE_ADDRESS;
    }
  }
  else
  {
    return EE_NO_VALID_PAGE;
  }

  return EE_NO_VALID_PAGE;
}

/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - EE_OK: on success
  *           - EE_FULL: if the page is full
  *           - EE error code: if an error occurs
  */
static EE_Status EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  uint32_t count = EE_DATA_SIZE; /* start the check after the header */

  /* Get valid Page for write operation */
  uint32_t validpage = EE_FindPage(FIND_WRITE_PAGE);
  /* Check if there is no valid page */
  if(validpage == EE_NO_VALID_PAGE)
  {
    return  EE_ERROR_NOVALID_PAGE;
  }

  /* Check each active page address starting from begining */
  while ( count < PAGE_SIZE)
  {
    /* Verify if address contents is erased */
    if ((*(__IO EE_DATA_TYPE*)(validpage+count)) == EE_MASK_FULL)
    {
      /* Set variable data + virtual adress */
      /* If program operation was failed, a Flash error code is returned */
      if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, validpage+count, ((EE_DATA_TYPE)(Data << 16) | VirtAddress) << EE_DATA_SHIFT) != HAL_OK)
      {
        return EE_WRITE_ERROR;
      }
      return EE_OK;
    }
    else
    {
      /* Next address location */
      count += EE_DATA_SIZE;
    }
  }
  /* Return PAGE_FULL in case the valid page is full */
  return EE_PAGE_FULL;
}

/**
  * @brief  Transfers last updated variables data from the full Page to
  *   an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - EE_OK: on success
  *           - EE error code: if an error occurs
  */
static EE_Status EE_PageTransfer(uint16_t VirtAddress, uint16_t Data, EE_Transfer_type type)
{
  uint32_t activepageaddress, newpageaddress;
  uint32_t varidx = 0;
  uint16_t DataValue;

  /* Get active Page for read operation */
  activepageaddress = EE_FindPage(FIND_READ_PAGE);
  if(activepageaddress == EE_NO_VALID_PAGE)
  {
    return EE_ERROR_NOVALID_PAGE;
  }

  /* Get active Page for read operation */
  newpageaddress = EE_FindPage((type == EE_TRANSFER_NORMAL?FIND_ERASE_PAGE:FIND_WRITE_PAGE));
  if(newpageaddress == EE_NO_VALID_PAGE)
  {
    return EE_ERROR_NOVALID_PAGE;
  }

  /* Mark the ativepage at receive state */
  /* If program operation was failed, a Flash error code is returned */
  if(type == EE_TRANSFER_NORMAL)
  {
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, newpageaddress, EE_PAGESTAT_RECEIVE) != HAL_OK)
    {
      return EE_WRITE_ERROR;
    }
  }

  /* Write the variable passed as parameter in the new active page */
  /* If program operation was failed, a Flash error code is returned */
  if(EE_VerifyPageFullWriteVariable(VirtAddress, Data) != EE_OK)
  {
    return EE_WRITE_ERROR;
  }

  /* Transfer process: transfer variables from old to the new active page */
  for (varidx = 0; varidx < NB_OF_VAR; varidx++)
  {
    if (VirtAddVarTab[varidx] != VirtAddress)  /* Check each variable except the one passed as parameter */
    {
      /* Read the other last variable updates */
      if(EE_ReadVariable(VirtAddVarTab[varidx], &DataValue) == EE_OK)
      {
        /* In case variable corresponding to the virtual address was found */
        /* Transfer the variable to the new active page */
        /* If program operation was failed, a Flash error code is returned */
        if(EE_VerifyPageFullWriteVariable(VirtAddVarTab[varidx], DataValue) != EE_OK)
        {
          return EE_WRITE_ERROR;
        }
      }
    }
  }

  /* Erase the current VALID_PAGE */
  if(EE_PageErase(EE_GetPageNumber(activepageaddress), EE_GetBankNumber(activepageaddress)) != EE_OK)
  {
    return EE_ERASE_ERROR;
  }

  /* Set new Page status to VALID_PAGE status */
  if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, newpageaddress, EE_PAGESTAT_VALID) != HAL_OK)
  {
    return EE_WRITE_ERROR;
  }

  /* Return last operation flash status */
  return EE_OK;
}

/**
  * @brief  Erase a page.
  * @param  Page: 32 bit Page number
  * @param  BankNb: 32 bit Bank number
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - EE_OK: on success
  *           - EE error code: if an error occurs
  */
static EE_Status EE_PageErase(uint32_t Page, uint16_t BankNb)
{
  FLASH_EraseInitTypeDef s_eraseinit;
  uint32_t page_error;

  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.NbPages     = 1;
  s_eraseinit.Page        = Page;
  s_eraseinit.Banks       = BankNb;

  /* Erase the old Page: Set old Page status to ERASED status */
  if (HAL_FLASHEx_Erase(&s_eraseinit, &page_error) != HAL_OK)
  {
    return EE_ERASE_ERROR;
  }
  return EE_OK;
}

/**
  * @brief  Gets the page of a given address
  * @param  Address: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t EE_GetPageNumber(uint32_t Address)
{
  return ((Address - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Address: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t EE_GetBankNumber(uint32_t Address)
{
  uint32_t bank;


  bank = FLASH_BANK_1;

  return bank;
}

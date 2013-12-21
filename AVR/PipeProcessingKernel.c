/*
 * PipeProcessingKernel.c
 *
 * Created: 03/06/2013 16:36:21
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 
 #include "stm32f10x.h"
#include "std_defs.h"
#include "MCU_Initialization.h"
#include "JobPipe_Module.h"
#include "Generic_Module.h"
#include "ChainProtocol_Module.h"
#include "USBProtocol_Module.h"
#include "A2D_Module.h"
#include "ASIC_Engine.h"
//#include <avr32/io.h>
#include "HostInteractionProtocols.h"
#include "AVR32X/AVR32_Module.h"
#include "AVR32_OptimizedTemplates.h"
#include "PipeProcessingKernel.h"
#include "bf_peripheral_timer.h"

#include <string.h>
#include <stdio.h>

// Information about the result we're holding
extern buf_job_result_packet JobResultFifo[PIPE_MAX_BUFFER_DEPTH];
extern job_packet NewJobFifo[PIPE_MAX_BUFFER_DEPTH];
extern unsigned int GLOBAL_LastJobResultProduced;
extern SOFT_TIMER ChipSoftTimer[TOTAL_CHIPS_INSTALLED];
extern __CHIP_PROCESSING_STATUS ChipMiningStatus[TOTAL_CHIPS_INSTALLED];
char _prev_job_was_from_pipe = FALSE;



// Global Variables
job_packet    jpActiveJobBeingProcessed;
unsigned int  iTotalEnginesFinishedActiveJob = 0;
unsigned int  iNonceListForActualJob[8] = {0,0,0,0,0,0,0,0};
unsigned char iNonceListForActualJob_Count = 0;
		
job_packet	 jpInQueueJob;
unsigned int  iTotalEnginesTakenInQueueJob = 0;
unsigned char bInQueueJobExists = FALSE;

// Our Activity Map
unsigned int  sEnginesActivityMap[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16 chips Max

/*
char PipeKernel_WasPreviousJobFromPipe(void)
{
	return _prev_job_was_from_pipe;
}*/		
		





void PipeKernel_Spin()
{
	// Run the process for all available chips on board
	u8 cChip;

	for (cChip = 0; cChip < TOTAL_CHIPS_INSTALLED; cChip++)
	{
		if (!CHIP_EXISTS(cChip)) continue;
		Flush_buffer_into_single_chip(cChip);
		
	}		
}	

void Flush_buffer_into_single_chip(u8 iChip)
{
	__CHIP_WORKING_STATE TempChipStatus;
	job_packet * TempJobPacket;

	if(ChipMiningStatus[iChip].ChipState == MINING)
	{
		TempChipStatus = ASIC_get_job_status(iChip);
		if((TempChipStatus == DONE_HAVE_NONCE) || (TempChipStatus == DONE_NO_NONCE))
		{
			if(cJobResultFifoFullFlag() == FALSE)
			{
				JobPipe_AddJobResult2Fifo(iChip);
				ChipMiningStatus[iChip].ChipState = IDLE;
			}
			else
			{
				ChipMiningStatus[iChip].ChipState = MINING;	//result FIFO is full, we need to wait till next round
			}
		}
		else if(TempChipStatus == IDLE)
		{
			ChipMiningStatus[iChip].ChipState = IDLE;
		}
		else
		{
			;	//Chip is mining, do nothing
		}
	}

	if(ChipMiningStatus[iChip].ChipState == IDLE)
	{
		if(cNewJobFifoEmptyFlag() != TRUE)
		{
			TempJobPacket = JobPipe_FetchNewJobFromFifo();
			memcpy((void*)(ChipMiningStatus[iChip].cMidstate), (void*)(TempJobPacket->midstate), SHA_MIDSTATE_SIZE);
			memcpy((void*)(ChipMiningStatus[iChip].cBlockdata), (void*)(TempJobPacket->block_data), SHA_BLOCKDATA_SIZE);
			ChipMiningStatus[iChip].ChipState = MINING;
			ChipMiningStatus[iChip].cEngineDoneCount = 0;
			ChipMiningStatus[iChip].cNonceCount = 0;
			ASIC_job_issue(iChip);
		}
	}
			// Update timestamp on the last job result produced
			GLOBAL_LastJobResultProduced = MACRO_GetTickCountRet;
}



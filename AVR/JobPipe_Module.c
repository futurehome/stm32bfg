#include "stm32f10x.h"
#include "JobPipe_Module.h"
#include "std_defs.h"
#include <stdio.h>
#include <string.h>
#include "bf_general.h"
#include "bf_peripheral_uart.h"
#include "bf_peripheral_timer.h"
#include "HostInteractionProtocols.h"


/*
 * JobPipe_Module.c
 *
 * Created: 20/12/2013 
 *  Author: Jiang Jun
 * Company: 
 * Breaf:  the flow of job assignment and result feedback are all rewrite.
 */ 


extern __CHIP_PROCESSING_STATUS ChipMiningStatus[TOTAL_CHIPS_INSTALLED];
extern unsigned int GLOBAL_ResBufferCompilationLatency;
extern unsigned char __aux_CharMap[];

// Information about the result we've done, change it to loop FIFO
buf_job_result_packet JobResultFifo[PIPE_MAX_BUFFER_DEPTH];
u8	cJobResultFifoStart;
u8	cJobResultFifoEnd;
job_packet NewJobFifo[PIPE_MAX_BUFFER_DEPTH];
u8	cNewJobFifoStart;
u8	cNewJobFifoEnd;

const u8 cChipIdle_NoResult_String[] = "INPROCESS:0\nCOUNT:0\nOK\n";
const u8 cChipMining_NoResult_String[] = "INPROCESS:1\nCOUNT:0\nOK\n";
const u8 cChipMining_HaveResult_Presemble[] = "INPROCESS:1\nCOUNT:";
const u8 cChipIdle_HaveResult_Presemble[] = "INPROCESS:0\nCOUNT:";
__USB_RETURN_JOB_STATUS ReturnJobStatus;
u8 cUsbReturnJobString[1024];
u16 wUsbReturnJobStringEnd;
u8 cUsbReturnJobIsUnfinished;
u8 cUsbHaveReturnJobs;
u16 wUsbReturnJobLeftStringPointer;

unsigned int	__total_buf_pipe_jobs_ever_received;




void JobPipe_init(void)
{
	u8 cChip;

	__total_buf_pipe_jobs_ever_received = 0;

	//reset all chip state to idle
	for(cChip=0; cChip<TOTAL_CHIPS_INSTALLED; cChip++)
	{
		ChipMiningStatus[cChip].ChipState = IDLE;
	}
	//reset start&end pointer to zero, means no result in buffer
	cJobResultFifoStart = 0;
	cJobResultFifoEnd	= 0;
	//reset job buffer pointer start&end to zero, means no job exist
	cNewJobFifoStart = 0;
	cNewJobFifoEnd = 0;
	ReturnJobStatus = ChipIdle_NoResult;
	cUsbReturnJobIsUnfinished = FALSE;
	cUsbHaveReturnJobs = FALSE;
	//"INPROCESS:0\nCOUNT:"
	cUsbReturnJobString[0] = 'I';
	cUsbReturnJobString[1] = 'N';
	cUsbReturnJobString[2] = 'P';
	cUsbReturnJobString[3] = 'R';
	cUsbReturnJobString[4] = 'O';
	cUsbReturnJobString[5] = 'C';
	cUsbReturnJobString[6] = 'E';
	cUsbReturnJobString[7] = 'S';
	cUsbReturnJobString[8] = 'S';
	cUsbReturnJobString[9] = ':';
	cUsbReturnJobString[10] = '0';
	cUsbReturnJobString[11] = '\n';
	cUsbReturnJobString[12] = 'C';
	cUsbReturnJobString[13] = 'O';
	cUsbReturnJobString[14] = 'U';
	cUsbReturnJobString[15] = 'N';
	cUsbReturnJobString[16] = 'T';
	cUsbReturnJobString[17] = ':';
	cUsbReturnJobString[18] = '0';
	wUsbReturnJobStringEnd = 20;		//need to skip 2 bytes of  job count char and RETURN char
}

void JobPipe__pipe_flush_buffer(void)
{
	//In this fuction, I guess clear job results and new jobs in buffer,   changed by JJ
	cJobResultFifoStart = 0;
	cJobResultFifoEnd	= 0;
	cNewJobFifoEnd = 0;
	cNewJobFifoStart = 0;
}

u8 cNewJobFifoFullFlag(void)
{
	u8 cFullFlag = FALSE;
	
	if(cNewJobFifoEnd > cNewJobFifoStart)
	{
		if((cNewJobFifoEnd - cNewJobFifoStart) == PIPE_MAX_BUFFER_DEPTH - 1)
		{
			cFullFlag = TRUE;
		}
	}
	else if(cNewJobFifoEnd < cNewJobFifoStart)
	{
		if((cNewJobFifoStart - cNewJobFifoEnd) == 1)
		{
			cFullFlag = TRUE;
		}
	}
	else
	{
		;
	}

	return cFullFlag;
}

u8 cNewJobFifoEmptyFlag(void)
{
	u8 cEmptyFlag = FALSE;
	
	if(cNewJobFifoEnd == cNewJobFifoStart)
	{
		cEmptyFlag = TRUE;
	}
	else
	{
		;
	}

	return cEmptyFlag;
}

u8 cJobResultFifoFullFlag(void)
{
	u8 cFullFlag = FALSE;
	
	if(cJobResultFifoEnd > cJobResultFifoStart)
	{
		if((cJobResultFifoEnd - cJobResultFifoStart) == PIPE_MAX_BUFFER_DEPTH - 1)
		{
			cFullFlag = TRUE;
		}
	}
	else if(cJobResultFifoEnd < cJobResultFifoStart)
	{
		if((cJobResultFifoStart - cJobResultFifoEnd) == 1)
		{
			cFullFlag = TRUE;
		}
	}
	else
	{
		;
	}

	return cFullFlag;
}

u8 cJobResultFifoEmptyFlag(void)
{
	u8 cEmptyFlag = FALSE;
	
	if(cJobResultFifoEnd == cJobResultFifoStart)
	{
		cEmptyFlag = TRUE;
	}
	else
	{
		;
	}

	return cEmptyFlag;
}


void JobPipe_AddNewJob2Fifo(job_packet* _NewJobPacket)
{
	//ComTransmitData(COM2, ",P",2);

	// Copy new job to the current end pointer
	memcpy((void*)(&(NewJobFifo[cNewJobFifoEnd])),(void*)_NewJobPacket, sizeof(job_packet));

	// Increase the end pointer of new jobs fifo
	cNewJobFifoEnd ++;
	if(cNewJobFifoEnd == PIPE_MAX_BUFFER_DEPTH)
	{
		cNewJobFifoEnd = 0;
	}
}

job_packet* JobPipe_FetchNewJobFromFifo(void)
{
	job_packet* FetchedNewJob;
	//ComTransmitData(COM2, ",p",2);

	FetchedNewJob = (job_packet*)(&(NewJobFifo[cNewJobFifoStart]));

	// Shift the start pointer to the next new job
	cNewJobFifoStart ++;
	if(cNewJobFifoStart == PIPE_MAX_BUFFER_DEPTH)
	{
		cNewJobFifoStart = 0;
	}

	// Proceed... All is ok!
	return FetchedNewJob;
}

void JobPipe_AddJobResult2Fifo(u8 cChip)
{
	//ComTransmitData(COM2, ",P",2);

	// Append new job result to the current end pointer
	memcpy((void*)JobResultFifo[cJobResultFifoEnd].midstate, (void*)ChipMiningStatus[cChip].cMidstate, SHA_MIDSTATE_SIZE);
	memcpy((void*)JobResultFifo[cJobResultFifoEnd].block_data, (void*)ChipMiningStatus[cChip].cBlockdata, SHA_BLOCKDATA_SIZE);
	JobResultFifo[cJobResultFifoEnd].iProcessingChip = cChip;
	JobResultFifo[cJobResultFifoEnd].i_nonce_count = ChipMiningStatus[cChip].cNonceCount;
	memcpy((void*)JobResultFifo[cJobResultFifoEnd].nonce_list, (void*)ChipMiningStatus[cChip].dwNonceList, MAX_NONCE_IN_RESULT * sizeof(u32));

	
	// Increase the end pointer of job result fifo
	cJobResultFifoEnd ++;
	if(cJobResultFifoEnd == PIPE_MAX_BUFFER_DEPTH)
	{
		cJobResultFifoEnd = 0;
	}
}

buf_job_result_packet* JobPipe_FetchJobResultFromFifo(void)
{
	buf_job_result_packet * FetchedJobResult;

	//ComTransmitData(COM2, ",p",2);

	FetchedJobResult = (buf_job_result_packet*)(&(JobResultFifo[cJobResultFifoStart]));

	// Shift the start pointer to the next job result
	cJobResultFifoStart ++;
	if(cJobResultFifoStart == PIPE_MAX_BUFFER_DEPTH)
	{
		cJobResultFifoStart = 0;
	}

	// Proceed... All is ok!
	return FetchedJobResult;
}

u8 JobPipe_GetNewJobFifoAvailableSpace(void)
{
	u8 cAvailableSpace;

	if(cNewJobFifoEnd > cNewJobFifoStart)
	{
		cAvailableSpace = cNewJobFifoStart - cNewJobFifoEnd + (PIPE_MAX_BUFFER_DEPTH - 1);
	}
	else if(cNewJobFifoEnd < cNewJobFifoStart)
	{
		cAvailableSpace = cNewJobFifoStart - cNewJobFifoEnd - 1;
	}
	else
	{
		cAvailableSpace = PIPE_MAX_BUFFER_DEPTH - 1;
	}

	return cAvailableSpace;
}

u8 JobPipe_GetJobResultCountInFifo(void)
{
	u8 cResultCount;

	if(cJobResultFifoEnd > cJobResultFifoStart)
	{
		cResultCount = cJobResultFifoEnd - cJobResultFifoStart;
	}
	else if(cJobResultFifoEnd < cJobResultFifoStart)
	{
		cResultCount = cJobResultFifoEnd + (PIPE_MAX_BUFFER_DEPTH - 1) - cJobResultFifoStart;
	}
	else
	{
		cResultCount = 0;
	}

	return cResultCount;
}

//calculate the new job number in FIFO
u8 JobPipe_GetNewJobCountInFifo(void)
{
	u8 cResultCount;

	if(cNewJobFifoEnd > cNewJobFifoStart)
	{
		cResultCount = cNewJobFifoEnd - cNewJobFifoStart;
	}
	else if(cNewJobFifoEnd < cNewJobFifoStart)
	{
		cResultCount = cNewJobFifoEnd + (PIPE_MAX_BUFFER_DEPTH - 1) - cNewJobFifoStart;
	}
	else
	{
		cResultCount = 0;
	}

	return cResultCount;
}

//In this function, check job result buffer and usb job feedback buffer status
//If there are results and usb feedback buffer is empty, then convert job result to feedback string,
//at last, set flag to indicate may response result to USB ZOX command
void JobPipe_ConvertJobResult2UsbStringBuffer(void)
{
	u8 i, j, k;
	u8 cJobResultCount;
	buf_job_result_packet* JobResult;
	//u8 cPreviousJobResultCount;
	//u8 cTempChar;
	
	GLOBAL_ResBufferCompilationLatency = MACRO_GetTickCountRet;
	
	if(cUsbReturnJobIsUnfinished != TRUE)			//ZOX unfinished, just waiting and do nothing
	{
		cJobResultCount = JobPipe_GetJobResultCountInFifo();
		if(cJobResultCount != 0)	//if=0,there is no result, do nothing, just return
		{
			//we need to check return string buffer, if there are some result strings, the new job
			//result should append at end, and increase result count, be careful!  the string length
			//should be within the buffer size
			//cPreviousJobResultCount = cUsbReturnJobString[18];
			if((ReturnJobStatus != Mining_HaveResult) && (ReturnJobStatus != ChipIdle_HaveResult))
			{
				if(cJobResultCount > MAX_RESULTS_TO_SEND_AT_A_TIME_FROM_BUFFER)
				{
					cJobResultCount = MAX_RESULTS_TO_SEND_AT_A_TIME_FROM_BUFFER;
				}

				//convert refreshed job count to ascii charactor
				cUsbReturnJobString[18] = cJobResultCount | 0x30; 
				cUsbReturnJobString[19] = '\n';			//should be RETURN char
				for(i=0; i<cJobResultCount; i++)
				{
					JobResult = JobPipe_FetchJobResultFromFifo();
					//convert and store midstate
					for (j = 0; j < 32; j++)
					{
						cUsbReturnJobString[wUsbReturnJobStringEnd] = _AUX_LEFT_HEX(JobResult->midstate[j]);
						wUsbReturnJobStringEnd ++;
						cUsbReturnJobString[wUsbReturnJobStringEnd] = _AUX_RIGHT_HEX(JobResult->midstate[j]);
						wUsbReturnJobStringEnd ++;
					}
					//adding a comma
					cUsbReturnJobString[wUsbReturnJobStringEnd] = ',';
					wUsbReturnJobStringEnd ++;
					//convert and store block data
					for (j = 0; j < 12; j++)
					{
						cUsbReturnJobString[wUsbReturnJobStringEnd] = _AUX_LEFT_HEX(JobResult->block_data[j]);
						wUsbReturnJobStringEnd ++;
						cUsbReturnJobString[wUsbReturnJobStringEnd] = _AUX_RIGHT_HEX(JobResult->block_data[j]);
						wUsbReturnJobStringEnd ++;
					}
					//adding a comma
					cUsbReturnJobString[wUsbReturnJobStringEnd] = ',';
					wUsbReturnJobStringEnd ++;
					//adding chip number
					cUsbReturnJobString[wUsbReturnJobStringEnd] =  __aux_CharMap[JobResult->iProcessingChip];
					wUsbReturnJobStringEnd ++;
					//adding a comma
					cUsbReturnJobString[wUsbReturnJobStringEnd] = ',';
					wUsbReturnJobStringEnd ++;
					//adding nonce count
					cUsbReturnJobString[wUsbReturnJobStringEnd] =  __aux_CharMap[JobResult->i_nonce_count];
					wUsbReturnJobStringEnd ++;
					//adding nonce list
					if(JobResult->i_nonce_count > 0)
					{
						for(k=0; k<JobResult->i_nonce_count; k++)
						{
							//adding a comma
							cUsbReturnJobString[wUsbReturnJobStringEnd] = ',';
							wUsbReturnJobStringEnd ++;
							//adding a nonce
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 28) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 24) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 20) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 16) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 12) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 8) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k] >> 4) & 0x0F];
							wUsbReturnJobStringEnd ++;
							cUsbReturnJobString[wUsbReturnJobStringEnd] = __aux_CharMap[(JobResult->nonce_list[k]) & 0x0F];
							wUsbReturnJobStringEnd ++;
						}
					}
					else    //nonce count = 0, do nothing
					{
						;
					}
					cUsbReturnJobString[wUsbReturnJobStringEnd] = '\n';
					wUsbReturnJobStringEnd ++;
				}
				cUsbReturnJobString[wUsbReturnJobStringEnd] = 'O';
				wUsbReturnJobStringEnd ++;
				cUsbReturnJobString[wUsbReturnJobStringEnd] = 'K';
				wUsbReturnJobStringEnd ++;
				cUsbReturnJobString[wUsbReturnJobStringEnd] = '\n';
				wUsbReturnJobStringEnd ++;
				cUsbReturnJobString[wUsbReturnJobStringEnd] = 0;
				wUsbReturnJobStringEnd ++;
			}

			//according to chip state: mining or idle, and result count, set corresponding presemble
			ReturnJobStatus = ChipIdle_HaveResult;
			cUsbReturnJobString[10] = '0';
			for(i=0; i<TOTAL_CHIPS_INSTALLED; i++)
			{
				if(ChipMiningStatus[i].ChipState == MINING)
				{
					ReturnJobStatus = Mining_HaveResult;
					cUsbReturnJobString[10] = '1';
					break;
				}
			}
		}
		else
		{
			/*
			ReturnJobStatus = ChipIdle_NoResult;
			for(i=0; i<TOTAL_CHIPS_INSTALLED; i++)
			{
				if(ChipMiningStatus[i].ChipState == MINING)
				{
					ReturnJobStatus = Mining_NoResult;
					break;
				}
			}*/
		}
	}

	GLOBAL_ResBufferCompilationLatency = MACRO_GetTickCountRet - GLOBAL_ResBufferCompilationLatency;
}



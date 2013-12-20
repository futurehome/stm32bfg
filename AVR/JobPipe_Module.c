#include "stm32f10x.h"
#include "JobPipe_Module.h"
#include "std_defs.h"
#include <stdio.h>
#include <string.h>
#include "bf_general.h"
#include "bf_peripheral_uart.h"

/*
 * JobPipe_Module.c
 *
 * Created: 08/10/2012 21:39:04
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 

////////////////////////////////////////////////////////////////////////////////
/// This is our Pipelined job processing system 
////////////////////////////////////////////////////////////////////////////////

// Job interleaving
//char __interleaved_was_last_job_loaded_into_engines;
//char __interleaved_loading_progress_chip;
//char __interleaved_loading_progress_engine;
//char __interleaved_loading_progress_finished;

// Information about the result we've done, change it to loop FIFO
buf_job_result_packet JobResultFifo[PIPE_MAX_BUFFER_DEPTH];
u8	cJobResultFifoStart;
u8	cJobResultFifoEnd;
//char __buf_job_results_count;  // Total of results in our __buf_job_results
// New Job buffer,   I've change it to loop FIFO
job_packet NewJobFifo[PIPE_MAX_BUFFER_DEPTH];
u8	cNewJobFifoStart;
u8	cNewJobFifoEnd;

//Chip status,   this buffer is used for processing jobs
extern __CHIP_PROCESSING_STATUS ChipMiningStatus[TOTAL_CHIPS_INSTALLED];

//u8  __inprocess_SCQ_chip_processing[TOTAL_CHIPS_INSTALLED];
//u8  __inprocess_SCQ_midstate[TOTAL_CHIPS_INSTALLED][32];
//u8  __inprocess_SCQ_blockdata[TOTAL_CHIPS_INSTALLED][16];

// In process information
//char   __inprocess_midstate[32];
//char   __inprocess_blockdata[12];

//unsigned int    __total_jobs_in_buffer;
unsigned int	__total_buf_pipe_jobs_ever_received;




void JobPipe_init(void)
{
	u8 cChip;
	// Initialize our buffer
	//__total_jobs_in_buffer = 0;
	__total_buf_pipe_jobs_ever_received = 0;

	// Put zeros everywhere
	//tx = 0;
	//for (tx = 0; tx < sizeof(__inprocess_midstate); tx++) __inprocess_midstate[tx] = 0;
	//for (tx = 0; tx < sizeof(__inprocess_blockdata); tx++) __inprocess_blockdata[tx] = 0;
	//for (tx = 0; tx < sizeof(__inprocess_SCQ_chip_processing); tx++) __inprocess_SCQ_chip_processing[tx] = 0;

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
	
	// Reset results...
	//__buf_job_results_count = 0;
	
	// Clear interleaving
	//__interleaved_loading_progress_chip = FALSE;
	//__interleaved_loading_progress_engine = FALSE;
	//__interleaved_loading_progress_finished = FALSE;
	//__interleaved_was_last_job_loaded_into_engines = FALSE;
}

void JobPipe__pipe_flush_buffer(void)
{
	/*
	// simply reset its counter;
	#if defined(FLUSH_CLEAR_RESULTS_BUFFER)
		__buf_job_results_count = 0; // NOTE: CHANGED ON REQUEUST, DO NOT LOOSE JOB RESULTS!
	#endif
	__total_jobs_in_buffer	= 0;
	
	// Interleaved reset...
	__interleaved_was_last_job_loaded_into_engines = FALSE;
	__interleaved_loading_progress_finished = FALSE;
	__interleaved_loading_progress_engine = FALSE;
	__interleaved_loading_progress_chip = FALSE;	
	*/

	//In this fuction, I guess only clear job results in buffer,   changed by JJ
	cJobResultFifoStart = 0;
	cJobResultFifoEnd	= 0;
	cNewJobFifoEnd = 0;
	cNewJobFifoStart = 0;
}

/*
char JobPipe__pipe_ok_to_pop(void)
{
	return ((__total_jobs_in_buffer > 0) ? 1 : 0);
}

char JobPipe__available_space(void)
{
	// simply reset its counter;
	return ((__total_jobs_in_buffer > PIPE_MAX_BUFFER_DEPTH) ? 0 : (PIPE_MAX_BUFFER_DEPTH - __total_jobs_in_buffer));
}

char JobPipe__pipe_ok_to_push()
{
	return ((__total_jobs_in_buffer < PIPE_MAX_BUFFER_DEPTH) ? 1 : 0);
}

char	JobPipe__total_jobs_in_pipe()
{
	return __total_jobs_in_buffer;
}*/

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
	ComTransmitData(COM2, ",P",2);

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
	ComTransmitData(COM2, ",p",2);

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
	ComTransmitData(COM2, ",P",2);

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

	ComTransmitData(COM2, ",p",2);

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

/*
char JobPipe__pipe_push_job(void* __input_pipe_job_info)
{
	// Is it ok to push a job into stack?
	if (!JobPipe__pipe_ok_to_push()) return PIPE_JOB_BUFFER_FULL;
	ComTransmitData(COM2, ",P",2);
	// Copy memory block
	memcpy((void*)((char*)(PIPE_PROC_BUF) + (__total_jobs_in_buffer * sizeof(job_packet))),
		   __input_pipe_job_info, sizeof(job_packet));

	// Increase the total jobs available in the stack
	__total_jobs_in_buffer++;

	// Proceed... All is ok!
	return PIPE_JOB_BUFFER_OK;
}*/

/*
char JobPipe__pipe_pop_job(void* __output_pipe_job_info)
{
	char tx;
	// Is it ok to pop a job from the stack?
	if (!JobPipe__pipe_ok_to_pop()) return PIPE_JOB_BUFFER_EMPTY;

	ComTransmitData(COM2, ",p",2);
	// Copy memory block (from element 0)
	memcpy(__output_pipe_job_info,
		  (void*)((char*)(PIPE_PROC_BUF) + (0 * sizeof(job_packet))),
		  sizeof(job_packet));

	// Shift all elements back
	tx = 0;
	for (tx = 1; tx < __total_jobs_in_buffer; tx++)
	{
		memcpy((void*)((char*)(PIPE_PROC_BUF) + ((tx - 1) * sizeof(job_packet))),
			   (void*)((char*)(PIPE_PROC_BUF) + (tx * sizeof(job_packet))),
			   sizeof(job_packet));
	}

	// Reduce total of jobs available in the stack
	__total_jobs_in_buffer--;

	// Proceed... All is ok!
	return PIPE_JOB_BUFFER_OK;
}*/

/*
char JobPipe__pipe_preview_next_job(void* __output_pipe_job_info)
{
	// If no job is in the buffer, don't do anything...
	if (__total_jobs_in_buffer == 0) return PIPE_JOB_BUFFER_EMPTY;
	
	// Copy memory block (from element 0)
	memcpy(__output_pipe_job_info,
		   (void*)((char*)(PIPE_PROC_BUF)),
		   sizeof(job_packet));

	// Proceed... All is ok!
	return PIPE_JOB_BUFFER_OK;	
}*/

/*
void* JobPipe__pipe_get_buf_job_result(unsigned int iIndex)
{
	return (void*)&__buf_job_results[iIndex];
}
*/
/*
void  JobPipe__pipe_skip_buf_job_results(unsigned int iTotalToSkip)
{
	char iTotalActualResults;
	char umr;
	if (iTotalToSkip == 0) return; // Nothing special to do really...
		
	// Flush these amount of jobs from results
	iTotalActualResults = __buf_job_results_count;
	if (iTotalToSkip >= iTotalActualResults)
	{
		// Just clear the results buffer
		__buf_job_results_count = 0;
		return;
	}
	
	// Copy the results backward	
	for (umr = iTotalToSkip; umr < iTotalActualResults; umr++)
	{
		memcpy(&__buf_job_results[umr - iTotalToSkip],
			   (void*)(&__buf_job_results[umr]),
			   sizeof(buf_job_result_packet));				   
	}
	
	__buf_job_results_count -= iTotalToSkip;
}*/

/*
unsigned int JobPipe__pipe_get_buf_job_results_count(void)
{
	return __buf_job_results_count;
}

void JobPipe__pipe_set_buf_job_results_count(unsigned int iCount)
{
	__buf_job_results_count = iCount;
}

void JobPipe__set_was_last_job_loaded_in_engines(char iVal)
{
	__interleaved_was_last_job_loaded_into_engines = iVal;
}

char JobPipe__get_was_last_job_loaded_in_engines()
{
	return __interleaved_was_last_job_loaded_into_engines;
}
*/
/*
void JobPipe__set_interleaved_loading_progress_chip(char iVal)
{
	__interleaved_loading_progress_chip = iVal;
}

char JobPipe__get_interleaved_loading_progress_chip()				 
{
	return __interleaved_loading_progress_chip;
}

void JobPipe__set_interleaved_loading_progress_engine(char iVal)
{
	__interleaved_loading_progress_engine = iVal;
}

char JobPipe__get_interleaved_loading_progress_engine()
{
	return __interleaved_loading_progress_engine;
}

void JobPipe__set_interleaved_loading_progress_finished(char iVal)
{
	__interleaved_loading_progress_finished = iVal;
}

char JobPipe__get_interleaved_loading_progress_finished()
{
	return __interleaved_loading_progress_finished;
}
*/

	
///////////////////
// TEST FUNCTIONS

/*
void JobPipe__test_buffer_shifter(void)
{
	char pIndex;
	// Then move all items one-index back (resulting in loss of the job-result at index 0)
	for (pIndex = 0; pIndex < PIPE_MAX_BUFFER_DEPTH - 1; pIndex += 1) // PIPE_MAX_BUFFER_DEPTH - 1 because we don't touch the last item in queue
	{
		memcpy((void*)__buf_job_results[pIndex].midstate, 	(void*)__buf_job_results[pIndex+1].midstate, 	32);
		memcpy((void*)__buf_job_results[pIndex].block_data, (void*)__buf_job_results[pIndex+1].block_data, 	12);
		memcpy((void*)__buf_job_results[pIndex].nonce_list,	(void*)__buf_job_results[pIndex+1].nonce_list,  8*sizeof(u32)); // 8 nonces maximum
	}	
}*/


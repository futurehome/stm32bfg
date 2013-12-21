/*
 * JobPipe_Module.h
 *
 * Created: 08/10/2012 21:38:56
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 
#ifndef JOBPIPE_MODULE_H_
#define JOBPIPE_MODULE_H_

#include "std_defs.h"

// Job structure...
#define SHA256_Test_String  "2f33a240bd6785caed5b67b4122079dd9359004ae23c64512c5c2dfbce097b08"


/// ************************** This is our Pipelined job processing system (holder of 2 jobs + 1 process)
#define PIPE_MAX_BUFFER_DEPTH	 40
#define PIPE_JOB_BUFFER_OK		 0
#define PIPE_JOB_BUFFER_FULL	 1
#define PIPE_JOB_BUFFER_EMPTY	 2

#define MAX_RESULTS_TO_SEND_AT_A_TIME_FROM_BUFFER 4   //because reserved ONLY ONE byte to save result count, the MAX should not exceed 9!!!
														//because of tight SRAM, only permit max =5

typedef enum
{
	ChipIdle_NoResult = 0,
	Mining_NoResult,
	Mining_HaveResult,
	ChipIdle_HaveResult
}__USB_RETURN_JOB_STATUS;

void 	JobPipe_init(void);
void 	JobPipe__pipe_flush_buffer(void);
u8 		cNewJobFifoFullFlag(void);
u8 		cNewJobFifoEmptyFlag(void);
u8 		cJobResultFifoFullFlag(void);
u8 		cJobResultFifoEmptyFlag(void);
void 	JobPipe_AddNewJob2Fifo(job_packet* _NewJobPacket);
job_packet* JobPipe_FetchNewJobFromFifo(void);
void 	JobPipe_AddJobResult2Fifo(u8 cChip);
buf_job_result_packet* JobPipe_FetchJobResultFromFifo(void);
u8 		JobPipe_GetNewJobFifoAvailableSpace(void);
u8 		JobPipe_GetJobResultCountInFifo(void);
u8 		JobPipe_GetNewJobCountInFifo(void);
void 	JobPipe_ConvertJobResult2UsbStringBuffer(void);















/*
char			JobPipe__available_space(void);
char			JobPipe__pipe_ok_to_push(void);
char			JobPipe__pipe_ok_to_pop(void);
char			JobPipe__pipe_push_job(void* __input_job_info);
char			JobPipe__pipe_pop_job (void* __output_job_info);
char			JobPipe__total_jobs_in_pipe(void);
void*			JobPipe__pipe_get_buf_job_result(unsigned int iIndex);
unsigned int	JobPipe__pipe_get_buf_job_results_count(void);
void			JobPipe__pipe_set_buf_job_results_count(unsigned int iCount);
void			JobPipe__pipe_skip_buf_job_results(unsigned int iTotalToSkip);
void			JobPipe__pipe_flush_buffer(void);
char			JobPipe__pipe_preview_next_job(void* __output_pipe_job_info);

// Set-Reset settings
char			JobPipe__get_was_last_job_loaded_in_engines(void);
void			JobPipe__set_was_last_job_loaded_in_engines(char iVal);
void			JobPipe__set_interleaved_loading_progress_chip(char iVal);
char			JobPipe__get_interleaved_loading_progress_chip(void);
void			JobPipe__set_interleaved_loading_progress_engine(char iVal);
char			JobPipe__get_interleaved_loading_progress_engine(void);
void			JobPipe__set_interleaved_loading_progress_finished(char iVal);
char			JobPipe__get_interleaved_loading_progress_finished(void);

// Functio made for performace test reasons
void			JobPipe__test_buffer_shifter(void);*/

#endif /* MCU_INITIALIZATION_H_ */


/*
 * PipeProcessingKernel.h
 *
 * Created: 03/06/2013 16:36:07
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 

#ifndef PIPEPROCESSINGKERNEL_H_
#define PIPEPROCESSINGKERNEL_H_

//char PipeKernel_WasPreviousJobFromPipe(void);
void PipeKernel_Spin(void);
void Flush_buffer_into_single_chip(u8 iChip);

#endif /* PIPEPROCESSINGKERNEL_H_ */

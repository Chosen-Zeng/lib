#include "AS5047P.h"
#include "TIM.h"
#include <stdlib.h>

#if defined AS5047P_NUM

AS5047P_t AS5047P[AS5047P_NUM];
unsigned short AS5047P_RxData[AS5047P_NUM];
AS5047P_msg_node *AS5047P_node_recv[AS5047P_NUM], *AS5047P_node_head[AS5047P_NUM], // AS5047P_node_head[] is meaningful only at first recv
    *AS5047P_node_send[AS5047P_NUM], *AS5047P_node_tail[AS5047P_NUM];

#define AS5047P_TYPE_CMD_R 0
#define AS5047P_TYPE_CMD_W 1
#define AS5047P_TYPE_DATA 2

static struct
{
    unsigned char ERRFL_msg : 1;    // ERRFL msg in FIFO
    unsigned char PROGEN_check : 1; // PROGEN set
    unsigned char PROG_msg : 1;     // PROG msg in FIFO
} AS5047P_state_bit[AS5047P_NUM];

static inline unsigned char ParityCalc(unsigned short data)
{
    unsigned char cnt = 16, ret = 0;
    while (cnt--)
    {
        ret ^= data >> cnt;
    }

    return ret & 1;
}

// @attention all msg should be added in one time, or exception may occur due to msg added previously being sent already
void AS5047P_FIFO_Enqueue(unsigned char AS5047P_ID, unsigned short AS5047P_msg)
{
    // not empty FIFO
    if (AS5047P_node_tail[AS5047P_ID])
    {
        // create new node
        AS5047P_node_tail[AS5047P_ID]->next = malloc(sizeof(AS5047P_msg_node));
        AS5047P_node_tail[AS5047P_ID]->next->next = NULL;
        AS5047P_node_tail[AS5047P_ID]->next->prev = AS5047P_node_tail[AS5047P_ID];
        AS5047P_node_tail[AS5047P_ID] = AS5047P_node_tail[AS5047P_ID]->next;

        // AS5047P_node_send[AS5047P_ID] point at new msg
        if (!AS5047P_node_send[AS5047P_ID])
            AS5047P_node_send[AS5047P_ID] = AS5047P_node_tail[AS5047P_ID];
    }
    // empty FIFO
    else
    {
        // create new node
        AS5047P_node_head[AS5047P_ID] = AS5047P_node_send[AS5047P_ID] = AS5047P_node_tail[AS5047P_ID] = malloc(sizeof(AS5047P_msg_node)); // recv node is null initially
        AS5047P_node_tail[AS5047P_ID]->prev = AS5047P_node_tail[AS5047P_ID]->next = NULL;
    }
    AS5047P_node_tail[AS5047P_ID]->msg = AS5047P_msg;

    // FIFO elmt > 1
    if (AS5047P_node_tail[AS5047P_ID]->prev) // node is created above, ensuring the existence of AS5047P_node_tail[AS5047P_ID]
        switch (AS5047P_node_tail[AS5047P_ID]->prev->type)
        {
        case AS5047P_TYPE_CMD_R:
        case AS5047P_TYPE_DATA:
        {
            AS5047P_node_tail[AS5047P_ID]->type = AS5047P_msg & 0x4000 ? AS5047P_TYPE_CMD_R : AS5047P_TYPE_CMD_W;

            break;
        }
        case AS5047P_TYPE_CMD_W:
        {
            AS5047P_node_tail[AS5047P_ID]->type = AS5047P_TYPE_DATA;

            break;
        }
        }
    // FIFO elmt == 1
    else
        AS5047P_node_tail[AS5047P_ID]->type = AS5047P_msg & 0x4000 ? AS5047P_TYPE_CMD_R : AS5047P_TYPE_CMD_W;
}

// @note node to dequeue is pointed at by AS5047P_node_recv[AS5047P_ID]
void AS5047P_FIFO_Dequeue(unsigned char AS5047P_ID)
{
    // not empty FIFO
    if (AS5047P_node_recv[AS5047P_ID])
    {
        // FIFO elmt > 1
        if (AS5047P_node_recv[AS5047P_ID]->next)
        {
            AS5047P_node_recv[AS5047P_ID] = AS5047P_node_recv[AS5047P_ID]->next;
            free(AS5047P_node_recv[AS5047P_ID]->prev);
            AS5047P_node_recv[AS5047P_ID]->prev = NULL;
        }
        // FIFO elmt == 1
        else
        {
            free(AS5047P_node_recv[AS5047P_ID]);
            AS5047P_node_recv[AS5047P_ID] = AS5047P_node_send[AS5047P_ID] = AS5047P_node_tail[AS5047P_ID] = NULL;
        }
    }
    // initial recv
    else if (AS5047P_node_head[AS5047P_ID])
    {
        AS5047P_node_recv[AS5047P_ID] = AS5047P_node_head[AS5047P_ID];
        AS5047P_node_head[AS5047P_ID] = NULL;
    }
}

// @brief read specific reg
inline void AS5047P_FIFO_R(unsigned char AS5047P_ID, unsigned short AS5047P_reg)
{
    AS5047P_FIFO_Enqueue(AS5047P_ID, AS5047P_R | AS5047P_reg);
}

// @brief write specific reg
inline void AS5047P_FIFO_W(unsigned char AS5047P_ID, unsigned short AS5047P_reg, unsigned short data)
{
    AS5047P_FIFO_Enqueue(AS5047P_ID, AS5047P_W | AS5047P_reg);
    AS5047P_FIFO_Enqueue(AS5047P_ID, data);
}

// @brief inquire pos by default, or send msg in FIFO if not empty
// @exception mem exception occurs if interrupt not triggered; msg added after send function before IRQHandler cause sequence error
unsigned char AS5047P_SendFIFO(SPI_TypeDef *SPI_handle, unsigned char AS5047P_ID)
{
    static timer_t AS5047P_time_PROG, AS5047P_time_ERRFL;

    if (Timer_CheckTimeout(&AS5047P_time_PROG, 1) &&  // minimum check time: 1s
        AS5047P_state_bit[AS5047P_ID].PROGEN_check && // PROGEN should be set
        !AS5047P[AS5047P_ID].PROGEN &&                // PROGEN not set
        !AS5047P_state_bit[AS5047P_ID].PROG_msg)      // PROG msg not in FIFO
    {
        AS5047P_FIFO_R(AS5047P_ID, AS5047P_REG_PROG);

        AS5047P_state_bit[AS5047P_ID].PROG_msg = 1;
        AS5047P_time_PROG.curr = AS5047P_time_PROG.intvl = AS5047P_time_PROG.prev = 0; // clear timer
    }

    if (Timer_CheckTimeout(&AS5047P_time_ERRFL, 1) && // minimum check time: 1s
        AS5047P[AS5047P_ID].err &&                    // error bit
        !AS5047P_state_bit[AS5047P_ID].ERRFL_msg)     // ERRFL msg not in FIFO
    {
        AS5047P_FIFO_R(AS5047P_ID, AS5047P_REG_ERRFL);
        AS5047P_FIFO_R(AS5047P_ID, AS5047P_REG_DIAAGC);

        AS5047P_state_bit[AS5047P_ID].ERRFL_msg = 1;
        AS5047P_time_ERRFL.curr = AS5047P_time_ERRFL.intvl = AS5047P_time_ERRFL.prev = 0; // clear timer
    }

    // specific msg
    if (AS5047P_node_send[AS5047P_ID])
    {
        if (SPI_SendData(SPI_handle, ParityCalc(AS5047P_node_send[AS5047P_ID]->msg) << 15 | AS5047P_node_send[AS5047P_ID]->msg))
            return 1;

        AS5047P_node_send[AS5047P_ID] = AS5047P_node_send[AS5047P_ID]->next;
    }
    // common msg
    else if (SPI_SendData(SPI_handle, ParityCalc(AS5047P_R | AS5047P_REG_ANGLECOM) << 15 | AS5047P_R | AS5047P_REG_ANGLECOM))
        return 1;

    return 0;
}

void AS5047P_MsgDecode(unsigned char AS5047P_ID)
{
    // FIFO mode
    if (!ParityCalc(AS5047P_RxData[AS5047P_ID])) // verify data integrity
    {
        if (AS5047P_node_recv[AS5047P_ID] &&                           // specific msg
            AS5047P_node_recv[AS5047P_ID]->type == AS5047P_TYPE_CMD_R) // msg to recv is read
        {
            if (AS5047P_node_recv[AS5047P_ID]->msg == (AS5047P_R | AS5047P_REG_ERRFL))
                AS5047P[AS5047P_ID].err = AS5047P_state_bit->ERRFL_msg = 0;
            else
                AS5047P[AS5047P_ID].err = (AS5047P_RxData[AS5047P_ID] & 0x4000) >> 14; // check error bit

            switch (AS5047P_node_recv[AS5047P_ID]->msg & 0x3FFF) // match the reg
            {
            case AS5047P_REG_DIAAGC:
            {
                AS5047P[AS5047P_ID].MAGL = (AS5047P_RxData[AS5047P_ID] & 0x800) >> 11;
                AS5047P[AS5047P_ID].MAGH = (AS5047P_RxData[AS5047P_ID] & 0x400) >> 10;
                AS5047P[AS5047P_ID].AGC = AS5047P_RxData[AS5047P_ID] & 0xFF;

                break;
            }
            case AS5047P_REG_PROG:
            {
                AS5047P_state_bit->PROG_msg = 0;
                AS5047P[AS5047P_ID].PROGEN = AS5047P_RxData[AS5047P_ID];

                break;
            }
            }
        }
        // common msg
        else if (!AS5047P_node_recv[AS5047P_ID])
        {
            AS5047P[AS5047P_ID].err = (AS5047P_RxData[AS5047P_ID] & 0x4000) >> 14; // check error bit
            AS5047P[AS5047P_ID].pos = 360.f * (AS5047P_RxData[AS5047P_ID] & 0x3FFF) / (1 << 14);
        }
    }

    // update next recv msg info
    AS5047P_FIFO_Dequeue(AS5047P_ID);
}

// @brief prepare for non-volatile reg program
inline void AS5047P_ProgramInit(unsigned char AS5047P_ID)
{
    AS5047P_FIFO_W(AS5047P_ID, AS5047P_REG_PROG, 0x9);

    AS5047P_state_bit[AS5047P_ID].PROGEN_check = 1;
}

// @brief end of non-volatile reg program
inline void AS5047P_ProgramDone(unsigned char AS5047P_ID)
{
    AS5047P_FIFO_W(AS5047P_ID, AS5047P_REG_PROG, 0x44);

    AS5047P[AS5047P_ID].PROGEN = AS5047P_state_bit[AS5047P_ID].PROGEN_check = 0;
}

/*
void DMA1_Channel1_IRQHandler(void)
{
    DMA1->IFCR |= 0x2;

    AS5047P_MsgDecode(0);

    // disable SPI
    while (SPI1->SR & 0x1880)
        ;
    SPI1->CR1 &= 0xFFBF;
}

// for better recv exp, refer to DMA_IRQHandler
void SPI1_IRQHandler(void)
{
    AS5047P_RxData[0] = SPI1->RXDR;

    AS5047P_MsgDecode(0);

    // disable SPI
    while (SPI1->SR & 0x1880)
        ;
    SPI1->CR1 &= 0xFFBF;
}
*/
#endif
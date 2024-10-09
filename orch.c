/**
 * @brief   a simple "Beep" program that plays an Orchestra stab
 * @author  Thom Cherryhomes 
 * @email   thom dot cherryhomes at gmail dot com
 * @license GPL v. 3, see license.md for details.
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <libraries/dos.h>
#include <devices/audio.h>
#include "orch.h"

#define PRIORITY ADALLOC_MINPREC

#define MAXVOLUME 64
#define PERIODSIZE 324

#define CHANNEL0 1
#define CHANNEL1 2
#define CHANNEL2 4
#define CHANNEL3 8

struct IOAudio *audioIOB = 0;
struct MsgPort *port = 0;

UBYTE allocationMap[] = 
{
	15,15,15,15
};

UBYTE deviceOpened = FALSE;

extern struct MsgPort *CreatePort();

/**
 * @brief Initialize sound for playback
 */
init()
{
    deviceOpened = FALSE;

    /* Create our reply port. */
    port = (struct MsgPort *)CreatePort(NULL);
    if (!port)
    {
	return FALSE;
    }

    /* Create our audio I/O block */
    audioIOB = (struct IOAudio *)
	CreateExtIO(port, sizeof(struct IOAudio));
    if (!audioIOB)
    {
	return FALSE;
    }

    /* setup audio block with priority and allocation map */
    /* This gets sent as part of OpenDevice.              */
    audioIOB->ioa_Request.io_Message.mn_Node.ln_Pri = PRIORITY;
    audioIOB->ioa_Data = allocationMap;
    audioIOB->ioa_Length = sizeof(allocationMap);
    
    if (OpenDevice(AUDIONAME, 0, audioIOB, 0)) /* 0 = success */
    {
	return FALSE;
    }
    else
	deviceOpened = TRUE;

    return TRUE;
}

/**
 * @brief Do the orch sound
 */
do_orch()
{
    if (!deviceOpened)
	return FALSE;

    /* Setup I/O block to write to audio device */
    audioIOB->ioa_Request.io_Command = CMD_WRITE;
    audioIOB->ioa_Request.io_Flags = ADIOF_PERVOL;
    audioIOB->ioa_Data = orch;
    audioIOB->ioa_Length = orch_len;
    audioIOB->ioa_Period = PERIODSIZE;
    audioIOB->ioa_Volume = MAXVOLUME;
    audioIOB->ioa_Cycles = 1; /* play waveform ONCE */

    /* And Do it! */
    BeginIO(audioIOB);
    WaitIO(audioIOB);

    return TRUE;
}

/**
 * @brief clean up
 */
done()
{
    if (deviceOpened)
	CloseDevice(audioIOB);

    if (audioIOB)
	DeleteExtIO(audioIOB);

    if (port)
	DeletePort(port);

    return TRUE;
}

main()
{
    if (!init())
	done();

    do_orch();

    done();

    return 0;
}

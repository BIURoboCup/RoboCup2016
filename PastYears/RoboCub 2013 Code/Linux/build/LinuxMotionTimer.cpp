/*
 *   LinuxMotionTimer.cpp
 *
 *   Author: ROBOTIS
 *
 */

#include <unistd.h>
#include <sys/time.h>
#include "MotionModule.h"
#include "LinuxMotionTimer.h"
#include "IncludesFile.h"
#include "WalkingAndActions.h"

using namespace Robot;

MotionManager* LinuxMotionTimer::m_Manager(0);
bool LinuxMotionTimer::m_TimerRunning(false);
timer_t LinuxMotionTimer::m_TimerID(0);


void LinuxMotionTimer::TimerProc(int arg)
{
//	cout << "[TimerProc]: starting... arg = " << arg << endl;
	// ignore the falling signal in order to allow the gyro to get updated.
	signal(SIGUSR2, SIG_IGN);
	signal(SIGRTMIN, SIG_IGN);
	if(m_Manager != 0 && m_TimerRunning == true)
	{
	//	printf("[LinuxMotionTimer]: before Process\n");
		m_Manager->Process();
	}
	// register again to the falling signal.
	signal(SIGUSR2, WalkingAndActions::FallenHandler);
	signal(SIGRTMIN, TimerProc);
//	cout << "[TimerProc]: quitting... arg = " << arg << endl;
}

void LinuxMotionTimer::Initialize(MotionManager* manager)
{
	if(m_TimerID > 0)
	{		
		timer_delete(m_TimerID);
		m_TimerID = 0;
	}

	m_Manager = manager;

	struct itimerspec value;
    struct sigevent av_sig_spec;
	long nsec_interval = MotionModule::TIME_UNIT*1000000;

    av_sig_spec.sigev_notify = SIGEV_SIGNAL;
    av_sig_spec.sigev_signo = SIGRTMIN;

    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = nsec_interval;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = nsec_interval;

    timer_create(CLOCK_REALTIME, &av_sig_spec, &m_TimerID);
    timer_settime(m_TimerID, 0, &value, NULL);
    signal(SIGRTMIN, TimerProc);
	m_TimerRunning = true;
}

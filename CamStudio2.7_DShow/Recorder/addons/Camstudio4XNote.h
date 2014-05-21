#ifndef CAMSTUDIO4XNOTE_H	
#define CAMSTUDIO4XNOTE_H

#pragma once

// Define CAMSTUDIO4XNOTE if this is a custom build release for Xnote Stopwatch users (2010/05/28, Janhgm)
// Setting this defines will preconfigure a few innitial settings
// #define CAMSTUDIO4XNOTE


// Stopwatch sources applications
#define XNOTE_SOURCE_UNDEFINED					0
#define XNOTE_SOURCE_XNOTESTOPWATCH				1
#define XNOTE_SOURCE_MOTIONDETECTOR				2  

// How stopwatch trigger occured 
#define XNOTE_TRIGGER_UNDEFINED					-1
#define XNOTE_TRIGGER_STOPWATCH_UNDEFINED		0
#define XNOTE_TRIGGER_STOPWATCH_MANUAL			1
#define XNOTE_TRIGGER_STOPWATCH_DEVICE			2
#define XNOTE_TRIGGER_MOTIONDETECTOR			3

// Action events 
// XNote stopwatch : hiword: action (1 - start, 2 - stop, 3 - snap, 4 - reset)
#define XNOTE_ACTION_UNDEFINED					0
#define XNOTE_ACTION_STOPWATCH_START			1  
#define XNOTE_ACTION_STOPWATCH_STOP				2  
#define XNOTE_ACTION_STOPWATCH_SNAP				3
#define XNOTE_ACTION_STOPWATCH_RESET			4
// Motion Detector : ( 5 - alert )
#define XNOTE_ACTION_MOTIONDETECTOR_ALERT		5

#endif // CAMSTUDIO4XNOTE_H	
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* ============ CONFIG ============ */

// Update these to match robot ports and directions.
#define LF_PORT 2
#define LM_PORT 2  // Left back/middle
#define LR_PORT 3  // Left top/rear
#define RF_PORT 4  // Right front
#define RM_PORT 5  // Right back/middle
#define RR_PORT 6  // Right top/rear

#define IMU_PORT 10

//TODO tune please I'm begging you

// lateral
#define LkP 3
#define LkI 0.0
#define LkD 0.0

//angular
#define AkP 0.0
#define AkI 0.0
#define AkD 0.0

#endif

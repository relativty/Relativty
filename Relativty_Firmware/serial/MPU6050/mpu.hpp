#ifndef MPU_H
#define MPU_H

struct s_mympu {
	float ypr[3];
	float gyro[3];
  float qW, qX, qY, qZ;
};

extern struct s_mympu mympu;

int mympu_open(unsigned int rate);
int mympu_update();

#endif

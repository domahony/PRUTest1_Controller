/*
 * GY80.h
 *
 *  Created on: Sep 9, 2015
 *      Author: domahony
 */

#ifndef GY80_H_
#define GY80_H_

typedef struct {

	union {
		float v[3];

		struct {
			float x;
			float y;
			float z;
		};

		struct {
			float heading;
			float pitch;
			float roll;
		};
	};
} VecType3D;


typedef struct {

	float temperature;
	float pressure;

	VecType3D accelerometer;
	VecType3D gyroscope;
	VecType3D compass;

} GY80;

#endif /* GY80_H_ */

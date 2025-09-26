/*
 *    Copyright (C) 2025 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LEDARRAY_H
#define LEDARRAY_H

// Ice includes
#include <Ice/Ice.h>
#include <LEDArray.h>

#include "../src/specificworker.h"


class LEDArrayI : public virtual RoboCompLEDArray::LEDArray
{
public:
	LEDArrayI(GenericWorker *_worker, const size_t id);
	~LEDArrayI();

	RoboCompLEDArray::PixelArray getLEDArray(const Ice::Current&);
	bool setLEDArray(RoboCompLEDArray::PixelArray pixelArray, const Ice::Current&);

private:

	GenericWorker *worker;
	size_t id;

	// Array handlers for each method
	std::array<std::function<RoboCompLEDArray::PixelArray(void)>, 1> getLEDArrayHandlers;
	std::array<std::function<bool(RoboCompLEDArray::PixelArray)>, 1> setLEDArrayHandlers;

};

#endif

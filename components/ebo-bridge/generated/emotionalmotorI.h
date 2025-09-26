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
#ifndef EMOTIONALMOTOR_H
#define EMOTIONALMOTOR_H

// Ice includes
#include <Ice/Ice.h>
#include <EmotionalMotor.h>

#include "../src/specificworker.h"


class EmotionalMotorI : public virtual RoboCompEmotionalMotor::EmotionalMotor
{
public:
	EmotionalMotorI(GenericWorker *_worker, const size_t id);
	~EmotionalMotorI();

	void expressAnger(const Ice::Current&);
	void expressDisgust(const Ice::Current&);
	void expressFear(const Ice::Current&);
	void expressJoy(const Ice::Current&);
	void expressSadness(const Ice::Current&);
	void expressSurprise(const Ice::Current&);
	void isanybodythere(bool isAny, const Ice::Current&);
	void listening(bool setListening, const Ice::Current&);
	void pupposition(float x, float y, const Ice::Current&);
	void talking(bool setTalk, const Ice::Current&);

private:

	GenericWorker *worker;
	size_t id;

	// Array handlers for each method
	std::array<std::function<void(void)>, 1> expressAngerHandlers;
	std::array<std::function<void(void)>, 1> expressDisgustHandlers;
	std::array<std::function<void(void)>, 1> expressFearHandlers;
	std::array<std::function<void(void)>, 1> expressJoyHandlers;
	std::array<std::function<void(void)>, 1> expressSadnessHandlers;
	std::array<std::function<void(void)>, 1> expressSurpriseHandlers;
	std::array<std::function<void(bool)>, 1> isanybodythereHandlers;
	std::array<std::function<void(bool)>, 1> listeningHandlers;
	std::array<std::function<void(float, float)>, 1> puppositionHandlers;
	std::array<std::function<void(bool)>, 1> talkingHandlers;

};

#endif

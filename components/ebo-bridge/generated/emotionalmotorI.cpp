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
#include "emotionalmotorI.h"

EmotionalMotorI::EmotionalMotorI(GenericWorker *_worker, const size_t id): worker(_worker), id(id)
{
	expressAngerHandlers = {
		[this]() { return worker->EmotionalMotor_expressAnger(); }
	};

	expressDisgustHandlers = {
		[this]() { return worker->EmotionalMotor_expressDisgust(); }
	};

	expressFearHandlers = {
		[this]() { return worker->EmotionalMotor_expressFear(); }
	};

	expressJoyHandlers = {
		[this]() { return worker->EmotionalMotor_expressJoy(); }
	};

	expressSadnessHandlers = {
		[this]() { return worker->EmotionalMotor_expressSadness(); }
	};

	expressSurpriseHandlers = {
		[this]() { return worker->EmotionalMotor_expressSurprise(); }
	};

	isanybodythereHandlers = {
		[this](auto a) { return worker->EmotionalMotor_isanybodythere(a); }
	};

	listeningHandlers = {
		[this](auto a) { return worker->EmotionalMotor_listening(a); }
	};

	puppositionHandlers = {
		[this](auto a, auto b) { return worker->EmotionalMotor_pupposition(a, b); }
	};

	talkingHandlers = {
		[this](auto a) { return worker->EmotionalMotor_talking(a); }
	};

}


EmotionalMotorI::~EmotionalMotorI()
{
}


void EmotionalMotorI::expressAnger(const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < expressAngerHandlers.size())
		 expressAngerHandlers[id]();
	else
		throw std::out_of_range("Invalid expressAnger id: " + std::to_string(id));

}

void EmotionalMotorI::expressDisgust(const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < expressDisgustHandlers.size())
		 expressDisgustHandlers[id]();
	else
		throw std::out_of_range("Invalid expressDisgust id: " + std::to_string(id));

}

void EmotionalMotorI::expressFear(const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < expressFearHandlers.size())
		 expressFearHandlers[id]();
	else
		throw std::out_of_range("Invalid expressFear id: " + std::to_string(id));

}

void EmotionalMotorI::expressJoy(const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < expressJoyHandlers.size())
		 expressJoyHandlers[id]();
	else
		throw std::out_of_range("Invalid expressJoy id: " + std::to_string(id));

}

void EmotionalMotorI::expressSadness(const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < expressSadnessHandlers.size())
		 expressSadnessHandlers[id]();
	else
		throw std::out_of_range("Invalid expressSadness id: " + std::to_string(id));

}

void EmotionalMotorI::expressSurprise(const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < expressSurpriseHandlers.size())
		 expressSurpriseHandlers[id]();
	else
		throw std::out_of_range("Invalid expressSurprise id: " + std::to_string(id));

}

void EmotionalMotorI::isanybodythere(bool isAny, const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < isanybodythereHandlers.size())
		 isanybodythereHandlers[id](isAny);
	else
		throw std::out_of_range("Invalid isanybodythere id: " + std::to_string(id));

}

void EmotionalMotorI::listening(bool setListening, const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < listeningHandlers.size())
		 listeningHandlers[id](setListening);
	else
		throw std::out_of_range("Invalid listening id: " + std::to_string(id));

}

void EmotionalMotorI::pupposition(float x, float y, const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < puppositionHandlers.size())
		 puppositionHandlers[id](x, y);
	else
		throw std::out_of_range("Invalid pupposition id: " + std::to_string(id));

}

void EmotionalMotorI::talking(bool setTalk, const Ice::Current&)
{

    #ifdef HIBERNATION_ENABLED
		worker->hibernationTick();
	#endif
    
	if (id < talkingHandlers.size())
		 talkingHandlers[id](setTalk);
	else
		throw std::out_of_range("Invalid talking id: " + std::to_string(id));

}


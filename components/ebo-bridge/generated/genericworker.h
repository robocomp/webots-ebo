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
#ifndef GENERICWORKER_H
#define GENERICWORKER_H

#include <stdint.h>
#include <grafcetStep/GRAFCETStep.h>
#include <ConfigLoader/ConfigLoader.h>
#include <QStateMachine>
#include <QEvent>
#include <QString>
#include <functional>
#include <atomic>
#include <QtCore>
#include <variant>
#include <unordered_map>


#include <BatteryStatus.h>
#include <CameraSimple.h>
#include <DifferentialRobot.h>
#include <EmergencyStop.h>
#include <EmergencyStopPub.h>
#include <EmotionalMotor.h>
#include <GenericBase.h>
#include <JoystickAdapter.h>
#include <LEDArray.h>
#include <Laser.h>
#include <Speech.h>

#define BASIC_PERIOD 100

using TuplePrx = std::tuple<RoboCompEmergencyStopPub::EmergencyStopPubPrxPtr>;


class GenericWorker : public QObject
{
Q_OBJECT
public:
	GenericWorker(const ConfigLoader& configLoader, TuplePrx tprx);
	virtual ~GenericWorker();
	virtual void killYourSelf();

	void setPeriod(const std::string& state, int period);
	int getPeriod(const std::string& state);

	QStateMachine statemachine;
	QTimer hibernationChecker;
	std::atomic_bool hibernation = false;


	RoboCompEmergencyStopPub::EmergencyStopPubPrxPtr emergencystoppub_pubproxy;

	virtual RoboCompBatteryStatus::TBattery BatteryStatus_getBatteryState() = 0;

	virtual RoboCompCameraSimple::TImage CameraSimple_getImage() = 0;

	virtual void DifferentialRobot_correctOdometer(int x, int z, float alpha) = 0;
	virtual void DifferentialRobot_getBasePose(int &x, int &z, float &alpha) = 0;
	virtual void DifferentialRobot_getBaseState(RoboCompGenericBase::TBaseState &state) = 0;
	virtual void DifferentialRobot_resetOdometer() = 0;
	virtual void DifferentialRobot_setOdometer(RoboCompGenericBase::TBaseState state) = 0;
	virtual void DifferentialRobot_setOdometerPose(int x, int z, float alpha) = 0;
	virtual void DifferentialRobot_setSpeedBase(float adv, float rot) = 0;
	virtual void DifferentialRobot_stopBase() = 0;

	virtual bool EmergencyStop_isEmergency() = 0;

	virtual void EmotionalMotor_expressAnger() = 0;
	virtual void EmotionalMotor_expressDisgust() = 0;
	virtual void EmotionalMotor_expressFear() = 0;
	virtual void EmotionalMotor_expressJoy() = 0;
	virtual void EmotionalMotor_expressSadness() = 0;
	virtual void EmotionalMotor_expressSurprise() = 0;
	virtual void EmotionalMotor_isanybodythere(bool isAny) = 0;
	virtual void EmotionalMotor_listening(bool setListening) = 0;
	virtual void EmotionalMotor_pupposition(float x, float y) = 0;
	virtual void EmotionalMotor_talking(bool setTalk) = 0;

	virtual RoboCompLEDArray::PixelArray LEDArray_getLEDArray() = 0;
	virtual bool LEDArray_setLEDArray(RoboCompLEDArray::PixelArray pixelArray) = 0;

	virtual RoboCompLaser::TLaserData Laser_getLaserAndBStateData(RoboCompGenericBase::TBaseState &bState) = 0;
	virtual RoboCompLaser::LaserConfData Laser_getLaserConfData() = 0;
	virtual RoboCompLaser::TLaserData Laser_getLaserData() = 0;

	virtual bool Speech_isBusy() = 0;
	virtual bool Speech_say(std::string text, bool overwrite) = 0;

	virtual void JoystickAdapter_sendData (RoboCompJoystickAdapter::TData data) = 0;


protected:
	std::unordered_map<std::string, std::unique_ptr<GRAFCETStep>> states;
	ConfigLoader configLoader;




private:

public slots:
	virtual void initialize() = 0;
	virtual void compute() = 0;
	virtual void emergency() = 0;
	virtual void restore() = 0;
	void hibernationCheck();
	void hibernationTick();
	
signals:
	void kill();
	void goToEmergency();
	void goToRestore();
};

#endif

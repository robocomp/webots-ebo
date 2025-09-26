/*
 *    Copyright (C) 2024 by YOUR NAME HERE
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

/**
	\brief
	@author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#define HIBERNATION_ENABLED

#include <array>
#include <genericworker.h>
#include <webots/Supervisor.hpp>
#include <webots/Camera.hpp>
#include <webots/Motor.hpp>
#include <webots/Lidar.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Display.hpp>
#include <webots/ImageRef.hpp>
#include <webots/Speaker.hpp>
#include <webots/LED.hpp>

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
    SpecificWorker(const ConfigLoader& configLoader, TuplePrx tprx, bool startup_check);
    ~SpecificWorker();
    RoboCompBatteryStatus::TBattery BatteryStatus_getBatteryState();
    RoboCompCameraSimple::TImage CameraSimple_getImage();
    void DifferentialRobot_correctOdometer(int x, int z, float alpha);
    void DifferentialRobot_getBasePose(int &x, int &z, float &alpha);
    void DifferentialRobot_getBaseState(RoboCompGenericBase::TBaseState &state);
    void DifferentialRobot_resetOdometer();
    void DifferentialRobot_setOdometer(RoboCompGenericBase::TBaseState state);
    void DifferentialRobot_setOdometerPose(int x, int z, float alpha);
    void DifferentialRobot_setSpeedBase(float adv, float rot);
    void DifferentialRobot_stopBase();
    bool EmergencyStop_isEmergency();
    void EmotionalMotor_expressAnger();
    void EmotionalMotor_expressDisgust();
    void EmotionalMotor_expressFear();
    void EmotionalMotor_expressJoy();
    void EmotionalMotor_expressSadness();
    void EmotionalMotor_expressSurprise();
    void EmotionalMotor_isanybodythere(bool isAny);
    void EmotionalMotor_listening(bool setListening);
    void EmotionalMotor_pupposition(float x, float y);
    void EmotionalMotor_talking(bool setTalk);
    RoboCompLaser::TLaserData Laser_getLaserAndBStateData(RoboCompGenericBase::TBaseState &bState);
    RoboCompLaser::LaserConfData Laser_getLaserConfData();
    RoboCompLaser::TLaserData Laser_getLaserData();
    bool Speech_isBusy();
    bool Speech_say(std::string text, bool overwrite);
    RoboCompLEDArray::PixelArray LEDArray_getLEDArray();
    bool LEDArray_setLEDArray(RoboCompLEDArray::PixelArray pixelArray);

    void JoystickAdapter_sendData(RoboCompJoystickAdapter::TData data);

    webots::Supervisor* robot;
    webots::Camera* camera;
    std::array<webots::Motor*, 2> motors;
    RoboCompCameraSimple::TImage imgData;
    std::array<webots::DistanceSensor*, 12> webotsLidars;
    RoboCompLaser::TLaserData robocompLidars;
    webots::DistanceSensor* emergencyLidar;
    webots::Display* display;
    webots::Speaker* speaker;
    std::array<webots::LED*, 26> webotsLeds;

    const int REALROBOTSLEDS = 54;

public slots:
    void initialize();
    void compute();
    void emergency();
    void restore();
    int startup_check();
private:
    bool startup_check_flag;

    std::vector<float> lidarAngles = {
            0,          // LIDAR 1
            1.0472,     // LIDAR 2
            1.5708,     // LIDAR 3
            2.0944,     // LIDAR 4
            2.35619,    // LIDAR 5
            2.61799,    // LIDAR 6
            3.14159,    // LIDAR 7
            3.66519,    // LIDAR 8
            3.92699,    // LIDAR 9
            4.18879,    // LIDAR 10
            4.71239,    // LIDAR 11
            5.23599     // LIDAR 12
    };
    std::map<std::string, webots::ImageRef*> facesImages;

    struct PARAMS
    {
        bool delay = false;
        bool do_joystick = true;
    };
    PARAMS pars;

    void initializeRobot();
    void receivingImageData();
    void receivingLidarsData();
    void setExpression(std::string expression);

    void printNotImplementedWarningMessage(std::string functionName);

    void testMovement();
    void testFaces();

    void printPosition();
    void printLidars();

signals:
        //void customSignal();
};

#endif

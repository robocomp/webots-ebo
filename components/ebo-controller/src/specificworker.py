#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
#    Copyright (C) 2025 by YOUR NAME HERE
#
#    This file is part of RoboComp
#
#    RoboComp is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    RoboComp is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
#
import random

from PySide6.QtCore import QTimer
from PySide6.QtWidgets import QApplication
from mercurial.revset import follow
from rich.console import Console
from genericworker import *
import interfaces as ifaces
import numpy as np

sys.path.append('/opt/robocomp/lib')
console = Console(highlight=False)


# If RoboComp was compiled with Python bindings you can use InnerModel in Python
# import librobocomp_qmat
# import librobocomp_osgviewer
# import librobocomp_innermodel


class SpecificWorker(GenericWorker):
    def __init__(self, proxy_map, startup_check=False):
        super(SpecificWorker, self).__init__(proxy_map)
        self.Period = 10

        # Set the state of the robot
        self.state = "forward"
        self.first_turn_direction = None
        self.actual_turn_direction = None
        self.max_speed = 10
        self.points = None
        self.wall_lost = False

        # Get the angles of the lidar
        self.lidar_address = {
            "front": [4, 5, 6, 7, 8],
            "back": [0],
            "left": [1, 2, 3],
            "right": [9, 10, 11],
        }

        if startup_check:
            self.startup_check()
        else:
            self.timer.timeout.connect(self.compute)
            self.timer.start(self.Period)

    def __del__(self):
        """Destructor"""

    def setParams(self, params):
        # try:
        #	self.innermodel = InnerModel(params["InnerModelPath"])
        # except:
        #	traceback.print_exc()
        #	print("Error reading config params")
        return True


    @QtCore.Slot()
    def compute(self):
        self.points = self.laser_proxy.getLaserData()

        match self.state:
            case "idle":
                print("Doing nothing")

            case "forward":
                if self.detectWallAddress("front"):
                    self.differentialrobot_proxy.setSpeedBase(0, 0)
                    self.first_turn_direction = self.chooseTurnDirection()
                    self.actual_turn_direction = self.first_turn_direction
                    self.state = "turn"
                else:
                    print("Moving forward")
                    self.differentialrobot_proxy.setSpeedBase(self.max_speed, 0)

            case "turn":
                if self.detectWallAddress("front"):
                    print("Turning robot to the " + self.actual_turn_direction)
                    self.turn(self.actual_turn_direction)
                else:
                    self.differentialrobot_proxy.setSpeedBase(0, 0)
                    self.state = "follow_wall"

            case "follow_wall":
                print("Following wall")
                self.followWall()

        return True

    def startup_check(self):
        print(f"Testing RoboCompCameraSimple.TImage from ifaces.RoboCompCameraSimple")
        test = ifaces.RoboCompCameraSimple.TImage()
        print(f"Testing RoboCompDifferentialRobot.TMechParams from ifaces.RoboCompDifferentialRobot")
        test = ifaces.RoboCompDifferentialRobot.TMechParams()
        print(f"Testing RoboCompLaser.LaserConfData from ifaces.RoboCompLaser")
        test = ifaces.RoboCompLaser.LaserConfData()
        print(f"Testing RoboCompLaser.TData from ifaces.RoboCompLaser")
        test = ifaces.RoboCompLaser.TData()
        QTimer.singleShot(200, QApplication.instance().quit)

    def detectWallAddress(self, address):
        for idx in self.lidar_address[address]:
            if (self.points[idx].dist / 1000) < 0.2:
                return True
        return False


    def chooseTurnDirection(self):
        print("Turning robot")
        right = self.detectWallAddress("right")
        left = self.detectWallAddress("left")

        if not right and left:
            turn_direction = "right"
        elif right and not left:
            turn_direction = "left"
        else:
            left_or_right = random.randint(0, 1)
            if left_or_right:
                turn_direction = "left"
            else:
                turn_direction = "right"

        return turn_direction

    def turn(self, direction):
        # positive = left, negative = right
        if direction == "left":
            self.differentialrobot_proxy.setSpeedBase(0, self.max_speed/2)
        else:
            self.differentialrobot_proxy.setSpeedBase(0, -self.max_speed/2)

    def perp_to_wall(self):
        lidar_dists = []
        thresh = 0.005
        if self.first_turn_direction == "left":
            for idx in self.lidar_address["right"]:
                lidar_dists.append(self.points[idx].dist / 1000)
            # Check if the three points are the same
            print(lidar_dists)
            if np.std(lidar_dists) < thresh:
                return True
            else:
                return False
        else:
            for idx in self.lidar_address["left"]:
                lidar_dists.append(self.points[idx].dist / 1000)
            # Check if the three points are the same
            print(lidar_dists)
            if np.std(lidar_dists) < thresh:
                return True
            else:
                return False

    def followWall(self):
        if self.first_turn_direction == "left":
            if not self.detectWallAddress("right"):
                print("Wall lost")
                self.wall_lost = True
                self.actual_turn_direction = "right"
                self.state = "turn"
            elif self.detectWallAddress("front"):
                print("Wall in front")
                self.actual_turn_direction = "left"
                self.state = "turn"
            else:
                self.differentialrobot_proxy.setSpeedBase(self.max_speed, 0)
        else:
            if not self.detectWallAddress("left"):
                print("Wall lost")
                self.wall_lost = True
                self.actual_turn_direction = "left"
                self.state = "turn"
            elif self.detectWallAddress("front"):
                print("Wall in front")
                self.actual_turn_direction = "right"
                self.state = "turn"
            else:
                self.differentialrobot_proxy.setSpeedBase(self.max_speed, 0)








    ######################
    # From the RoboCompCameraSimple you can call this methods:
    # self.camerasimple_proxy.getImage(...)

    ######################
    # From the RoboCompCameraSimple you can use this types:
    # RoboCompCameraSimple.TImage

    ######################
    # From the RoboCompDifferentialRobot you can call this methods:
    # self.differentialrobot_proxy.correctOdometer(...)
    # self.differentialrobot_proxy.getBasePose(...)
    # self.differentialrobot_proxy.getBaseState(...)
    # self.differentialrobot_proxy.resetOdometer(...)
    # self.differentialrobot_proxy.setOdometer(...)
    # self.differentialrobot_proxy.setOdometerPose(...)
    # self.differentialrobot_proxy.setSpeedBase(...)
    # self.differentialrobot_proxy.stopBase(...)

    ######################
    # From the RoboCompDifferentialRobot you can use this types:
    # RoboCompDifferentialRobot.TMechParams

    ######################
    # From the RoboCompLaser you can call this methods:
    # self.laser_proxy.getLaserAndBStateData(...)
    # self.laser_proxy.getLaserConfData(...)
    # self.laser_proxy.getLaserData(...)

    ######################
    # From the RoboCompLaser you can use this types:
    # RoboCompLaser.LaserConfData
    # RoboCompLaser.TData



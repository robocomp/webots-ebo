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
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(const ConfigLoader& configLoader, TuplePrx tprx, bool startup_check) : GenericWorker(configLoader, tprx)
{
this->startup_check_flag = startup_check;
	if(this->startup_check_flag)
	{
		this->startup_check();
	}
	else
	{
		#ifdef HIBERNATION_ENABLED
			hibernationChecker.start(500);
		#endif

		
		// Example statemachine:
		/***
		//Your definition for the statesmachine (if you dont want use a execute function, use nullptr)
		states["CustomState"] = std::make_unique<GRAFCETStep>("CustomState", period, 
															std::bind(&SpecificWorker::customLoop, this),  // Cyclic function
															std::bind(&SpecificWorker::customEnter, this), // On-enter function
															std::bind(&SpecificWorker::customExit, this)); // On-exit function

		//Add your definition of transitions (addTransition(originOfSignal, signal, dstState))
		states["CustomState"]->addTransition(states["CustomState"].get(), SIGNAL(entered()), states["OtherState"].get());
		states["Compute"]->addTransition(this, SIGNAL(customSignal()), states["CustomState"].get()); //Define your signal in the .h file under the "Signals" section.

		//Add your custom state
		statemachine.addState(states["CustomState"].get());
		***/

		statemachine.setChildMode(QState::ExclusiveStates);
		statemachine.start();

		auto error = statemachine.errorString();
		if (error.length() > 0){
			qWarning() << error;
			throw error;
		}
		
	}
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
    std::cout << "Destroying SpecificWorker" << std::endl;
}

void SpecificWorker::initialize()
{

//chekpoint robocompUpdater
    std::cout << "Initialize worker" << std::endl;
    if(this->startup_check_flag)
    {
        this->startup_check();
    }
    else
    {
#ifdef HIBERNATION_ENABLED
        hibernationChecker.start(500);
#endif
        initializeRobot();
    }

}

void SpecificWorker::initializeRobot(){
    robot = new webots::Supervisor();

    //--------------------------- Camera
    camera = robot->getCamera("camera");
    if (camera) camera->enable(this->getPeriod("Compute")); else std::cout << "Cámara no encontrada." << std::endl;

    // Motors
    const char* motorNames[2] = {"motor_right", "motor_left"};
    for (size_t i = 0; i < motors.size(); ++i)
    {
        motors[i] = robot->getMotor(motorNames[i]);
        motors[i]->setPosition(INFINITY);                   // Velocity mode
        motors[i]->setVelocity(0);                      // Initial velocity
    }

    //--------------------------- Lidars
    for (int i=0; i<webotsLidars.size(); i++){
        std::string lidar_name = (i == 0) ? "lidar" : "lidar(" + std::to_string(i) + ")";
        webots::DistanceSensor* lidar = robot->getDistanceSensor(lidar_name);
        if (lidar){
            lidar->enable(this->getPeriod("Compute"));
            webotsLidars[i] = lidar;
            robocompLidars.emplace_back();
        } else std::cout << "Lidar: " << lidar_name << " no encontrado." << std::endl;
    }

    //--------------------------- Display
    display = robot->getDisplay("display");
    if (!display) std::cout << "Pantalla no encontrada." << std::endl;

    std::string imageDirectory = "../../resources/images/";
    for (const auto &entry : std::filesystem::directory_iterator(imageDirectory)) {
        std::string filePath = entry.path().string();  // Ruta completa del archivo
        std::string fileName = entry.path().stem().string();  // Nombre del archivo

        // Cargar la imagen y verificar si fue cargada correctamente
        webots::ImageRef* image = display->imageLoad(filePath);

        if (image != nullptr) {
            // Guardar la imagen en el std::map usando el nombre del archivo como clave
            facesImages[fileName] = image;
            std::cout << "Imagen cargada: " << fileName << std::endl;
        } else {
            std::cout << "No se pudo cargar la imagen: " << fileName << std::endl;
        }
    }
    display->imagePaste(facesImages["Idle"], 0, 0, false);

    //--------------------------- Speaker
    speaker = robot->getSpeaker("speaker");
    if (!speaker) std::cout << "Speaker no encontrado." << std::endl;
    if(speaker){
        speaker->setEngine("pico");
        speaker->setLanguage("es-ES");
        speaker->speak("Hola. Soy tu amigo EBO", 10);
    }

    //--------------------------- LEDs
    for (int i=0; i<webotsLeds.size(); i++){
        std::string led_name = (i == 0) ? "led" : "led(" + std::to_string(i) + ")";
        webots::LED* led = robot->getLED(led_name);
        if (led){
            led->set(0);
            webotsLeds[i] = led;
        } else std::cout << "Led: " << led_name << " no encontrado." << std::endl;
    }

    //--------------------------- Simulation step
    robot->step(this->getPeriod("Compute"));
}


void SpecificWorker::compute()
{
    receivingImageData();
    receivingLidarsData();
    
    robot->step(this->getPeriod("Compute"));
}

void SpecificWorker::emergency()
{
    std::cout << "Emergency worker" << std::endl;
    for(auto motor: motors) motor->setVelocity(0);
}

//Execute one when exiting to emergencyState
void SpecificWorker::restore()
{
    std::cout << "Restore worker" << std::endl;
}

int SpecificWorker::startup_check()
{
    std::cout << "Startup check" << std::endl;
    QTimer::singleShot(200, qApp, SLOT(quit()));
    return 0;
}

#pragma region Battery

RoboCompBatteryStatus::TBattery SpecificWorker::BatteryStatus_getBatteryState()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
//implementCODE
    printNotImplementedWarningMessage("BatteryStatus_getBatteryState");
}

#pragma endregion

#pragma region CameraSimple

RoboCompCameraSimple::TImage SpecificWorker::CameraSimple_getImage()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    return imgData;
}

void SpecificWorker::receivingImageData() {

    // Obtener el buffer de imagen
    const unsigned char* image = camera->getImage();
    if (!image) {
        throw std::runtime_error("Error al obtener la imagen de la cámara.");
    }

    // Configurar los campos de la estructura TImage
    imgData.compressed = false;  // Asumimos que la imagen no está comprimida
    imgData.width = camera->getWidth();
    imgData.height = camera->getHeight();
    imgData.depth = 3;  // Asumimos una imagen RGB con 3 bytes por píxel (8 bits por canal)

    // Crear un std::vector de bytes para almacenar los datos de la imagen
    const int totalPixels = imgData.width * imgData.height * imgData.depth;
    imgData.image.resize(totalPixels);

    // Copiar la imagen de la cámara a imgData.image (asumiendo formato RGB)
    for (int i = 0; i < totalPixels; i++) {
        imgData.image[i] = image[i];  // Copiamos el buffer crudo de la imagen
    }
}

#pragma endregion

#pragma region DifferentialRobot

void SpecificWorker::DifferentialRobot_correctOdometer(int x, int z, float alpha)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    printNotImplementedWarningMessage("DifferentialRobot_correctOdometer");
}

void SpecificWorker::DifferentialRobot_getBasePose(int &x, int &z, float &alpha)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    const double *pos = robot->getSelf()->getPosition();
    const double alpha_rad = robot->getSelf()->getOrientation()[2]; // Angulo de rotación en el plano

    x = static_cast<int>(pos[0] * 1000);  // Convertir metros a milímetros
    z = static_cast<int>(pos[2] * 1000);  // Convertir metros a milímetros
    alpha = static_cast<float>(alpha_rad);  // En radianes
}

void SpecificWorker::DifferentialRobot_getBaseState(RoboCompGenericBase::TBaseState &state)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    // Robot position
    const double *pos =  robot->getSelf()->getPosition();
    state.x = static_cast<int>(pos[0] * 1000);  // Convertir a milímetros
    state.z = static_cast<int>(pos[2] * 1000);  // Convertir a milímetros

    // Robot orientation
    state.alpha = robot->getSelf()->getOrientation()[2];  // Usar rotación en el plano

    // Robot speeds (can be obtained from motors)
    state.advVx = motors[0]->getVelocity();  // Velocidad del motor derecho
    state.rotV = motors[1]->getVelocity();  // Velocidad del motor izquierdo
}

void SpecificWorker::DifferentialRobot_resetOdometer()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    printNotImplementedWarningMessage("DifferentialRobot_resetOdometer");
}

void SpecificWorker::DifferentialRobot_setOdometer(RoboCompGenericBase::TBaseState state)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    printNotImplementedWarningMessage("DifferentialRobot_setOdometer");
}

void SpecificWorker::DifferentialRobot_setOdometerPose(int x, int z, float alpha)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    printNotImplementedWarningMessage("DifferentialRobot_setOdometerPose");
}

void SpecificWorker::DifferentialRobot_setSpeedBase(float adv, float rot)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    float right_speed = adv + rot;
    float left_speed = adv - rot;

    motors[0]->setVelocity(right_speed);
    motors[1]->setVelocity(left_speed);

    std::cout << "Setting speed: adv = " << adv << ", rot = " << rot << std::endl;
}

void SpecificWorker::DifferentialRobot_stopBase()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    motors[0]->setVelocity(0);
    motors[1]->setVelocity(0);

    std::cout << "Robot stopped." << std::endl;

    robot->getTime();
}

#pragma endregion

#pragma region EmergencyStop

bool SpecificWorker::EmergencyStop_isEmergency()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    printNotImplementedWarningMessage("EmergencyStop_isEmergency");
    return false;
}

#pragma endregion

#pragma region EmotionalRobot
void SpecificWorker::EmotionalMotor_expressAnger()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    std::cout << "Expressing Anger!" << std::endl;
    setExpression("Anger");
}

void SpecificWorker::EmotionalMotor_expressDisgust()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    std::cout << "Expressing Disgust!" << std::endl;
    setExpression("Disgust");
}

void SpecificWorker::EmotionalMotor_expressFear()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    std::cout << "Expressing Fear!" << std::endl;
    setExpression("Fear");
}

void SpecificWorker::EmotionalMotor_expressJoy()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    std::cout << "Expressing Joy!" << std::endl;
    setExpression("Joy");
}

void SpecificWorker::EmotionalMotor_expressSadness()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    std::cout << "Expressing Sadness!" << std::endl;
    setExpression("Sadness");
}

void SpecificWorker::EmotionalMotor_expressSurprise()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    std::cout << "Expressing Surprise!" << std::endl;
    setExpression("Surprise");
}

void SpecificWorker::EmotionalMotor_isanybodythere(bool isAny)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("EmotionalMotor_isanybodythere");

}

void SpecificWorker::EmotionalMotor_listening(bool setListening)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("EmotionalMotor_listening");

}

void SpecificWorker::EmotionalMotor_pupposition(float x, float y)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("EmotionalMotor_pupposition");

}

void SpecificWorker::EmotionalMotor_talking(bool setTalk)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("EmotionalMotor_talking");

}

#pragma endregion

#pragma region Laser

RoboCompLaser::TLaserData SpecificWorker::Laser_getLaserAndBStateData(RoboCompGenericBase::TBaseState &bState)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("Laser_getLaserAndBStateData");
}

RoboCompLaser::LaserConfData SpecificWorker::Laser_getLaserConfData()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("Laser_getLaserConfData");
}

RoboCompLaser::TLaserData SpecificWorker::Laser_getLaserData()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    return robocompLidars;
}

#pragma endregion

#pragma region Speech
bool SpecificWorker::Speech_isBusy()
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    printNotImplementedWarningMessage("Speech_isBusy");
}

bool SpecificWorker::Speech_say(std::string text, bool overwrite)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    speaker->speak(text, 1);
}

#pragma endregion

#pragma region LEDArray

RoboCompLEDArray::PixelArray SpecificWorker::LEDArray_getLEDArray()
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
    printNotImplementedWarningMessage("LEDArray_getLEDArray");
}

bool SpecificWorker::LEDArray_setLEDArray(RoboCompLEDArray::PixelArray pixelArray)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif

    // Definir la cantidad total de LEDs en el robot real y en Webots
     const size_t WEBOTSLEDS = webotsLeds.size();      // Total de LEDs en Webots

    // Asegúrate de que pixelArray tiene datos
    if (pixelArray.empty()) {
        std::cerr << "Error: pixelArray it's empty." << std::endl;
        return false;
    }

    // Iterar sobre los elementos del pixelArray
    for (const auto& pixelEntry : pixelArray) {
        short index = pixelEntry.first; // Índice del LED en el robot real
        const RoboCompLEDArray::Pixel& pixel = pixelEntry.second; // Pixel correspondiente

        // Asegurarse de que el índice esté dentro del rango de LEDs del robot real
        if (index < 0 || index >= REALROBOTSLEDS) {
            std::cerr << "Warning: Index " << index << " is out of bounds for real robot LEDs." << std::endl;
            continue; // Salir si el índice está fuera de límites
        }

        // Calcular el índice correspondiente en webotsLeds
        size_t webotsIndex = index * WEBOTSLEDS / REALROBOTSLEDS;

        // Asegurarse de que el índice esté dentro del rango de LEDs de Webots
        if (webotsIndex < WEBOTSLEDS) {
            // Unificar en un entero de 32 bits (con Alpha = 0xFF)
            unsigned int color = (0xFF << 24) | (static_cast<unsigned int>(pixel.red) << 16) |
                                 (static_cast<unsigned int>(pixel.green) << 8) |
                                 static_cast<unsigned int>(pixel.blue);

            // Comprobar que el puntero no es nulo antes de establecer el color
            if (webotsLeds[webotsIndex] != nullptr) {
                webotsLeds[webotsIndex]->set(color);  // Método para establecer el color en el LED de Webots
            } else {
                std::cerr << "Warning: Webots LED at index " << webotsIndex << " is null." << std::endl;
            }
        } else {
            std::cerr << "Warning: Calculated webots index " << webotsIndex << " exceeds Webots LED count." << std::endl;
        }
    }

    return true;
}

#pragma endregion

//SUBSCRIPTION to sendData method from JoystickAdapter interface
void SpecificWorker::JoystickAdapter_sendData(RoboCompJoystickAdapter::TData data)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    // Declaration of the structure to be filled
    float adv=0, rot=0;
    /*
    // Iterate through the list of buttons in the data structure
    for (RoboCompJoystickAdapter::ButtonParams button : data.buttons) {
        // Currently does nothing with the buttons
    }
    */

    // Iterate through the list of axes in the data structure
    for (RoboCompJoystickAdapter::AxisParams axis : data.axes)
    {
        // Process the axis according to its name
        if(axis.name == "rotate")
            rot = axis.value;
        else if (axis.name == "advance")
            adv = axis.value;
        else
            std::cout << "[ JoystickAdapter ] Warning: Using a non-defined axes (" << axis.name << ")." << std::endl;
    }
    if(pars.do_joystick)
        DifferentialRobot_setSpeedBase( adv, rot);
}


void SpecificWorker::printNotImplementedWarningMessage(std::string functionName)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif

    std::cout << "Function not implemented used: " << "[" << functionName << "]" << std::endl;
}

void SpecificWorker::testMovement()
{
    // Variables para la posición inicial
    int initial_x = 0, initial_z = 0;
    float initial_alpha = 0.0;

    // Obtener la posición inicial del robot
    DifferentialRobot_getBasePose(initial_x, initial_z, initial_alpha);
    std::cout << "Starting position - x: " << initial_x << ", z: " << initial_z << ", alpha: " << initial_alpha << std::endl;

    // 1. Avanzar hacia adelante durante 2 segundos
    std::cout << "Moving forward..." << std::endl;
    DifferentialRobot_setSpeedBase(1.0, 0.0);  // Avanzar recto (velocidad de avance 1.0)
    robot->step(2000);  // Esperar 2 segundos
    printPosition();

    // 2. Girar a la derecha (rotación en sentido antihorario)
    std::cout << "Turning right..." << std::endl;
    DifferentialRobot_setSpeedBase(0.0, -0.5);  // Girar a la derecha (velocidad de rotación -0.5)
    robot->step(1000);  // Esperar 1 segundo
    printPosition();

    // 3. Avanzar hacia adelante durante 2 segundos
    std::cout << "Moving forward again..." << std::endl;
    DifferentialRobot_setSpeedBase(1.0, 0.0);
    robot->step(2000);
    printPosition();

    // 4. Moverse hacia atrás durante 2 segundos
    std::cout << "Moving backward..." << std::endl;
    DifferentialRobot_setSpeedBase(-1.0, 0.0);  // Ir marcha atrás
    robot->step(2000);
    printPosition();

    // 5. Girar 180 grados
    std::cout << "Turning..." << std::endl;
    DifferentialRobot_setSpeedBase(0.0, 1.0);  // Girar en sentido horario
    robot->step(2000);  // Girar durante 2 segundos
    printPosition();

    // 6. Avanzar un poco más hacia adelante durante 2 segundos
    std::cout << "Moving forward a bit more..." << std::endl;
    DifferentialRobot_setSpeedBase(1.0, 0.0);
    robot->step(2000);
    printPosition();

    // 7. Dar la vuelta y volver a la posición inicial
    std::cout << "Returning to the initial position..." << std::endl;
    DifferentialRobot_setSpeedBase(0.0, 1.0);  // Girar en sentido horario
    robot->step(2000);
    DifferentialRobot_setSpeedBase(-1.0, 0.0);  // Ir marcha atrás para volver
    robot->step(2000);
    printPosition();

    // 8. Detener el robot
    std::cout << "Stopping robot..." << std::endl;
    DifferentialRobot_stopBase();

    // Obtener la posición final y compararla con la inicial
    int final_x = 0, final_z = 0;
    float final_alpha = 0.0;
    DifferentialRobot_getBasePose(final_x, final_z, final_alpha);

    std::cout << "Final position - x: " << final_x << ", z: " << final_z << ", alpha: " << final_alpha << std::endl;
}

// Función auxiliar para imprimir la posición actual
void SpecificWorker::printPosition()
{
    int x = 0, z = 0;
    float alpha = 0.0;
    DifferentialRobot_getBasePose(x, z, alpha);
    std::cout << "Current position - x: " << x << ", z: " << z << ", alpha: " << alpha << std::endl;
}

void SpecificWorker::receivingLidarsData() {
    for(int i=0; i < webotsLidars.size(); i++){
        webots::DistanceSensor* lidar = webotsLidars[i];

        RoboCompLaser::TData robocompLidar;
        robocompLidar.angle = lidarAngles[i];
        robocompLidar.dist = static_cast<float>(lidar->getValue());
        robocompLidars[i] = robocompLidar;
    }
}

void SpecificWorker::printLidars() {
    for (int i = 0; i < robocompLidars.size(); ++i) {
        std::cout << "Lidar n." << i << ":" << std::endl;
        std::cout << "\tangle: " << robocompLidars[i].angle << std::endl;
        std::cout << "\tdist: " << robocompLidars[i].dist << std::endl;
    }
}

void SpecificWorker::setExpression(std::string expression) {
    webots::ImageRef* image = facesImages[expression];
    if(!image){
        std::cout << "Expression not found." << std::endl;
        return;
    }

    display->imagePaste(image, 0, 0, false);
}

void SpecificWorker::testFaces() {

    EmotionalMotor_expressAnger();
    robot->step(1000);  // Esperar 1 segundo

    EmotionalMotor_expressDisgust();
    robot->step(1000);  // Esperar 1 segundo

    EmotionalMotor_expressJoy();
    robot->step(1000);  // Esperar 1 segundo

    EmotionalMotor_expressFear();
    robot->step(1000);  // Esperar 1 segundo

    EmotionalMotor_expressSadness();
    robot->step(1000);  // Esperar 1 segundo

    EmotionalMotor_expressSurprise();
    robot->step(1000);  // Esperar 1 segundo
}


/**************************************/
// From the RoboCompEmergencyStopPub you can publish calling this methods:
// this->emergencystoppub_pubproxy->emergencyStop(...)

/**************************************/
// From the RoboCompBatteryStatus you can use this types:
// RoboCompBatteryStatus::TBattery

/**************************************/
// From the RoboCompCameraSimple you can use this types:
// RoboCompCameraSimple::TImage

/**************************************/
// From the RoboCompDifferentialRobot you can use this types:
// RoboCompDifferentialRobot::TMechParams

/**************************************/
// From the RoboCompLEDArray you can use this types:
// RoboCompLEDArray::Pixel

/**************************************/
// From the RoboCompLaser you can use this types:
// RoboCompLaser::LaserConfData
// RoboCompLaser::TData


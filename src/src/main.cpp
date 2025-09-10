#include "main.h"

#include "lemlib/api.hpp" // IWYU pragma: keep

#include "config.h"
#include "shell.h"


// Controller
pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup left_motors({LF_PORT, LM_PORT, LR_PORT});
pros::MotorGroup right_motors({RF_PORT, RM_PORT, RR_PORT});

lemlib::Drivetrain drivetrain(&left_motors,
                              &right_motors,
                              10,
                              lemlib::Omniwheel::NEW_275,
                              450,
                              2
);

pros::Imu imu(IMU_PORT);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttle_curve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steer_curve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

lemlib::ControllerSettings lateral_controller(LkP,
                                              LkI,
                                              LkD,
                                              3,
                                              1,
                                              100,
                                              3,
                                              500,
                                              20
);

lemlib::ControllerSettings angular_controller(AkP,
                                              AkI,
                                              AkD,
                                              3,
                                              1,
                                              100,
                                              3,
                                              500,
                                              20
);

lemlib::OdomSensors sensors(nullptr,
                            nullptr,
                            nullptr,
                            nullptr,
                            &imu
);

lemlib::Chassis chassis(drivetrain,
                        lateral_controller,
                        angular_controller,
                        sensors,
                        &throttle_curve,
                        &steer_curve
);

void initialize() {
  pros::lcd::initialize();
  chassis.calibrate();

  // print position to brain screen
  pros::Task screen_task([&]() {
      while (true) {
          // print robot location to the brain screen
          pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
          pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
          pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
          // delay to save resources
          pros::delay(20);
        }
    });
}

void disabled() {}
void competition_initialize() {}

void autonomous() {
}

pros::Controller controller(pros::E_CONTROLLER_MASTER);

void opcontrol() {
  while (true) {
    int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

    #ifdef SHELL
    shell();
    #endif

    //oh

    chassis.arcade(leftY, rightX);

    pros::delay(25);
  }
}

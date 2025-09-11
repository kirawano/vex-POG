#include "main.h"

#include "lemlib/api.hpp" // IWYU pragma: keep

#include "config.h"
#include "shell.h"

// ================= Driver tuning: Expo curves =================
// All values are in controller units where stick range is [-127, 127].

// Deadband: ignore small joystick noise around center.
constexpr int kThrottleDeadband = 3;
constexpr int kSteerDeadband    = 3;

// Minimum output applied once outside the deadband.
// Helps overcome static friction --> easier to start moving.
constexpr int kThrottleMinOutput = 10;
constexpr int kSteerMinOutput    = 10;

// Expo gain: >1 curves response (more fine near center, more aggressive near ends).
// 1.0 is linear. Start around 1.02–1.04 for subtle curve.
constexpr float kThrottleExpoGain = 1.019f;
constexpr float kSteerExpoGain    = 1.019f;

// Arcade drive mapping (Arcade-only):
// Left stick Y  = throttle (forward/back)
// Right stick X = turn     (left/right)
// ===============================================================


// Primary handheld controller (V5 Master)
pros::Controller master(pros::E_CONTROLLER_MASTER);

// 6-motor drivetrain (3 per side)
pros::MotorGroup left_motors({LF_PORT, LM_PORT, LR_PORT});
pros::MotorGroup right_motors({RF_PORT, RM_PORT, RR_PORT});

// Drivetrain model for 6-wheel, 2.75" omni setup
lemlib::Drivetrain drivetrain(
    &left_motors,
    &right_motors,
    10,                            // track width (inches) — distance between left/right wheel midlines
    lemlib::Omniwheel::NEW_275,    // 2.75" omni wheels
    450,                           // wheel free speed (RPM) — effective at wheel
    2                              // external gear ratio (motor:wheel). Adjust if drive is geared.
);

// IMU for heading (and IMU-based odometry)
pros::Imu imu(IMU_PORT);

// Expo input curves for driver control (arcade mapping)
lemlib::ExpoDriveCurve throttle_curve(
    kThrottleDeadband,             // deadband
    kThrottleMinOutput,            // min output
    kThrottleExpoGain              // expo gain
);

lemlib::ExpoDriveCurve steer_curve(
    kSteerDeadband,                // deadband
    kSteerMinOutput,               // min output
    kSteerExpoGain                 // expo gain
);

// Motion control PID settings (values come from config.h)
lemlib::ControllerSettings lateral_controller(
    LkP, LkI, LkD,
    3, 1, 100,                     // tuning/settle parameters (see LemLib docs)
    3, 500, 20
);

lemlib::ControllerSettings angular_controller(
    AkP, AkI, AkD,
    3, 1, 100,                     // tuning/settle parameters (see LemLib docs)
    3, 500, 20
);

// Odometry sensors — using IMU-only (no external encoders)
lemlib::OdomSensors sensors(
    nullptr,                       // left tracking encoder
    nullptr,                       // right tracking encoder
    nullptr,                       // middle/lateral encoder
    nullptr,                       // vertical encoder (if used)
    &imu                           // IMU for heading
);

// High-level chassis wrapper (handles driver curves + motion control)
lemlib::Chassis chassis(
    drivetrain,
    lateral_controller,
    angular_controller,
    sensors,
    &throttle_curve,
    &steer_curve
);

void initialize() {
  pros::lcd::initialize();
  chassis.calibrate(); // calibrates IMU and prepares chassis systems

  // Realtime pose display on the brain screen.
  // Note: This task is local and will end when initialize() returns.
  // Make it static if you want it to persist during opcontrol/auton.
  pros::Task screen_task([&]() {
    while (true) {
      const auto pose = chassis.getPose();
      pros::lcd::print(0, "X: %f", pose.x);
      pros::lcd::print(1, "Y: %f", pose.y);
      pros::lcd::print(2, "Theta: %f", pose.theta);
      pros::delay(20);
    }
  });
}

void disabled() {}
void competition_initialize() {}

void autonomous() {
}

// Removed duplicate controller; use `master` declared above.
// pros::Controller controller(pros::E_CONTROLLER_MASTER);

void opcontrol() {
  while (true) {
    // Arcade inputs (Arcade-only):
    const int throttle = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);   // Left stick: forward/back
    const int turn     = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);  // Right stick: left/right

    #ifdef SHELL
    // Optional CLI for debugging/tuning (ensure this doesn't block)
    shell();
    #endif

    // Arcade drive using LemLib’s input curves and chassis mapping
    chassis.arcade(throttle, turn);

    pros::delay(25); // ~40 Hz loop
  }
}

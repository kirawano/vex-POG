#include "main.h"
#include <cmath>
#include "config.hpp"

constexpr bool LEFT_REVERSED  = false;
constexpr bool RIGHT_REVERSED = true;   // Typically true for mirrored sides

// Expo + smoothing tuning
constexpr double JOYSTICK_DEADBAND = 0.05; // ignore tiny stick noise
constexpr double EXPO_AMOUNT       = 0.35; // 0.0 = linear, 1.0 = very curved
constexpr double SMOOTH_ALPHA      = 0.22; // 0.05..0.3; higher = snappier
// =================================

// Controller
pros::Controller master(pros::E_CONTROLLER_MASTER);

// Drive motors (06:1 green, encoders in degrees)
pros::Motor mLF(LF_PORT, pros::E_MOTOR_GEARSET_06, LEFT_REVERSED,  pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor mLM(LM_PORT, pros::E_MOTOR_GEARSET_06, LEFT_REVERSED,  pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor mLR(LR_PORT, pros::E_MOTOR_GEARSET_06, LEFT_REVERSED,  pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor mRF(RF_PORT, pros::E_MOTOR_GEARSET_06, RIGHT_REVERSED, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor mRM(RM_PORT, pros::E_MOTOR_GEARSET_06, RIGHT_REVERSED, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor mRR(RR_PORT, pros::E_MOTOR_GEARSET_06, RIGHT_REVERSED, pros::E_MOTOR_ENCODER_DEGREES);

// Helpers
inline double clamp(double x, double lo, double hi) { return std::fmax(lo, std::fmin(hi, x)); }
inline double deadband(double x, double th) { return (std::fabs(x) < th) ? 0.0 : x; }

// Signed power for "expo" curve (nonlinear joystick feel)
// e in [1,3] typical; we blend linear with cubic using EXPO_AMOUNT for simplicity
inline double expoCurve(double x, double expoAmount) {
  // Blend: (1-expo)*x + expo*x^3 -> smooth near center, full near edges
  return (1.0 - expoAmount) * x + expoAmount * x * x * x;
}

// Exponential smoother (first-order IIR): y += alpha * (target - y)
struct ExpSmoother {
  double y = 0.0;
  double alpha = SMOOTH_ALPHA;
  void reset(double v = 0.0) { y = v; }
  double step(double target) {
    y += alpha * (target - y);
    return y;
  }
};

// Set both sides with voltage (-12000..12000)
void driveVoltage(int left_mV, int right_mV) {
  mLF.move_voltage(left_mV);
  mLM.move_voltage(left_mV);
  mLR.move_voltage(left_mV);
  mRF.move_voltage(right_mV);
  mRM.move_voltage(right_mV);
  mRR.move_voltage(right_mV);
}

void driveStop(pros::motor_brake_mode_e brake = pros::E_MOTOR_BRAKE_COAST) {
  mLF.set_brake_mode(brake); mLM.set_brake_mode(brake); mLR.set_brake_mode(brake);
  mRF.set_brake_mode(brake); mRM.set_brake_mode(brake); mRR.set_brake_mode(brake);
  driveVoltage(0, 0);
}

// Arcade mix: forward [-1..1], turn [-1..1] -> left/right [-1..1]
inline void arcadeMix(double forward, double turn, double &left, double &right) {
  left  = clamp(forward + turn, -1.0, 1.0);
  right = clamp(forward - turn, -1.0, 1.0);
}

// Simple timed move with smoothing (rudimentary autonomous helper)
void driveTimed(int ms, double forward, double turn = 0.0) {
  ExpSmoother lS, rS;
  const int dt = 10;
  double lT, rT;
  arcadeMix(forward, turn, lT, rT);
  for (int t = 0; t < ms; t += dt) {
    int l_mV = static_cast<int>(lS.step(lT) * 12000.0);
    int r_mV = static_cast<int>(rS.step(rT) * 12000.0);
    driveVoltage(l_mV, r_mV);
    pros::delay(dt);
  }
  driveStop(pros::E_MOTOR_BRAKE_COAST);
}

void initialize() {
  // Set initial brake modes
  mLF.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  mLM.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  mLR.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  mRF.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  mRM.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  mRR.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}

void disabled() {}
void competition_initialize() {}

void autonomous() {
  // Example: smooth forward, pause, smooth turn, stop
  driveTimed(1200, 0.8, 0.0); // forward 80% for 1.2s
  pros::delay(200);
  driveTimed(700, 0.0, 0.6);  // turn right 60% for 0.7s
  driveStop(pros::E_MOTOR_BRAKE_BRAKE);
}

void opcontrol() {
  ExpSmoother leftS, rightS;
  const int loopMs = 10;

  while (true) {
    // Read sticks, normalize to [-1..1]
    double fwd = -master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) / 127.0;  // up is negative -> invert
    double trn =  master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X) / 127.0;

    // Deadband then expo curve
    fwd = expoCurve(deadband(fwd, JOYSTICK_DEADBAND), EXPO_AMOUNT);
    trn = expoCurve(deadband(trn, JOYSTICK_DEADBAND), EXPO_AMOUNT);

    // Mix to sides
    double lTarget, rTarget;
    arcadeMix(fwd, trn, lTarget, rTarget);

    // Exponential speed increase (smoothing)
    int l_mV = static_cast<int>(leftS.step(lTarget) * 12000.0);
    int r_mV = static_cast<int>(rightS.step(rTarget) * 12000.0);

    driveVoltage(l_mV, r_mV);
    pros::delay(loopMs);
  }
}

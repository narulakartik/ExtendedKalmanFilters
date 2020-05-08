#include "FusionEKF.h"
#include <iostream>
#include "Eigen/Dense"
#include "tools.h"
#include <math.h>
#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;
using std::vector;


/**
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);
 // P_=MatrixXd(4,4);
 // F_=MatrixXd(4,4);
  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
              0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
              0, 0.0009, 0,
              0, 0, 0.09;
  H_laser_<< 1,0,0,0,0,1,0,0;
  
  ekf_.F_=MatrixXd(4,4);
  ekf_.F_<<1,0,1,0,
  		  0,1,0,1,
  		  0,0,1,0,
  	      0,0,0,1;
  ekf_.P_=MatrixXd(4,4);
  ekf_.P_<<1,0,1,0,
  		  0,1,0,1,
  		 0,0,1000,0,
  		0,0,0,1000;
  
    	
  //P<<1,0,0,0,
  //	 0,1,0,0,
   //  0,0,1000,0,
    // 0,0,0,1000;
  
    
  /**
   * TODO: Finish initializing the FusionEKF.
   * TODO: Set the process and measurement noises
   */
 
}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /**
   * Initialization
   */
  long previous_timestamp_;
  long new_timestamp_;
 
  if (!is_initialized_) {
    /**
     * TODO: Initialize the state ekf_.x_ with the first measurement.
     * TODO: Create the covariance matrix.
     * You'll need to convert radar from polar to cartesian coordinates.
     */

    // first measurement
    cout << "EKF: " << endl;
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1, 1, 1, 1;
    
    
    
    
    
    
    
    
    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      // TODO: Convert radar from polar to cartesian coordinates 
      //         and initialize state.
      float rho=measurement_pack.raw_measurements_[0];
      float phi=measurement_pack.raw_measurements_[1];
      float rhodot=measurement_pack.raw_measurements_[2];
      float x=rho*cos(phi);
      float y=rho*sin(phi);
      ekf_.x_<<x,y,0,0;
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      // TODO: Initialize state.
      float x=measurement_pack.raw_measurements_[0];
      float y=measurement_pack.raw_measurements_[1];
      ekf_.x_<<x,y,0,0;
    }
    previous_timestamp_=measurement_pack.timestamp_;
    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }
   new_timestamp_=measurement_pack.timestamp_;
  /**
   * Prediction
   */

  /**
   * TODO: Update the state transition matrix F according to the new elapsed time.
   * Time is measured in seconds.
   * TODO: Update the process noise covariance matrix.
   * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  float noise_ax=9;
  float noise_ay=9;
  
  float dt=(new_timestamp_-previous_timestamp_)/1000000;
  MatrixXd f;
  f=MatrixXd(4,4);
  f<<1,0,dt,0,
  0,1,0,dt;
  0,0,1,0;
  0,0,0,1;
  ekf_.F_=f;
  //ekf_.F_(0,2)=dt;
  //ekf_.F_(1,3)=dt;
  float dt_2 = dt * dt;
  float dt_3 = dt_2 * dt;
  float dt_4 = dt_3 * dt;
  MatrixXd q;
  q = MatrixXd(4, 4);
  q <<  dt_4/4*noise_ax, 0, dt_3/2*noise_ax, 0,
         0, dt_4/4*noise_ay, 0, dt_3/2*noise_ay,
         dt_3/2*noise_ax, 0, dt_2*noise_ax, 0,
         0, dt_3/2*noise_ay, 0, dt_2*noise_ay;
  ekf_.Predict();
  MatrixXd h_radar;
  h_radar=tools.CalculateJacobian(ekf_.x_);
  MatrixXd h_laser;
  h_laser=MatrixXd(2,4);
  
  h_laser<<1,0,0,0,0,1,0,0;
  /**
   * Update
   */

  /**
   * TODO:
   * - Use the sensor type to perform the update step.
   * - Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // TODO: Radar updates
    
     ekf_.Init(ekf_.x_, ekf_.P_, f, h_radar, R_radar_, q);
    
    
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);

  } else {
    // TODO: Laser updates
    
    ekf_.Init(ekf_.x_, ekf_.P_, f, h_laser, R_laser_, q);
    
    ekf_.Update(measurement_pack.raw_measurements_);
    
  }
  previous_timestamp_=new_timestamp_;
  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
  
}

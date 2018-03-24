/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * cartesian_mechanics.h
 *
 * Copyright (C) 2016 Alberto Cotronei @MagoKimbra
 */

#ifndef _CARTESIAN_MECHANICS_H_
#define _CARTESIAN_MECHANICS_H_

#if IS_CARTESIAN

  class Cartesian_Mechanics : public Mechanics {

    public: /** Constructor */

      Cartesian_Mechanics() {}

    public: /** Public Parameters */

      static const float  base_max_pos[XYZ],
                          base_min_pos[XYZ],
                          base_home_pos[XYZ],
                          max_length[XYZ];

      #if ENABLED(DUAL_X_CARRIAGE)
        static DualXMode  dual_x_carriage_mode;
        static float      inactive_hotend_x_pos,            // used in mode 0 & 1
                          raised_parked_position[NUM_AXIS], // used in mode 1
                          duplicate_hotend_x_offset;        // used in mode 2
        static int16_t    duplicate_hotend_temp_offset;     // used in mode 2
        static millis_t   delayed_move_time;                // used in mode 1
        static bool       active_hotend_parked,             // used in mode 1 & 2
                          hotend_duplication_enabled;       // used in mode 2
      #endif

    private: /** Private Parameters */

      #if ENABLED(HYSTERESIS)
        static float    m_hysteresis_axis_shift[XYZE],
                        m_hysteresis_mm[XYZE];
        static long     m_hysteresis_steps[XYZE];
        static uint8_t  m_hysteresis_prev_direction_bits,
                        m_hysteresis_bits;
      #endif

      #if ENABLED(ZWOBBLE)
        static float  m_zwobble_amplitude, m_zwobble_puls, m_zwobble_phase,
                      zwobble_zLut[STEPS_IN_ZLUT][2],
                      zwobble_lastZ, zwobble_lastZRod,
                      m_zwobble_scalingFactor;
        static bool   m_zwobble_consistent,
                      m_zwobble_sinusoidal;
        static int    zwobble_lutSize;
      #endif

    public: /** Public Function */

      /**
       * Initialize Cartesian parameters
       */
      static void init();

      /**
       * Home all axes according to settings
       *
       * Parameters
       *
       *  None  Home to all axes with no parameters.
       *        With QUICK_HOME enabled XY will home together, then Z.
       *
       * Cartesian parameters
       *
       *  X   Home to the X endstop
       *  Y   Home to the Y endstop
       *  Z   Home to the Z endstop
       *
       */
      static void home(const bool always_home_all);

      /**
       * Prepare a single move and get ready for the next one
       * If Mesh Bed Leveling is enabled, perform a mesh move.
       */
      static bool prepare_move_to_destination_mech_specific();

      /**
       * Set an axis' current position to its home position (after homing).
       *
       * For Cartesian robots this applies one-to-one when an
       * individual axis has been homed.
       *
       * Callers must sync the planner position after calling this!
       */
      static void set_axis_is_at_home(const AxisEnum axis);

      #if ENABLED(DUAL_X_CARRIAGE)
        static float x_home_pos(const int extruder);
      #endif

      #if ENABLED(HYSTERESIS)
        static void set_hysteresis_axis(uint8_t axis, float mm);
        static void report_hysteresis();
        static void insert_hysteresis_correction(const float x, const float y, const float z, const float e);
        static void calc_hysteresis_steps();
      #endif

      #if ENABLED(ZWOBBLE)
        #define STEPS_IN_ZLUT 50
        #define ZWOBBLE_MIN_Z 0.1
        // minimum distance within which two distances in mm are considered equal
        #define TOLERANCE_MM 0.01
        #define DISTANCE(_A,_B) abs((_A) - (_B))
        #define EQUAL_WITHIN_TOLERANCE(_A, _B) (DISTANCE(_A, _B) < TOLERANCE_MM)
        #define TWOPI 6.28318530718
        #define ZACTUAL_IS_SCALED(_I) (zwobble_zLut[_I][1] < 0)
        #define ZACTUAL(_I) (zwobble_zLut[_I][1] < 0 ? -zwobble_zLut[_I][1] * m_zwobble_scalingFactor : zwobble_zLut[_I][1])
        #define ZROD(_I) zwobble_zLut[_I][0]
        #define SET_ZACTUAL(_I,_V) zwobble_zLut[_I][1] = _V
        #define SET_ZROD(_I,_V) zwobble_zLut[_I][0] = _V
        static void report_zwobble();
        static void insert_zwobble_correction(const float targetZ);
        static void set_zwobble_amplitude(float _amplitude);
        static void set_zwobble_period(float _period);
        static void set_zwobble_phase(float _phase);
        static void set_zwobble_sample(float zRod, float zActual);
        static void set_zwobble_scaledsample(float zRod, float zScaledLength);
        static void set_zwobble_scalingfactor(float zActualPerScaledLength);
      #endif

    private: /** Private Function */

      /**
       *  Home axis
       */
      static void homeaxis(const AxisEnum axis);

      /**
       * Prepare a linear move in a Cartesian setup.
       * Bed Leveling will be applied to the move if enabled.
       *
       * Returns true if current_position[] was set to destination[]
       */
      static bool prepare_move_to_destination_cartesian();

      /**
       * Prepare a linear move in a dual X axis setup
       */
      #if ENABLED(DUAL_X_CARRIAGE)
        static bool prepare_move_to_destination_dualx();
        static int  x_home_dir(const uint8_t extruder) { return extruder ? X2_HOME_DIR : X_HOME_DIR; }
      #endif

      #if ENABLED(QUICK_HOME)
        static void quick_home_xy();
      #endif

      #if ENABLED(Z_SAFE_HOMING)
        static void home_z_safely();
      #endif

      #if ENABLED(DOUBLE_Z_HOMING)
        static void double_home_z();
      #endif

      #if ENABLED(HYSTERESIS)
        static void     set_hysteresis(float x_mm, float y_mm, float z_mm, float e_mm);
        static uint8_t  calc_direction_bits(const long *position, const long *target);
        static uint8_t  calc_move_bits(const long *position, const long *target);
      #endif

      #if ENABLED(ZWOBBLE)
        static void  calculateLut();
        static void  initLinearLut();
        static void  insertInLut(float, float);
        static float findInLut(float);
        static float findZRod(float);
        static bool  areParametersConsistent();
      #endif

  };

  extern Cartesian_Mechanics mechanics;

#endif // IS_CARTESIAN

#endif /* _CARTESIAN_MECHANICS_H_ */

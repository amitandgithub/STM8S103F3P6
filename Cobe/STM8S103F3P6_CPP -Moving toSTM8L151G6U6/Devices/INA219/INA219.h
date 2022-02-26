/*
* INA219.h
*
*  Created on: 4 Apr 2021
*      Author: Amit Chaudhary
*/

#ifndef INA219_h
#define INA219_h

#include "System.h"

namespace Devices
{

  template <typename I2c, uint8_t Device_Address>
  class INA219
  {
  public:
    typedef struct
    {
      float Voltage;
      float Current;
      float mAH;
    } Power_t;

    static void HwInit()
    {
      I2c::HwInit();
      SetCalibration_16V_400mA();
      //SetCalibration_32V_1A();
    }
    static void SetCalibration_32V_2A(void);
    static void SetCalibration_32V_1A(void);
    static void SetCalibration_16V_400mA(void);
    static float GetBusVoltage_V();
    static float GetCurrent_mA();
    static void Run(Power_t *pPower);
    static void Tests(void);
    
  private:
    static void WriteRegister(uint8_t reg, uint16_t value);
    static uint16_t ReadRegister(uint8_t reg);
    static int16_t GetBusVoltage_raw();
    static int16_t GetShuntVoltage_raw();
    static int16_t GetCurrent_raw();
    static float GetShuntVoltage_mV();
    
    static float GetPower_mW(void);
    static uint32_t GetMillis() { return 1; }
    static void CaptureSamples(Power_t *pPower);

  private:
    static uint16_t ina219_calValue;
    //static float    ina219_currentDivider_mA;
    static uint8_t mode;
    static uint16_t ina219_powerDivider_mW;
  };

  /*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/
#define INA219_ADDRESS (0x40) // 1000000 (A0+A1=GND)
#define INA219_READ (0x01)
  /*=========================================================================*/

  /*=========================================================================
  CONFIG REGISTER (R/W)
  -----------------------------------------------------------------------*/
#define INA219_REG_CONFIG (0x00)
  /*---------------------------------------------------------------------*/
#define INA219_CONFIG_RESET (0x8000) // Reset Bit

#define INA219_CONFIG_BVOLTAGERANGE_MASK (0x2000) // Bus Voltage Range Mask
#define INA219_CONFIG_BVOLTAGERANGE_16V (0x0000)  // 0-16V Range
#define INA219_CONFIG_BVOLTAGERANGE_32V (0x2000)  // 0-32V Range

#define INA219_CONFIG_GAIN_MASK (0x1800)    // Gain Mask
#define INA219_CONFIG_GAIN_1_40MV (0x0000)  // Gain 1, 40mV Range
#define INA219_CONFIG_GAIN_2_80MV (0x0800)  // Gain 2, 80mV Range
#define INA219_CONFIG_GAIN_4_160MV (0x1000) // Gain 4, 160mV Range
#define INA219_CONFIG_GAIN_8_320MV (0x1800) // Gain 8, 320mV Range

#define INA219_CONFIG_BADCRES_MASK (0x0780)  // Bus ADC Resolution Mask
#define INA219_CONFIG_BADCRES_9BIT (0x0080)  // 9-bit bus res = 0..511
#define INA219_CONFIG_BADCRES_10BIT (0x0100) // 10-bit bus res = 0..1023
#define INA219_CONFIG_BADCRES_11BIT (0x0200) // 11-bit bus res = 0..2047
#define INA219_CONFIG_BADCRES_12BIT (0x0400) // 12-bit bus res = 0..4097

#define INA219_CONFIG_SADCRES_MASK (0x0078)             // Shunt ADC Resolution and Averaging Mask
#define INA219_CONFIG_SADCRES_9BIT_1S_84US (0x0000)     // 1 x 9-bit shunt sample
#define INA219_CONFIG_SADCRES_10BIT_1S_148US (0x0008)   // 1 x 10-bit shunt sample
#define INA219_CONFIG_SADCRES_11BIT_1S_276US (0x0010)   // 1 x 11-bit shunt sample
#define INA219_CONFIG_SADCRES_12BIT_1S_532US (0x0018)   // 1 x 12-bit shunt sample
#define INA219_CONFIG_SADCRES_12BIT_2S_1060US (0x0048)  // 2 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_4S_2130US (0x0050)  // 4 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_8S_4260US (0x0058)  // 8 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_16S_8510US (0x0060) // 16 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_32S_17MS (0x0068)   // 32 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_64S_34MS (0x0070)   // 64 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_128S_69MS (0x0078)  // 128 x 12-bit shunt samples averaged together

#define INA219_CONFIG_MODE_MASK (0x0007) // Operating Mode Mask
#define INA219_CONFIG_MODE_POWERDOWN (0x0000)
#define INA219_CONFIG_MODE_SVOLT_TRIGGERED (0x0001)
#define INA219_CONFIG_MODE_BVOLT_TRIGGERED (0x0002)
#define INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED (0x0003)
#define INA219_CONFIG_MODE_ADCOFF (0x0004)
#define INA219_CONFIG_MODE_SVOLT_CONTINUOUS (0x0005)
#define INA219_CONFIG_MODE_BVOLT_CONTINUOUS (0x0006)
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS (0x0007)
  /*=========================================================================*/

  /*=========================================================================
  SHUNT VOLTAGE REGISTER (R)
  -----------------------------------------------------------------------*/
#define INA219_REG_SHUNTVOLTAGE (0x01)
  /*=========================================================================*/

  /*=========================================================================
  BUS VOLTAGE REGISTER (R)
  -----------------------------------------------------------------------*/
#define INA219_REG_BUSVOLTAGE (0x02)
  /*=========================================================================*/

  /*=========================================================================
  POWER REGISTER (R)
  -----------------------------------------------------------------------*/
#define INA219_REG_POWER (0x03)
  /*=========================================================================*/

  /*=========================================================================
  CURRENT REGISTER (R)
  -----------------------------------------------------------------------*/
#define INA219_REG_CURRENT (0x04)
  /*=========================================================================*/

  /*=========================================================================
  CALIBRATION REGISTER (R/W)
  -----------------------------------------------------------------------*/
#define INA219_REG_CALIBRATION (0x05)
  /*=========================================================================*/

  template <typename I2c, uint8_t Device_Address>
  void INA219<I2c,Device_Address>::WriteRegister(uint8_t reg, uint16_t value)
  {
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = ((value >> 8) & 0xFF);
    buf[2] = (value & 0xFF);
    I2c::Tx(Device_Address, buf, 3);
  }
  
  template <typename I2c, uint8_t Device_Address>
    uint16_t INA219<I2c,Device_Address>::ReadRegister(uint8_t reg)
    {
      uint8_t buf[3];
      buf[0] = reg;
      I2c::Tx(Device_Address, buf, 1);
      I2c::Rx(Device_Address, buf, 2);
      return  ( ((uint16_t)buf[0] << 8U) | buf[1]);
    }

  /**************************************************************************/
  /*!
  @brief  Configures to INA219 to be able to measure up to 32V and 2A
  of current.  Each unit of current corresponds to 100uA, and
  each unit of power corresponds to 2mW. Counter overflow
  occurs at 3.2A.
  
  @note   These calculations assume a 0.1 ohm resistor is present
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  void INA219<I2c,Device_Address>::SetCalibration_32V_2A(void)
  {

    // By default we use a pretty huge range for the input voltage,
    // which probably isn't the most appropriate choice for system
    // that don't use a lot of power.  But all of the calculations
    // are shown below if you want to change the settings.  You will
    // also need to change any relevant register settings, such as
    // setting the VBUS_MAX to 16V instead of 32V, etc.

    // VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
    // VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
    // RSHUNT = 0.1               (Resistor value in ohms)

    // 1. Determine max possible current
    // MaxPossible_I = VSHUNT_MAX / RSHUNT
    // MaxPossible_I = 3.2A

    // 2. Determine max expected current
    // MaxExpected_I = 2.0A

    // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
    // MinimumLSB = MaxExpected_I/32767
    // MinimumLSB = 0.000061              (61uA per bit)
    // MaximumLSB = MaxExpected_I/4096
    // MaximumLSB = 0,000488              (488uA per bit)

    // 4. Choose an LSB between the min and max values
    //    (Preferrably a roundish number close to MinLSB)
    // CurrentLSB = 0.0001 (100uA per bit)

    // 5. Compute the calibration register
    // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
    // Cal = 4096 (0x1000)
    uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                      INA219_CONFIG_GAIN_8_320MV |
                      INA219_CONFIG_BADCRES_12BIT |
                      INA219_CONFIG_SADCRES_12BIT_128S_69MS | // INA219_CONFIG_SADCRES_12BIT_128S_69MS  INA219_CONFIG_SADCRES_12BIT_1S_532US
                      INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

    ina219_calValue = 4096;
    
    if(mode == 3) return;
    mode = 3;

    // 6. Calculate the power LSB
    // PowerLSB = 20 * CurrentLSB
    // PowerLSB = 0.002 (2mW per bit)

    // 7. Compute the maximum current and shunt voltage values before overflow
    //
    // Max_Current = Current_LSB * 32767
    // Max_Current = 3.2767A before overflow
    //
    // If Max_Current > Max_Possible_I then
    //    Max_Current_Before_Overflow = MaxPossible_I
    // Else
    //    Max_Current_Before_Overflow = Max_Current
    // End If
    //
    // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
    // Max_ShuntVoltage = 0.32V
    //
    // If Max_ShuntVoltage >= VSHUNT_MAX
    //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
    // Else
    //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
    // End If

    // 8. Compute the Maximum Power
    // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
    // MaximumPower = 3.2 * 32V
    // MaximumPower = 102.4W

    // Set multipliers to convert raw current/power values
    //ina219_currentDivider_mA = 0.025;//10; // Current LSB = 100uA per bit (1000/100 = 10)
    ina219_powerDivider_mW = 2;    // Power LSB = 1mW per bit (2/1)

    // Set Calibration register to 'Cal' calculated above
    WriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Set Config register to take into account the settings above
    WriteRegister(INA219_REG_CONFIG, config);
  }

  /**************************************************************************/
  /*!
  @brief  Configures to INA219 to be able to measure up to 32V and 1A
  of current.  Each unit of current corresponds to 40uA, and each
  unit of power corresponds to 800�W. Counter overflow occurs at
  1.3A.
  
  @note   These calculations assume a 0.1 ohm resistor is present
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  void INA219<I2c,Device_Address>::SetCalibration_32V_1A(void)
  {
    // By default we use a pretty huge range for the input voltage,
    // which probably isn't the most appropriate choice for system
    // that don't use a lot of power.  But all of the calculations
    // are shown below if you want to change the settings.  You will
    // also need to change any relevant register settings, such as
    // setting the VBUS_MAX to 16V instead of 32V, etc.

    // VBUS_MAX = 32V		(Assumes 32V, can also be set to 16V)
    // VSHUNT_MAX = 0.32	(Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
    // RSHUNT = 0.1			(Resistor value in ohms)

    // 1. Determine max possible current
    // MaxPossible_I = VSHUNT_MAX / RSHUNT
    // MaxPossible_I = 3.2A

    // 2. Determine max expected current
    // MaxExpected_I = 1.0A

    // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
    // MinimumLSB = MaxExpected_I/32767
    // MinimumLSB = 0.0000305             (30.5�A per bit)
    // MaximumLSB = MaxExpected_I/4096
    // MaximumLSB = 0.000244              (244�A per bit)

    // 4. Choose an LSB between the min and max values
    //    (Preferrably a roundish number close to MinLSB)
    // CurrentLSB = 0.0000400 (40�A per bit)

    // 5. Compute the calibration register
    // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
    // Cal = 10240 (0x2800)

    uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                      INA219_CONFIG_GAIN_8_320MV |
                      INA219_CONFIG_BADCRES_12BIT |
                      INA219_CONFIG_SADCRES_12BIT_128S_69MS |
                      INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

    ina219_calValue = 10240;
    
    if(mode == 2) return;
    
    mode = 2;
    // 6. Calculate the power LSB
    // PowerLSB = 20 * CurrentLSB
    // PowerLSB = 0.0008 (800�W per bit)

    // 7. Compute the maximum current and shunt voltage values before overflow
    //
    // Max_Current = Current_LSB * 32767
    // Max_Current = 1.31068A before overflow
    //
    // If Max_Current > Max_Possible_I then
    //    Max_Current_Before_Overflow = MaxPossible_I
    // Else
    //    Max_Current_Before_Overflow = Max_Current
    // End If
    //
    // ... In this case, we're good though since Max_Current is less than MaxPossible_I
    //
    // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
    // Max_ShuntVoltage = 0.131068V
    //
    // If Max_ShuntVoltage >= VSHUNT_MAX
    //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
    // Else
    //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
    // End If

    // 8. Compute the Maximum Power
    // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
    // MaximumPower = 1.31068 * 32V
    // MaximumPower = 41.94176W

    // Set multipliers to convert raw current/power values
    //ina219_currentDivider_mA = 0.02;//50;//25; // Current LSB = 40uA per bit (1000/40 = 25)
    ina219_powerDivider_mW = 1;    // Power LSB = 800�W per bit

    // Set Calibration register to 'Cal' calculated above
    WriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Set Config register to take into account the settings above

    WriteRegister(INA219_REG_CONFIG, config);
  }

template <typename I2c, uint8_t Device_Address>
  void INA219<I2c,Device_Address>::SetCalibration_16V_400mA(void)
  {
    // Calibration which uses the highest precision for
    // current measurement (0.1mA), at the expense of
    // only supporting 16V at 400mA max.

    // VBUS_MAX = 16V
    // VSHUNT_MAX = 0.04          (Assumes Gain 1, 40mV)
    // RSHUNT = 0.1               (Resistor value in ohms)

    // 1. Determine max possible current
    // MaxPossible_I = VSHUNT_MAX / RSHUNT
    // MaxPossible_I = 0.4A

    // 2. Determine max expected current
    // MaxExpected_I = 0.4A

    // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
    // MinimumLSB = MaxExpected_I/32767
    // MinimumLSB = 0.0000122              (12uA per bit)
    // MaximumLSB = MaxExpected_I/4096
    // MaximumLSB = 0.0000977              (98uA per bit)

    // 4. Choose an LSB between the min and max values
    //    (Preferrably a roundish number close to MinLSB)
    // CurrentLSB = 0.00005 (50uA per bit)

    // 5. Compute the calibration register
    // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
    // Cal = 8192 (0x2000)

    uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                      INA219_CONFIG_GAIN_1_40MV |
                      INA219_CONFIG_BADCRES_12BIT |
                      INA219_CONFIG_SADCRES_12BIT_128S_69MS |
                      INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
    ina219_calValue = 8192;
    
   if(mode == 1) return;
     
    mode = 1;

    // 6. Calculate the power LSB
    // PowerLSB = 20 * CurrentLSB
    // PowerLSB = 0.001 (1mW per bit)

    // 7. Compute the maximum current and shunt voltage values before overflow
    //
    // Max_Current = Current_LSB * 32767
    // Max_Current = 1.63835A before overflow
    //
    // If Max_Current > Max_Possible_I then
    //    Max_Current_Before_Overflow = MaxPossible_I
    // Else
    //    Max_Current_Before_Overflow = Max_Current
    // End If
    //
    // Max_Current_Before_Overflow = MaxPossible_I
    // Max_Current_Before_Overflow = 0.4
    //
    // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
    // Max_ShuntVoltage = 0.04V
    //
    // If Max_ShuntVoltage >= VSHUNT_MAX
    //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
    // Else
    //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
    // End If
    //
    // Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
    // Max_ShuntVoltage_Before_Overflow = 0.04V

    // 8. Compute the Maximum Power
    // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
    // MaximumPower = 0.4 * 16V
    // MaximumPower = 6.4W

    // Set multipliers to convert raw current/power values
    //ina219_currentDivider_mA = 0.05;//40;//20; // Current LSB = 50uA per bit (1000/50 = 20)
    ina219_powerDivider_mW = 1;    // Power LSB = 1mW per bit

    // Set Calibration register to 'Cal' calculated above
    WriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Set Config register to take into account the settings above

    WriteRegister(INA219_REG_CONFIG, config);
  }

  /**************************************************************************/
  /*!
  @brief  Gets the raw bus voltage (16-bit signed integer, so +-32767)
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  int16_t INA219<I2c,Device_Address>::GetBusVoltage_raw()
  {
    // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
    return (int16_t)( (ReadRegister(INA219_REG_BUSVOLTAGE) >> 3) * 4);
  }

  /**************************************************************************/
  /*!
  @brief  Gets the raw shunt voltage (16-bit signed integer, so +-32767)
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  int16_t INA219<I2c,Device_Address>::GetShuntVoltage_raw()
  {
    return (int16_t)ReadRegister(INA219_REG_SHUNTVOLTAGE);
  }

  /**************************************************************************/
  /*!
  @brief  Gets the raw current value (16-bit signed integer, so +-32767)
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  int16_t INA219<I2c,Device_Address>::GetCurrent_raw()
  {
    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    WriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Now we can safely read the CURRENT register!
    //ReadRegister(INA219_REG_CURRENT, &value);

    return (int16_t)ReadRegister(INA219_REG_CURRENT);
  }

  /**************************************************************************/
  /*!
  @brief  Gets the shunt voltage in mV (so +-327mV)
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  float INA219<I2c,Device_Address>::GetShuntVoltage_mV()
  {
    return GetShuntVoltage_raw() * 0.01;
  }

  /**************************************************************************/
  /*!
  @brief  Gets the shunt voltage in volts
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  float INA219<I2c,Device_Address>::GetBusVoltage_V()
  {
    return GetBusVoltage_raw() * 0.001;
  }

  /**************************************************************************/
  /*!
  @brief  Gets the current value in mA, taking into account the
  config settings and current LSB
  */
  /**************************************************************************/
  template <typename I2c, uint8_t Device_Address>
  float INA219<I2c,Device_Address>::GetCurrent_mA()
  {
    float valueDec = GetCurrent_raw();

    if(mode == 1)
    {
      return valueDec*0.05;//0.025;
    }
    else if(mode == 2)
    {
      return valueDec*0.04;// 0.02
    }
    else
    {
      return valueDec*0.1; //0.05;
    }
  }

template <typename I2c, uint8_t Device_Address>
  float INA219<I2c,Device_Address>::GetPower_mW(void)
  {
    uint16_t value_Raw;
    float valueDec;

    WriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    value_Raw = ReadRegister(INA219_REG_POWER);
    valueDec = (int16_t)value_Raw;
    valueDec /= ina219_powerDivider_mW;
    return valueDec;
  }

  template <typename I2c, uint8_t Device_Address>
    void INA219<I2c,Device_Address>::Tests(void)
    {
      volatile static Power_t Power;
      
      HwInit();
      
      while(1)
      {
        Power.Voltage = GetBusVoltage_V();
        Power.Current = GetCurrent_mA();
        delay_ms(300);
      }
    }
  
  
  template <typename I2c, uint8_t Device_Address>
    uint16_t INA219<I2c,Device_Address>::ina219_calValue;
  
//  template <typename I2c, uint8_t Device_Address>
//    float INA219<I2c,Device_Address>::ina219_currentDivider_mA;
  
  template <typename I2c, uint8_t Device_Address>
    uint8_t INA219<I2c,Device_Address>::mode;
  
  template <typename I2c, uint8_t Device_Address>
    uint16_t INA219<I2c,Device_Address>::ina219_powerDivider_mW;
  
  
  
} /* namespace */

#endif 

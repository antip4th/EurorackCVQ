%% Two Point Calibration
% Calculates the correction values out of two measurements per converter.

% mV      | LSB
% -----------------
% 1000    | 6554
% 2000    | 13107
% 3000    | 19661
% 5000    | 32768
% 7000    | 45875
% 8000    | 52429
% 9000    | 58982	

Vpp = 10;

calibMeasInputVoltage  = [ ]; % Voltages applied to the input in mV
calibMeasInputInteger  = [6131, 58698]; % uint16 given by the ADC

calibMeasOutputVoltage = [ ]; % Voltages measured at the output in mV
calibMeasOutputInteger = [6554, 58982]; % uint16 sent to the DAC

% transform measured voltages to int16 values
calibInputIntNominal  = calibMeasInputVoltage/(1000*Vpp) * 2^16;
calibOutputIntNominal = calibMeasOutputVoltage/(1000*Vpp) * 2^16;

% calculate the errors, using the integer variables
[CgIn, CoIn]   = calcCalibVals( calibMeasInputInteger(1), calibMeasInputInteger(2),  calibInputIntNominal(1),  calibInputIntNominal(2));
[CgOut, CoOut] = calcCalibVals( calibMeasOutputInteger(1), calibMeasOutputInteger(2), calibOutputIntNominal(1), calibOutputIntNominal(2));

fprintf("Input: C_g = %f, C_o = %.f\n", CgIn, CoIn);
fprintf("Output: C_g = %f, C_o = %.f\n", CgOut, CoOut);

% check values: calculate remaining error in LSB

calibMeasInputInteger(1)*CgIn + CoIn - calibInputIntNominal(1)
calibMeasInputInteger(2)*CgIn + CoIn - calibInputIntNominal(2)
calibMeasOutputInteger(1)*CgOut + CoOut - calibOutputIntNominal(1)
calibMeasOutputInteger(2)*CgOut + CoOut - calibOutputIntNominal(2)

function [Cg, Co] = calcCalibVals(v_r1, v_r2, v_c1, v_c2)
    Cg = (v_c2-v_c1)/(v_r2-v_r1);
    Co = round((v_r2*v_c1 - v_r1*v_c2)/(v_r2-v_r1));
end
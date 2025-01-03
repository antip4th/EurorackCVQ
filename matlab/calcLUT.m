close all
clear
clc

% Script to claculate the 16-Bit output values for 16-Bit input values, 
% to quantize to semitones

% define constants

% including 0V, there are 121 discrete semitone steps in the range from 
% 0V to 10V with 1V/Octave

Vpp = 10; % Volt
nST = 121;
nLSB = 2^16;
STmargin = 3;

% Number of semitones to remove from the top and bottom of 
% the List as a margin to gain/offset-erros in the in/output
% stages

% assign variables
[mVST, lsbST] = deal(zeros(nST,1));
STthres        = zeros(nST-1, 1);
LUT = zeros(nLSB, 1);

% calculation

mVperLSB = (Vpp*1000)/nLSB;
mVperST  = (Vpp*1000)/(nST-1);
LSBperST = (nLSB-1)/(nST-1);

for st = 1:nST
    mVST(st)    = mVperST*(st-1);
    lsbST(st) = round(LSBperST*(st-1));

    if st == nST
        STthres(st) = round(LSBperST*(st-1));
    else
        STthres(st) = round(LSBperST*st - LSBperST/2);
    end
end

% calculate LUT for ideal conditions:
% 
% for i = 1:nST
%     if i == 1
%         LUT(1:STthres(i)) = lsbST(i);
%     else
%         LUT(STthres(i-1)+1:STthres(i)+1) = lsbST(i);
%     end
% end

% calculate LUT for real conditions:
% STmargin semitones are "removed" at top and bottom of the list by setting
% this range in LUT to the "first/last allowed tone"

LUT(1:STthres(STmargin)) = lsbST(STmargin);
LUT(STthres(end-(STmargin-1)):end) = lsbST(end-(STmargin-1));

for i = STmargin:length(STthres)-(STmargin-1)
    LUT(STthres(i-1)+1:STthres(i)+1) = lsbST(i);
end

% plot LUT characteristic
f = figure();
plot(0:nLSB-1,LUT)
xlim([0 nLSB-1]);
ylim([0 nLSB-1]);
grid;
title("Kennline der conversion-LUT");
xlabel("Eingangswert/LSB");
ylabel("Ausgangswert/LSB");
f.WindowState = "maximized";
fontsize(f,14,"points");

writeLUTHeader(LUT);


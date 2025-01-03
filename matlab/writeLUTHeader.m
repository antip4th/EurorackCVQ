function writeLUTHeader(LUT)
fid = fopen("conversionLUT.h","w");
fprintf(fid, 'uint16_t conversionLUT');
fprintf(fid,['[',num2str(length(LUT)),']={\n']);
k = 0;
for p = 1 : length(LUT)
   fprintf(fid,' %6.0f,', LUT(p));
   k = k + 1;
   if k > 15
     fprintf(fid, '\n');
     k = 0;
  end
end
fprintf(fid,'};\n\n');

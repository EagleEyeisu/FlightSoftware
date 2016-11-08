clear, clc
close all


%use import tool to import the data from the text file
data = importdata('Nov_5MATLABdata.txt');
time = data(:,1);
time_t = transpose(time);
alt = data(:,2);
pressure = data(:,3);
pressure_p = transpose(pressure);
temp = data(:,4);
temp_a = transpose(temp);

%Calculation of data length
l = size(data);
length = l(1);

%preallocation of matrices
velocity = zeros(1,length);
alt_km = zeros(1,length);
alt_calc = zeros(1,length);
time_min = zeros(1,length);
alt_standard = zeros(1,120000);
temp_standard = zeros(1,120000);
pres_standard = zeros(1,120000);
Altitude = zeros(1,length);

for i=1:length
    pressure_kpa(i) = pressure(i)/10.0;
end

SeaP = 1026.4/10.0;

%adjustment of time from sec to minutes
for i=1:length
    time_min(i) = time(i)/60;
end

for j=1:length
    if pressure_kpa(j)<37.65
        %temp_standard(j) = 15.04 - .00649*h;
        alt_calc(j) = (-47454.8*(pressure_kpa(j)^(0.087812)-1.65374))/(pressure_kpa(j)^(0.087812));
    elseif pressure_kpa(i)>=67.05 & pressure_kpa(i)<37.65
        %temp_standard(j) = -56.46;
        alt_calc(j) = -6369.43*(ln(pressure_kpa(j))-4.85016);
    else
        %temp_standard(j) = -131.21 + 0.00299*h;
        alt_calc(j) = 44397.5-18437*pressure_kpa(j)^(0.190259);        
    end
end

for i=1:length
    alt_feet = alt_calc*3.28084;
end

% for i = 1:length
%     x=SeaP/pressure_kpa(i)
%     alt_calc(i) = ln(x)*7000;
% end

maxAlt = max(alt_calc)*3.28084;



fprintf('the max altitude is:')
fprintf('%f',maxAlt)
fprintf('\n')

figure(1)
plot(time_min,alt_feet,'LineWidth',2);
title('altitude progression')
xlabel('Time (min)')
ylabel('Altitude (feet)')
grid on

clear, clc

%use import tool to import the data from the text file
data = importdata('FLTDATA.txt');
time = data(:,1);
time_t = transpose(time);
alt = data(:,2);
pressure = data(:,3);
pressure_p = transpose(pressure);
temp = data(:,4);
temp_a = transpose(temp);

fprintf('the min temp is:')
fprintf('%f',min(temp))
fprintf('\n')

fprintf('the min Pressure is:')
fprintf('%f',min(pressure))
fprintf('\n')

%Calculation of data length
l = size(data);
length = l(1);

%preallocation of matrices
velocity = zeros(1,length);
alt_km = zeros(1,length);
time_min = zeros(1,length);
alt_standard = zeros(1,120000);
temp_standard = zeros(1,120000);
pres_standard = zeros(1,120000);
Altitude = zeros(1,length);

%Adjustment of altitude from m to km
for i=1:length
    alt_km(i) = alt(i)/1000;
end

%adjustment of time from sec to minutes
for i=1:length
    time_min(i) = time(i)/60;
end

%calculation of velocity
for i=1:length/10
  if i == 1
      velocity(i) = 0 ; 
  else
      velocity(i) = (alt(i*10)-alt(i*10-10))/(time(i*10)-time(i*10-10));
  end
end
t = (1:1250);

%Calculating Standard Atmosphere models
%multiply pressure by 10 to convert kPa to mbar
for h=1:120000
    if h<11000
        temp_standard(h) = 15.04 - .00649*h;
        pres_standard(h) = (101.29*((temp_standard(h)+273.1)/288.08)^(5.256))*10;
        alt_standard(h) = h;
    elseif h>=11000 & h<25000
        temp_standard(h) = -56.46;
        pres_standard(h) = (22.65*exp(1.73 - 0.000157*h))*10;
        alt_standard(h) = h;
    else
        temp_standard(h) = -131.21 + 0.00299*h;
        pres_standard(h) = (2.448*((temp_standard(h)+273.1)/216.6)^(-11.388))*10;
        alt_standard(h) = h;        
    end
end

for hi = 1:length
    Altitude(hi) = hi;
end

v = smooth(velocity,10);
temp_smooth = smooth(temp,10);
temp_smooth2 = smooth(temp_smooth,10);

fprintf('the max velocity is:')
fprintf('%f',abs(min(velocity)))
fprintf('\n')

fprintf('the max altitude is:')
fprintf('%f',max(alt))
fprintf('\n')

figure(1)
plot(time_min,alt,'LineWidth',2);
title('altitude progression')
xlabel('Time (min)')
ylabel('Altitude (km)')

figure(2)
plot(time_min,pressure,'LineWidth',2);
title('Pressure progression')
xlabel('Time (min)')
ylabel('Pressure (mb)')

figure(3)
plot(time_min,temp_smooth2,'LineWidth',2);
title('Temperature progression')
xlabel('Time (min)')
ylabel('Temperature (C)')

figure(4)
plot(time,velocity);
title('Instantaneous Velocity progression')
xlabel('Time (sec)')
ylabel('Instantaneous Velocity (m/s)')

figure(5)
plot(temp_standard,alt_standard,temp_smooth2,Altitude,'LineWidth',2);
title('Standard Atmosphere Temperature Gradient')
xlabel('Temperature (Celsius)')
ylabel('Altitude (m)')
legend('Standard Atmospheric model','Recorded Data')
grid on

figure(6)
plot(pres_standard,alt_standard,'LineWidth',2);
title('Standard Atmosphere Pressure Gradient')
ylabel('Altitude (m)')
xlabel('Pressure (K-Pa)')
legend('Standard Atmospheric model')
grid on

figure(7)
plot(pressure, alt, 'LineWidth',2)
title('Recorded Data: Altitude vs Pressure')
ylabel('Altitude (m)')
xlabel('Pressure (K-Pa)')
legend('Recorded Data')
grid on

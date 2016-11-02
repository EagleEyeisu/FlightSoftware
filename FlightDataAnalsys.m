clear, clc

%use import tool to import the data from the text file
%
data = importdata('Oct_3MATLABdata.txt');
time = data(:,1);
alt = data(:,2);
pressure = data(:,3);
temp = data(:,4);

l = size(data);
length = l(1);

velocity = zeros(1,length);
alt_km = zeros(1,length);
time_min = zeros(1,length);

for i=1:length
    alt_km(i) = alt(i)/1000;
end

for i=1:length
    time_min(i) = time(i)/60;
end

%for i=1:length
%   if i == 1
%       velocity(i) = 0 ; 
%   else
%       velocity(i) = (alt(i)-alt(i-1))/(time(i)-time(i-1));
%   end
%end

v = smooth(velocity,10);
max_alt = max(alt)

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
plot(time_min,temp,'LineWidth',2);
title('Temperature progression')
xlabel('Time (min)')
ylabel('Temperature (C)')

%figure(4)
%plot(time,velocity);
%title('Instantaneous Velocity progression')
%xlabel('Time (sec)')
%ylabel('Instantaneous Velocity (m/s)')

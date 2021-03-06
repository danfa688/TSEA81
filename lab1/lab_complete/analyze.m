MAX_ITERATIONS=2048
DELAY_IN_USECS=1000



%system('make');
%system('taskset -c 0 ./lab');
load sample_times.txt
load output.txt
fid=fopen('data.raw','r');
raw_data = fread(fid,Inf, 'int8');
fclose(fid);

raw_data = [raw_data; raw_data; raw_data; raw_data; raw_data; raw_data; raw_data; raw_data];

ideal_output = [raw_data(1:DELAY_IN_USECS:(DELAY_IN_USECS*(MAX_ITERATIONS)))];


figure
plot(abs(diff(sample_times(:,1))));
title('Time difference between samples');
ylabel('Time difference [s]');
xlabel('Sample no');




figure
plot(output(:,1))
title('Sampled input data');
xlabel('Sample no');
ylabel('Sample value');

figure
plot(ideal_output(:,1) - output(:,1));
%hold on
%plot(ideal_output(:,1) - output(:,2),'r');
title('Difference between ideally sampled data and experimental results');



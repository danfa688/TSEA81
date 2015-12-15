load output.txt
thread_40=output;
%%
load broadcast.mat

close all
figure
hold on
plot(0:1:length(thread_1)-1,thread_1,'r*')
plot(0:1:length(thread_10)-1,thread_10,'g*')
plot(0:1:length(thread_20)-1,thread_20,'k*')
plot(0:1:length(thread_40)-1,thread_40,'b*')
hold off
legend('1 thread','10 threads','20 threads','40 threads','location', 'southeast')
xlabel('Thread ID');
ylabel('Time [us]');
axis ([0 39 0 inf])

figure
plot([1 10 20 30 40 50 60 70], [mean(thread_1) mean(thread_10) mean(thread_20) mean(thread_30) mean(thread_40) ...
    mean(thread_50) mean(thread_60) mean(thread_70)])
xlabel('Number of thread');
ylabel('Mean time [us]');
axis ([1 70 0 inf])

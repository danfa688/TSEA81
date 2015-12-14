load output.txt
thread_1=output;
%%
load braodcast.mat

close all
figure
hold on
stem(0:1:length(thread_1)-1,thread_1,'r')
plot(0:1:length(thread_10)-1,thread_10,'g')
plot(0:1:length(thread_20)-1,thread_20,'y')
plot(0:1:length(thread_40)-1,thread_40,'b')
hold off
legend('1 thread','10 threads','20 threads','40 threads','location', 'southeast')
xlabel('Thread ID');
ylabel('Time [us]');

figure
plot([1 10 20 40], [mean(thread_1) mean(thread_10) mean(thread_20) mean(thread_40)])
xlabel('Number of thread');
ylabel('Mean time [us]');
axis ([1 40 0 inf])

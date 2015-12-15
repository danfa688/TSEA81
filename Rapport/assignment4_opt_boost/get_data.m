load output.txt
planed_trips_100=output;
%%

close all
% figure
% hold on
% plot(0:1:length(thread_1)-1,thread_1,'r*')
% plot(0:1:length(thread_10)-1,thread_10,'g*')
% plot(0:1:length(thread_20)-1,thread_20,'k*')
% plot(0:1:length(thread_40)-1,thread_40,'b*')
% hold off
% legend('1 thread','10 threads','20 threads','40 threads','location', 'southeast')
% xlabel('Thread ID');
% ylabel('Time [us]');
% axis ([0 39 0 inf])

load person_dat_4_opt

h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_4_opt', 'epsc')

load message_dat_4_opt

h=figure;
plot([1 10 20 40 50 80 100], [mean(planed_trips_1) mean(planed_trips_10) mean(planed_trips_20) mean(planed_trips_40) ...
    mean(planed_trips_50) mean(planed_trips_80) mean(planed_trips_100)])
xlabel('Number of planned trips (message length)');
ylabel('Mean time [us]');
axis ([1 100 0 inf])
saveas(h, 'figures/message_length_4_opt', 'epsc')

load braodcast

h=figure
plot([1 10 20 40], [mean(thread_1) mean(thread_10) mean(thread_20) mean(thread_40)])
xlabel('Number of thread');
ylabel('Mean time [us]');
axis ([1 40 0 inf])
saveas(h, 'figures/mean_broadcast', 'epsc')


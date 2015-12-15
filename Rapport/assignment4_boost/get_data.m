load output.txt
persons_70=output;
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

load person_dat_4

h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_4', 'epsc')



close all

%Assignemnt 3%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load person_dat_3
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_3', 'epsc')

load person_dat_3_opt
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_3_opt', 'epsc')

%In same plot%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load person_dat_3
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])

hold on

load person_dat_3_opt
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
legend('Raw', 'Optimised', 'location','northwest')
hold off
saveas(h, 'figures/number_of_persons_3_together', 'epsc')

%Assignment 4%%%%%%%%%%%%%%%%%%%%%%%%%%%

load person_dat_4
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_4', 'epsc')

load person_dat_4_opt
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_4_opt', 'epsc')

%In same plot%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

load person_dat_4
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])

hold on

load person_dat_4_opt

plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
legend('Raw', 'Optimised', 'location','northwest')
hold off
saveas(h, 'figures/number_of_persons_4_together', 'epsc')

%%%%%%%%%Message length%%%%%%%%%%%%%%%%%%%%%%%%

load message_dat_4_opt

h=figure;
plot([1 10 20 40 50 80 100], [mean(planed_trips_1) mean(planed_trips_10) mean(planed_trips_20) mean(planed_trips_40) ...
    mean(planed_trips_50) mean(planed_trips_80) mean(planed_trips_100)])
xlabel('Number of planned trips (message length)');
ylabel('Mean time [us]');
axis ([1 100 0 inf])
saveas(h, 'figures/message_length_4_opt', 'epsc')

%assignment 3, 4 opt together%%%%%%%%%%%%%%%%%
load person_dat_3_opt
h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])

hold on

load person_dat_4_opt

plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
legend('threads', 'processes', 'location','northwest')
hold off
saveas(h, 'figures/number_of_persons_3_4_opt_together', 'epsc')


%Broadcast%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

load broadcast

h=figure;
plot([1 10 20 30 40 50 60 70], [mean(thread_1) mean(thread_10) mean(thread_20) mean(thread_30) mean(thread_40) ...
    mean(thread_50) mean(thread_60) mean(thread_70)])
xlabel('Number of thread');
ylabel('Mean time [us]');
axis ([1 70 0 inf])
saveas(h, 'figures/mean_broadcast', 'epsc')

load vip_prio

prio=zeros(10,1);
normal=zeros(10,1);
for i=1:10
    prio(i)=mean(vip(i+1,1:i));
    normal(i)=mean(vip(i,i:end));
end

h=figure;
plot(1:10,prio,'b')
hold on
plot(0:9,normal,'r')
xlabel('Number of vip persons');
ylabel('Mean time [us]');
legend('VIP', 'normal', 'location' , 'northwest')
saveas(h, 'figures/vip_prio', 'epsc')

load vip_alg

prio=zeros(10,1);
normal=zeros(10,1);
for i=1:10
    prio(i)=mean(vip(i+1,1:i));
    normal(i)=mean(vip(i,i:end));
end

h=figure;
plot(1:10,prio,'b')
hold on
plot(0:9,normal,'r')
xlabel('Number of vip persons');
ylabel('Mean time [us]');
legend('VIP', 'normal', 'location' , 'northwest')
saveas(h, 'figures/vip_alg', 'epsc')

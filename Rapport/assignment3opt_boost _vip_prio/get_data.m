vip = zeros(10,10);

%%
load output.txt
vip(6,:)=output;
%%

close all
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
%axis ([0 11 0 inf])
legend('VIP', 'normal', 'location' , 'northwest')
saveas(h, 'figures/number_of_persons_3_opt', 'epsc')


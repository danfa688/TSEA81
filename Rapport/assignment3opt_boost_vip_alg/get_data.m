vip = zeros(11,10);

%%
load output.txt
vip(11,:)=output;
%%

close all
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


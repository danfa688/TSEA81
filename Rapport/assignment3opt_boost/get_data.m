load output.txt
persons_70=output;
%%

close all
load person_dat_3_opt

h=figure;
plot([10 20 30 40 50 60 70], [mean(persons_10) mean(persons_20) mean(persons_30) mean(persons_40) ...
    mean(persons_50) mean(persons_60) mean(persons_70)])
xlabel('Number of persons');
ylabel('Mean time [us]');
axis ([10 70 0 inf])
saveas(h, 'figures/number_of_persons_3_opt', 'epsc')


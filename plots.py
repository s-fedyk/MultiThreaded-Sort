import matplotlib.pyplot as plt
import os
import numpy as np
import re
processors_2 = [[512000000, 58171676/1000000], [1024000000, 118790569/1000000], [2048000000, 244982002/1000000], [4096000000, 503177333/1000000]]
processors_4 = [[512000000, 29017068/1000000], [1024000000, 59889006/1000000], [2048000000, 122724973/1000000], [4096000000, 253117140/1000000]]
processors_8 = [[512000000, 14871957/1000000], [1024000000, 31016618/1000000], [2048000000, 63405301/1000000], [4096000000, 130879347/1000000]]
processors_16 = [[512000000, 7633020/1000000], [1024000000, 15884688/1000000], [2048000000, 31889605/1000000], [4096000000, 69306725/1000000]]
processors_32 = [[512000000, 4093391/1000000], [1024000000, 8729126/1000000], [2048000000, 17059356/1000000], [4096000000, 35045324/1000000]]
processors_64 = [[512000000, 2568863/1000000], [1024000000, 5408739/1000000], [2048000000, 11020239/1000000], [4096000000, 23357375/1000000]]
processors_128 = [[512000000, 2313368/1000000], [1024000000, 4863716/1000000], [2048000000, 9870920/1000000], [4096000000, 20131978/1000000]]

def plot_seconds():
    plt.clf()

    x1, y1 = zip(*processors_2)
    x2, y2 = zip(*processors_4)
    x3, y3 = zip(*processors_8)
    x4, y4 = zip(*processors_16)
    x5, y5 = zip(*processors_32)
    x6, y6 = zip(*processors_64)
    x7, y7 = zip(*processors_128)

    plt.plot(x1, y1, alpha=0.5, label="2")
    plt.plot(x2, y2, alpha=0.5, label="4")
    plt.plot(x3, y3, alpha=0.5, label="8")
    plt.plot(x4, y4, alpha=0.5, label="16")
    plt.plot(x5, y5, alpha=0.5, label="32")
    plt.plot(x6, y6, alpha=0.5, label="64")
    plt.plot(x7, y7, alpha=0.5, label="128")

    plt.scatter(x1, y1)

    plt.xlabel("Input size")
    plt.ylabel("Runtime (in seconds)")

    plt.legend()

    plt.savefig("figures/runtimes.png")


directory_path = "./results/"
file_names = "k"
pattern = re.compile(r'psrs: (\d+) phase1 (\d+) phase2 (\d+) phase3 (\d+) phase4 (\d+)')
def sum_times_in_phase(p): 
    file_pattern = re.compile(r'(\d+)-processors-(\d+)')

    total= 0;
    p1_sum = 0;
    p2_sum = 0;
    p3_sum = 0;
    p4_sum = 0;
    for file_name in os.listdir(directory_path):
        match = file_pattern.match(file_name)
        if match:
            num_processors = int(match.group(1))
            n = int(match.group(2))
            if (num_processors == p):
                # Your code to process the file goes here
                file_path = os.path.join(directory_path, file_name)
                with open(file_path, 'r') as file_handle:
                    text = file_handle.read();
                    match = pattern.match(text)
                    psrs_value = int(match.group(1))

                    p1_sum += int(match.group(2))
                    p2_sum += int(match.group(3))
                    p3_sum += int(match.group(4))
                    p4_sum += int(match.group(5))
                    total+= p1_sum+p2_sum+p3_sum+p4_sum

    return [p1_sum/total, p2_sum/total, p3_sum/total, p4_sum/total]


if __name__ == "__main__":

    processors = ['2', '4', '8', '16', '32', '64','128']
    i = 2

    phase1 = []
    phase2 = []
    phase3 = []
    phase4 = []



    while (i <= 128):
        times = sum_times_in_phase(i)

        phase1.append(times[0])
        phase2.append(times[1])
        phase3.append(times[2])
        phase4.append(times[3])
        
        i *= 2

    plt.bar(processors, phase1, label='Phase1')
    plt.bar(processors, phase2, bottom=phase1, label='Phase2')
    plt.bar(processors, phase3, bottom=phase2, label='Phase3')
    plt.bar(processors, phase4, bottom=phase3, label='Phase4')

    plt.ylabel('Time Spent')
    plt.title('Time Spent in Different Phases')
    plt.legend();
    plt.savefig("figures/phases.png")

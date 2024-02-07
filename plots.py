import matplotlib.pyplot as plt
import os
import numpy as np
import re
sequential = [[512000000, 107629259], [1024000000, 224393217], [2048000000, 462979491], [4096000000, 954416148]]
processors_2 = [[512000000, 58171676], [1024000000, 118790569], [2048000000, 244982002], [4096000000, 503177333]]
processors_4 = [[512000000, 29017068], [1024000000, 59889006], [2048000000, 122724973], [4096000000, 253117140]]
processors_8 = [[512000000, 14871957], [1024000000, 31016618], [2048000000, 63405301], [4096000000, 130879347]]
processors_16 = [[512000000, 7633020], [1024000000, 15884688], [2048000000, 31889605], [4096000000, 69306725]]
processors_32 = [[512000000, 4093391], [1024000000, 8729126], [2048000000, 17059356], [4096000000, 35045324]]
processors_64 = [[512000000, 2568863], [1024000000, 5408739], [2048000000, 11020239], [4096000000, 23357375]]
processors_128 = [[512000000, 2313368], [1024000000, 4863716], [2048000000, 9870920], [4096000000, 20131978]]

times = [processors_2, processors_4, processors_8, processors_16, processors_32, processors_64, processors_128]

def plot_speedup(times, sizeIndex):
    plt.close()

    plt.xlabel("Number of processors")
    plt.ylabel("Speedup")

    for index in range(0,sizeIndex):
        [_,seq_seconds] = sequential[index]
        i = 2

        y = []
        x = []
        for time in times:
            [size, seconds] = time[index]
            y.append(seq_seconds/seconds)
            x.append(i)
            i*=2

        plt.plot(x,y, alpha=0.5, label="{:,}".format(sequential[index][0]))

    x = []
    y = []
    i = 2
    while (i <=128):
        x.append(i)
        y.append(i)
        i*=2
    plt.plot(x,y, alpha=0.5, label="Theoretical Speedup")
    plt.legend()
    plt.savefig("figures/speedup.png")
    plt.close()





def plot_seconds():
    plt.clf()
    plt.title('Runtimes grouped by processors and number of keys')

    x1, y1 = zip(*processors_2)
    x2, y2 = zip(*processors_4)
    x3, y3 = zip(*processors_8)
    x4, y4 = zip(*processors_16)
    x5, y5 = zip(*processors_32)
    x6, y6 = zip(*processors_64)
    x7, y7 = zip(*processors_128)
    x8, y8 = zip(*sequential)

    plt.plot(x8, y8, alpha=0.5, label="1")
    plt.plot(x1, y1, alpha=0.5, label="2")
    plt.plot(x2, y2, alpha=0.5, label="4")
    plt.plot(x3, y3, alpha=0.5, label="8")
    plt.plot(x4, y4, alpha=0.5, label="16")
    plt.plot(x5, y5, alpha=0.5, label="32")
    plt.plot(x6, y6, alpha=0.5, label="64")
    plt.plot(x7, y7, alpha=0.5, label="128")
    
    plt.scatter(x1, y1)

    plt.xlabel("Number of keys")
    plt.ylabel("Runtime (in µs)")

    plt.legend()

    plt.savefig("figures/runtimes.png")
    plt.close()

def create_table(data, columns, row_headers):
    fig, ax = plt.subplots()
    ax.axis('off')  # Turn off axis labels and ticks

    table_data = []

    # Add header row
    table_data.append(columns)
    def extract(x):        
        return "{:,}".format(x[1]) + "µs"

    header_row = [''] + columns if row_headers else columns
    # Add data rows
    i = 0
    for row in data:
        times = list(map(extract, row))

        if row_headers:
            table_data.append([row_headers[i]] + times)
        else:
            table_data.append(times)
        i += 1

    table = ax.table(cellText=table_data, loc='center', cellLoc='center', colLabels=None)
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.2, 1.2)  # Adjust the table size as needed
    plt.savefig("figures/timesTable.png")
    plt.close()

def times_in_phases(p):
    file_pattern = re.compile(r'(\d+)-processors-(\d+)')

    items = []

    total_sum= 0;
    p1_sum = 0;
    p2_sum = 0;
    p3_sum = 0;
    p4_sum = 0;
    files_matched = 0
    for file_name in os.listdir(directory_path):
        match = file_pattern.match(file_name)
        if match:
            num_processors = int(match.group(1))
            n = int(match.group(2))
            if (num_processors == p):
                files_matched+=1
                # Your code to process the file goes here
                file_path = os.path.join(directory_path, file_name)
                with open(file_path, 'r') as file_handle:
                    text = file_handle.read();
                    match = pattern.match(text)
                    psrs_value = int(match.group(1))
                    p1 = int(match.group(2))
                    p2 = int(match.group(3))
                    p3 = int(match.group(4))
                    p4 = int(match.group(5))

                    item = [p,n,p1,p2,p3,p4]
                    items.append(item)
    
    return items
    

def phase_table(data, columns, row_headers):
    fig, ax = plt.subplots()
    ax.axis('off')  # Turn off axis labels and ticks

    table_data = []

    # Add header row
    table_data.append(columns)
    def extract(x):        
        return "{:,}".format(x) + "µs"

    header_row = [''] + columns if row_headers else columns
    # Add data rows
    i = 0
    for row in data:
        times = list(map(extract, row[2:]))
        [p,n] = row[:2]

        table_data.append([p,n] +times)

        i += 1

    table = ax.table(cellText=table_data, loc='center', cellLoc='center', colLabels=None)
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.25, 1)  # Adjust the table size as needed
    plt.savefig("figures/phaseTable.png")
    plt.close()


directory_path = "./results/"
file_names = "k"
pattern = re.compile(r'psrs: (\d+) phase1 (\d+) phase2 (\d+) phase3 (\d+) phase4 (\d+)')
def sum_times_in_phase(p): 
    file_pattern = re.compile(r'(\d+)-processors-(\d+)')

    total_sum= 0;
    p1_sum = 0;
    p2_sum = 0;
    p3_sum = 0;
    p4_sum = 0;
    files_matched = 0
    for file_name in os.listdir(directory_path):
        match = file_pattern.match(file_name)
        if match:
            num_processors = int(match.group(1))
            n = int(match.group(2))
            if (num_processors == p):
                files_matched+=1
                # Your code to process the file goes here
                file_path = os.path.join(directory_path, file_name)
                with open(file_path, 'r') as file_handle:
                    text = file_handle.read();
                    match = pattern.match(text)
                    psrs_value = int(match.group(1))
                    p1 = int(match.group(2))
                    p2 = int(match.group(3))
                    p3 = int(match.group(4))
                    p4 = int(match.group(5))
                    total = p1+p2+p3+p4
                    p1_sum += p1/total
                    p2_sum += p2/total
                    p3_sum += p3/total
                    p4_sum += p4/total

    # different inputs, same processor amount. avg
    return (p1_sum, p2_sum, p3_sum, p4_sum)


def plot_phases():
    processors = ['2', '4', '8', '16', '32', '64','128']
    i = 2

    phase1 = []
    phase2 = []
    phase3 = []
    phase4 = []

    while (i <= 128):
        print("summing phases")
        (p1,p2,p3,p4) = sum_times_in_phase(i)
        total = p1+p2+p3+p4
        phase1.append(np.array(p1))
        phase2.append(np.array(p2))
        phase3.append(np.array(p3))
        phase4.append(np.array(p4))
        i *= 2

    plt.bar(processors, phase1, label='Phase1')
    plt.bar(processors, phase2, bottom=phase1, label='Phase2')
    plt.bar(processors, phase3, bottom=[sum(x) for x in zip(phase1,phase2)], label='Phase3')
    plt.bar(processors, phase4, bottom=[sum(x) for x in zip(phase1,phase2,phase3)], label='Phase4')

    plt.ylabel('% Time')
    plt.xlabel('Number of processors')
    plt.title('% Time spent in phase grouped by number of processors')
    plt.legend();
    plt.savefig("figures/phases.png")
    plt.close()


if __name__ == "__main__":
    plot_seconds()
    plot_speedup(times, 4)
    plot_phases()
    
    create_table([sequential] +times,["", "512,000,000", "1,024,000,000", "2,048,000,000", "4,096,000,000"], 
                 ["1","2", "4", "8", "16", "32", "64", '128'])

    processor_phase_times = []
    i = 2
    while (i <= 128):
        processor_phase_times += times_in_phases(i)
        i *= 2
    print(processor_phase_times)
    phase_table(processor_phase_times, ['Processors','# Keys','Phase1', 'Phase2', 'Phase3', 'Phase4'],["1","2", "4", "8", "16", "32", "64", '128'])


    

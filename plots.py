import matplotlib.pyplot as plt

processors_2 = [[512000000, 78868583/1000000], [1024000000, 161365390/1000000], [2048000000, 333016008/1000000], [4096000000, 676090841/1000000]]
processors_4 = [[512000000, 40532050/1000000], [1024000000, 84435337/1000000], [2048000000, 172617776/1000000], [4096000000, 362741085/1000000]]
processors_8 = [[512000000, 22300856/1000000], [1024000000, 45372394/1000000], [2048000000, 94049237/1000000], [4096000000, 235985783/1000000]]
processors_16 = [[512000000, 14096661/1000000], [1024000000, 27514360/1000000], [2048000000, 58240011/1000000], [4096000000, 216964525/1000000]]
processors_32 = [[512000000, 10223605/1000000], [1024000000, 18902816/1000000], [2048000000, 39302631/1000000], [4096000000, 330188861/1000000]]
processors_64 = [[512000000, 10410936/1000000], [1024000000, 18834719/1000000], [2048000000, 36212383/1000000], [4096000000, 565048110/1000000]]
processors_128 = [[512000000, 14026955/1000000], [1024000000, 25408626/1000000], [2048000000, 46221814/1000000], [4096000000, 1055728096/1000000]]

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

def plot_seconds_excluded():
    plt.clf()

    x1, y1 = zip(*processors_2[:3])
    x2, y2 = zip(*processors_4[:3])
    x3, y3 = zip(*processors_8[:3])
    x4, y4 = zip(*processors_16[:3])
    x5, y5 = zip(*processors_32[:3])
    x6, y6 = zip(*processors_64[:3])
    x7, y7 = zip(*processors_128[:3])

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

    plt.savefig("figures/runtimes_excluded.png")


if __name__ == "__main__":
    plot_seconds()
    plot_seconds_excluded()



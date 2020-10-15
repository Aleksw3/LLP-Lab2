import csv
import matplotlib.pyplot as plt
import numpy as np

def read_file(name):
	time,current,voltage = [],[],[]

	with open(name, 'r') as f:
		rd = csv.reader(f)
		for line in rd:
			if "Time" in line[0]:
				continue
			data = line[0].split(";")
			time.append((int(data[0])))
			current.append(float(data[1]))
			voltage.append(float(data[2]))

	data = {"time":(np.array(time)-time[0])*1e-6, "current":np.array(current),"voltage":np.array(voltage)}
	return data


sleep = read_file("sleep")
polling = read_file("polling")
deep_sleep = read_file("deep_sleep")

print(f"avg idle sleep current {np.mean(sleep['current'][-120:-1])} mA")
print(f"avg idle deep_sleep current {np.mean(deep_sleep['current'][-120:-1])*1000} uA")
print(f"avg idle polling current {np.mean(polling['current'][-50:-1])} mA")



# ## Idle current
# a = -120
# b = -120
# c = -50
# plt.plot(polling["time"][c:-1]-polling["time"][c],polling["current"][c:-1])
# plt.plot(sleep["time"][a:-1]-sleep["time"][a],sleep["current"][a:-1])
# plt.plot(deep_sleep["time"][b:-1]-deep_sleep["time"][b],deep_sleep["current"][b:-1])

# plt.ylabel("Current [mA]")
# plt.xlabel("Time [s]")
# plt.legend(["Deep Sleep", "Sleep","Polling"][::-1])
# plt.grid()
# plt.savefig("Current_comparison_idle.png")
# plt.show()



# ## EM2 current
# b = -120
# plt.plot(deep_sleep["time"][b:-1]-deep_sleep["time"][b],deep_sleep["current"][b:-1]*1000)

# plt.ylabel(r'Current [$\mu$A]')
# plt.xlabel("Time [s]")

# plt.legend(["Deep Sleep"])
# plt.grid()
# plt.savefig("Current_em2.png")
# plt.show()

# ## Idle current
# a = 50
# b = 50
# c = 50
# plt.plot(polling["time"][0:c]-polling["time"][0],polling["current"][0:c])
# plt.plot(sleep["time"][0:a]-sleep["time"][0],sleep["current"][0:a])
# plt.plot(deep_sleep["time"][0:b]-deep_sleep["time"][0],deep_sleep["current"][0:b])

# plt.ylabel("Current [mA]")
# plt.xlabel("Time [s]")
# plt.legend(["Deep Sleep", "Sleep","Polling"][::-1])
# plt.grid()
# plt.savefig("Current_comparison.png")
# plt.show()
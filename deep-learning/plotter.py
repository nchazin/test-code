import sys
import pandas
import matplotlib.pyplot as plt

dataset = pandas.read_csv(sys.argv[1], usecols=[1], engine='python')
plt.plot(dataset)
plt.title(sys.argv[1])
plt.show()

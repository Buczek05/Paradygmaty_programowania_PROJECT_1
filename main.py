from plotly import graph_objects as go
FILE_NAME = "results.csv"


class Main:
    data = None

    def __init__(self):
        self.setup_data()
        self.plot()

    def setup_data(self):
        self.data = Data()
        self.data.setup()

    def plot(self):
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=self.data.x, y=self.data.fun, name="function"))
        fig.add_trace(go.Scatter(x=self.data.x, y=self.data.series, name="series"))
        fig.show()


class Data:
    x = []
    fun = []
    series = []
    iterate_numbers = []

    def __init__(self):
        self.filename = FILE_NAME

    def setup(self):
        with open(self.filename, "r") as file:
            lines = file.readlines()
            for line in lines[1:]:
                line = line.split(";")
                self.x.append(float(line[0]))
                self.fun.append(float(line[1]))
                self.series.append(float(line[2]))
                self.iterate_numbers.append(float(line[3]))


if __name__ == "__main__":
    Main()

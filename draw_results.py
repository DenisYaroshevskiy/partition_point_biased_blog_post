import argparse
import json
import plotly

def generateMapOfStyles():
    styles = {}
    styles['benchmark_search<linear>'] = dict(
        mode = 'lines',
        name = 'linear_search',
        line = dict(width = 3, dash = 'dash', color = 'rgb(100, 100, 0)')
    )
    styles['benchmark_search<binary>'] = dict(
        mode = 'lines',
        name = 'binary_search',
        line = dict(width = 3, dash = 'dot', color = 'rgb(100, 000, 100)')
    )

    return styles

class parsedBenchmark:
    def __init__(self, name, xs, times):
        self.name = name
        self.xs = xs
        self.times = times

class runner:
    def __init__(self):
        self.jsonFiles = []
        self.benchmarks = []
        self.data = None
        self.layout = None

    def parseFromOptions(self):
        parser = argparse.ArgumentParser(\
        description="Draws google benchmark's json output as a graph")
        parser.add_argument('results', metavar='results', nargs='+', \
             help='selected benchmarks to display on one chart')
        options = parser.parse_args()
        self.jsonFiles = options.results

    def loadJsons(self):
        for jsonFile in self.jsonFiles:
            loaded = json.load(open(jsonFile))
            name = loaded['benchmarks'][0]['name'].split('/')[0]

            xs = []
            times = []
            for measurement in loaded['benchmarks']:
                xs.append(int(measurement['name'].split('/')[1]))
                times.append(float(measurement["real_time"]))

            self.benchmarks.append(parsedBenchmark(name, xs, times))

    def generateData(self):
        styles = generateMapOfStyles()
        traces = []
        for benchmark in self.benchmarks:
            traces.append(plotly.graph_objs.Scatter(
                x = benchmark.xs,
                y = benchmark.times,
                **styles[benchmark.name]))
        self.data = traces

    def generateLayout(self):
        layout = {}
        xs = self.benchmarks[0].xs

        for benchmark in self.benchmarks:
            assert(xs == benchmark.xs)

        layout['xaxis'] = dict(
            title = 'distance(f, result)',
            tick0 = 0,
            dtick = (xs[-1] - xs[0]) / 25
        )

        layout['yaxis'] = dict(title = 'ns')
        self.layout = layout

    def draw(self):
        plotly.offline.plot(dict(data = self.data, layout = self.layout), auto_open = True)

if __name__ == "__main__":
    r = runner()
    r.parseFromOptions()
    r.loadJsons()
    r.generateData()
    r.generateLayout()
    r.draw()

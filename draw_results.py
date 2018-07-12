import argparse
import json
import plotly

class runner:
    def __init__(self):
        self.benchmarks = []

    def parseFromOptions(self):
        parser = \
          argparse.ArgumentParser(description="Draws google benchmark's json output as a graph")
        parser.add_argument('printed jsons: ', metavar='jsons', nargs='+',
                            help='selected benchmarks to display on one chart')
        parser.parse_args()


if __name__ == "__main__":
    r = runner()
    r.parseFromOptions()

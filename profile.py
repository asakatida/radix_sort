from math import log10
from pprint import pprint
from random import shuffle
from sys import argv
from timeit import timeit

from radix_sort import radix_sort


def time(
        i,
        s='assert radix_sort(lst) == sorted_lst',
        number=5,
        globals={
            'lst': None,
            'radix_sort': radix_sort,
            'sorted_lst': None,
        }):
    globals.update({
        'lst': range(i, 0, -1),
        'sorted_lst': sorted(range(i, 0, -1))})
    return timeit(s, number=number, globals=globals)


def main():
    top = int(argv[1], base=int(argv[2]))
    profile = {i: time(i) for i in range(top // 2, top)}
    ratio = {k: (v, v / ((k + 1) * log10(k + 1))) for k, v in profile.items()}
    profile = [v[1] for _, v in ratio.items()]
    m = max(profile), min(profile)
    m = {'max': (profile.index(m[0]), m[0]), 'min': (profile.index(m[1]), m[1])}

    with open('profile.log', 'w') as ostream:
        pprint(ratio, ostream)
        pprint(m, ostream)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()

import argparse
import math

import numpy as np
import functools
import operator

parser = argparse.ArgumentParser(description='Generate testcase for istrstr')
parser.add_argument('size', type=int)
parser.add_argument('--seed', type=int, default=0)
parser.add_argument('--types', nargs='+', type=str, default=['en', 'num', 'spaces'], choices=['en', 'ru', 'num', 'spaces'])
parser.add_argument('--cases', action='store_true')
parser.add_argument('--chunk_size', type=int, default=10000)

args = parser.parse_args()

rs = np.random.RandomState(args.seed)

alphabets = {
    'en': 'qwertyuiopasdfghjklzxcvbnm',
    'ru': 'йцукенгшщзфывапролдячсмитьё',
    'num': '1234567890',
    'spaces': ' '
}

space = functools.reduce(operator.add, (alphabets[x] for x in args.types))
if args.cases:
    space += space.upper()

for chunk_ix in range(math.ceil(args.size / args.chunk_size)):
    size = min(args.chunk_size, args.size - args.chunk_size * chunk_ix)
    result = rs.choice(list(space), size)
    print(''.join(result), end='')

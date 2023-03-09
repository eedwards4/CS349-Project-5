
import random as rand
import string
import argparse

parser = argparse.ArgumentParser(
    prog='gen_input.py',
    description='generate input',
)


parser.add_argument('coords',
                    action='extend',
                    nargs=2,
                    choices=range(3, 1001),
                    help='--REQUIRED--\nMust provide x and y boundaries of the matrix {RANGE(3,1000)}.',
                    metavar='XY COORDS',
                    type=int)
parser.add_argument('-c', '--cases',
                    choices=range(1, 100),
                    metavar='RANGE(1, 100)',
                    help='Number of test cases.',
                    default='1',
                    type=int)
parser.add_argument('-n', '--num-classes',
                    choices=range(1, 26),
                    metavar='RANGE(1, 25)',
                    help='Number of ship classes. Default is 2.',
                    default='2',
                    type=int)
parser.add_argument('-lv', '--low-value',
                    choices=range(0, 1000),
                    metavar='RANGE(0, 999)',
                    help='Lowest value for a class. Default is 0.',
                    default=0,
                    type=int)
parser.add_argument('-mv', '--max-value',
                    choices=range(1, 1001),
                    metavar='RANGE(1, 1000)',
                    help='Max value for a class. Default is 1000.',
                    default=1000,
                    type=int)

args = parser.parse_args()

file = open('cmake-build-debug\\gen_input.txt', 'w')
cases = args.cases
num_classes = args.num_classes
x_bound = args.coords[0]
y_bound = args.coords[1]

valid_alphas = string.ascii_uppercase.replace('E', '')
char_set = []
for number in range(0, num_classes):
    char_set.insert(len(char_set) - 1, rand.choice(valid_alphas))
    valid_alphas.replace(char_set[len(char_set) - 1], '')

file.write('{}\n'.format(cases))
for i in range(0, cases):
    enterprise = [rand.randint(1, y_bound - 2), rand.randint(1, x_bound - 2)]
    file.write('{0} {1} {2}\n'.format(num_classes, x_bound, y_bound))
    for j in range(0, num_classes):
        file.write('{0} {1}\n'.format(char_set[j], rand.randint(args.low_value, args.max_value)))
    for y_pos in range(0, y_bound):
        for x_pos in range(0, x_bound):
            if y_pos == enterprise[0] and x_pos == enterprise[1]:
                file.write('E')
            else:
                file.write('{}'.format(rand.choice(char_set)))
        file.write('\n')
    file.write('\n')

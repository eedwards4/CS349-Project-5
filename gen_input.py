#
#
#   The first arg of this program determines how many test cases are given, and the second arg determines the
#   limit on how many digits the numbers added together can have.
#
#
import random as rand
import string
import argparse

parser = argparse.ArgumentParser(
    prog='gen_input.py',
    description='generate input'
)

parser.add_argument('-c', '--cases',
                    choices=range(1, 100),
                    metavar='RANGE(1, 100)',
                    default='1',
                    type=int)
parser.add_argument('-n', '--num-classes',
                    choices=range(2, 26),
                    metavar='RANGE(2, 25)',
                    default='2',
                    type=int)
parser.add_argument('-xy', '--coord',
                    nargs=2,
                    choices=range(1, 1001),
                    help='--REQUIRED-- Must provide x and y boundaries of the matrix {RANGE(1,1000)}.',
                    metavar='BOUNDARY_VAL',
                    required=True,
                    type=int)
parser.add_argument('-lv', '--low-value',
                    choices=range(0, 1000),
                    metavar='RANGE(0, 999)',
                    help='Lowest value for a class.',
                    default=0,
                    type=int)
parser.add_argument('-mv', '--max-value',
                    choices=range(1, 1001),
                    metavar='RANGE(1, 1000)',
                    help='Max value for a class.',
                    default=1000,
                    type=int)

args = parser.parse_args()

file = open('cmake-build-debug\\gen_input.txt', 'w')
cases = args.cases
num_classes = args.num_classes
x_bound = args.coord[0]
y_bound = args.coord[1]

valid_alphas = string.ascii_uppercase.replace('E', '')
char_set = []
for number in range(0, num_classes):
    char_set.insert(len(char_set) - 1, rand.choice(valid_alphas))
    valid_alphas.replace(char_set[len(char_set) - 1], '')
enterprise = [rand.randint(0, y_bound), rand.randint(0, x_bound)]

file.write('{}\n'.format(cases))
for i in range(0, cases):
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

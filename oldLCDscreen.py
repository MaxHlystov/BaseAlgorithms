##Напишите программу, которая выводит число в стиле LCD калькулятора.
##
##На вход программе подаётся последовательность цифр, которую нужно
##вывести на экран в специальном стиле (см. пример).
##
##Размер всех цифр 4 символа в ширину и 7 символов в высоту. Между
##цифрами в выводе должен быть один пустой столбец. Перед первой
##цифрой не должно быть пробелов.
##
##Выведенные цифры должны быть обведены рамочкой, в углах которой
##находится символ x ("икс"), горизонтальная линия создаётся
##из символа - ("дефис"), а вертикальная -- из символа вертикальной
##черты: |.
##
##Формат ввода:
##Строка произвольной длины (минимум один символ), содержащая
##последовательность цифр.
##
##Формат вывода:
##9 строк, содержащих цифры, записанные в указанном в задании формате.
##
##Sample Input:
##0123456789
##
##Sample Output:
##x-------------------------------------------------x
##| --        --   --        --   --   --   --   -- |
##||  |    |    |    | |  | |    |       | |  | |  ||
##||  |    |    |    | |  | |    |       | |  | |  ||
##|           --   --   --   --   --        --   -- |
##||  |    | |       |    |    | |  |    | |  |    ||
##||  |    | |       |    |    | |  |    | |  |    ||
##| --        --   --        --   --        --   -- |
##x-------------------------------------------------x


from itertools import chain, tee
from functools import reduce
from operator import add


def initDigits():
    ''' () => {LCD parameters}
        Return parameters of LCD display.
        It is used for other functions,
        as a LCDparams argument. '''
    numbers = [' --      --  --      --  --  --  --  -- ',
               '|  |   |   |   ||  ||   |      ||  ||  |',
               '|  |   |   |   ||  ||   |      ||  ||  |',
               '         --  --  --  --  --      --  -- ',
               '|  |   ||      |   |   ||  |   ||  |   |',
               '|  |   ||      |   |   ||  |   ||  |   |',
               ' --      --  --      --  --      --  -- ']
    return {'height': 7, # height of a sign on the display
            'numbers': numbers, # String representations of signs
            'width': 4} # With of numbers


def charToSign(LCDparams, c):
    ''' (char) => [str]
        Gets an list of characters and return list
        of signs. Each sign is a list of strings
        represent of character on LCD display.
    '''
    if c == ' ':
        return [' '] * LCDparams['height']
    try:
        d = int(c)
    except ValueError:
        return [''] * LCDparams['height']
    if d < 0 or d > 9:
        return [''] * LCDparams['height']
    width = LCDparams['width']
    start = width * d
    end = start + width
    return [line[start:end] for line in LCDparams['numbers']]


def sparce(chars):
    ''' ([char]) => [char])
        Take a list of chars and insert space between them.
        ['1','2','3'] => ['1', ' ', '2', ' ', '3']
    '''
    concat = lambda res, val: res + [' '] + [val] if res else [val]
    return reduce(concat, chars, [])


def signWidth(sign):
    if sign:
        return len(sign[0])
    return 0


def addBorder(LCDparams, signs):
    ''' ([[str]]) => str
        Takes a list of signs and return list of signs
        with border. For each sign it adds string of
        border up and down. And for left and right
        borders it adds first and last signs of border.
    '''
    Iters = tee(signs)
    widths = map(signWidth, Iters[0])
    closedSigns = map(lambda s, w: ['-'*w] + s + ['-'*w], Iters[1], widths)
    verticalBorder = [['x'] + ['|'] * LCDparams['height'] + ['x']]
    return chain(verticalBorder, closedSigns, verticalBorder)


def signsAsString(LCDparams, signs):
    ''' ([[str]]) => str
        Takes a list of digit representations and
        make a string to show on the display.
    '''
    lines = reduce(lambda res, val: map(add, res, val), signs)
    return "\n".join(lines)

                 
def getLCDstring(LCDparams, s):
    ''' (str) => str
        Takes string with digits and return string
        representation of LCD screen.'''
    chars = sparce(s)
    signs = map(lambda c: charToSign(LCDparams, c), chars)
    signs = addBorder(LCDparams, signs)
    return signsAsString(LCDparams, signs)


LCDparams = initDigits()
s = "0123456789" #input()
print(getLCDstring(LCDparams, s))

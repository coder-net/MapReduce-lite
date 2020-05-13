from collections import Counter


def main():
    with open('../data/words_count_text.txt') as f:
        text = f.read()
    words = text.split(' ')
    c = Counter(words)
    l = [[key, str(c[key])] for key in c]
    l.sort(key=lambda x: x[0])
    with open('../data/words_count_py_result.txt', 'w') as out:
        for pair in l:
            out.write("\t".join(pair) + '\n')

main()
import pytest
import os
from _w2_cffi import ffi, lib

test_dir = os.path.abspath(os.path.join(__file__, os.path.pardir, os.path.pardir, "tests"))

expected = (
    (3, 6, 8, 9),
    (3, 6, 8, 11),
    (3, 6, 8, 12),
    (3, 6, 8, 13),
    (3, 6, 8, 16),
    (3, 6, 8, 17),
    (3, 6, 8, 18),
    (3, 6, 8, 20),
    (3, 6, 8, 21),
    (3, 6, 8, 29),
    (3, 6, 8, 31),
    (13, 14, 16, 17),
    (13, 14, 16, 18),
    (13, 14, 16, 20),
    (13, 14, 16, 21),
    (13, 14, 16, 29),
    (13, 14, 16, 31),
    (13, 14, 18, 20),
    (13, 14, 18, 21),
    (13, 14, 18, 29),
    (13, 14, 18, 31),
    (24, 25, 26, 28),
    (21, 27, 29, 31),
    (3, 6, 8, 9, 12),
    (3, 6, 8, 9, 13),
    (3, 6, 8, 9, 16),
    (3, 6, 8, 9, 18),
    (3, 6, 8, 9, 21),
    (3, 6, 8, 9, 29),
    (3, 6, 8, 9, 31),
    (3, 6, 8, 11, 16),
    (3, 6, 8, 11, 18),
    (3, 6, 8, 11, 21),
    (3, 6, 8, 11, 29),
    (3, 6, 8, 11, 31),
    (13, 14, 16, 17, 18),
    (13, 14, 16, 17, 21),
    (13, 14, 16, 17, 29),
    (13, 14, 16, 17, 31),
    (13, 14, 18, 20, 29),
    (13, 14, 18, 20, 31),
    (3, 6, 8, 12),
    (3, 6, 8, 13),
    (3, 6, 8, 21),
    (3, 6, 8, 31),
    (3, 6, 8, 11, 16, 21),
    (3, 6, 8, 11, 16, 31),
    (13, 14, 16, 17, 18, 21),
    (13, 14, 16, 17, 18, 31),
    (13, 14, 18, 20, 29, 31)
)

def test_lemstrom():
    with open(os.path.join(test_dir, "query_a.csv"), "r") as f:
        pattern = lib.LoadScoreFromFile(f)

    with open(os.path.join(test_dir, "leiermann.csv"), "r") as f:
        target = lib.LoadScoreFromFile(f)

    results = lib.search(pattern, target)

    ptr = results.head
    for i in range(results.length):
        for j in range(len(expected[i])):
            if ptr.targetNotes[j] != expected[i][j]:
                pytest.fail(f"got {ptr.targetNotes}, expected {expected[i]}")
        ptr = ptr.next

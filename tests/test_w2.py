import io
import pytest
#from _w2 import ffi, lib
from indexers import legacy_intra_vectors

def test_w2():
    df_pattern = intra_vectors("tests/leiermann.xml")
    df_target = intra_vectors("tests/query_a.mid")

    pattern = df_pattern.to_csv()
    target = df_target.to_csv()

    print(pysearch(pattern, target))


pattern = legacy_intra_vectors("tests/query_a.mid", 1)
target = legacy_intra_vectors("tests/leiermann.xml", 10)
#lib.search(pattern.encode('utf-8'), target.encode('utf-8'))

#pattern = df_pattern.to_csv()
#target = df_target.to_csv()
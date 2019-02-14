import io
import w2

def pysearch(df_pattern, df_target):
    pattern_buf = io.StringIO(df_pattern.to_csv())
    target_buf = io.StringIO(df_target.to_csv())
    output_buf = io.StringIO()

    w2.search.search(pattern_buf, target_buf, output_buf)

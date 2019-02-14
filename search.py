import io
import w2

def search(df_pattern, df_target):
    pattern_buf = io.StringIO()
    target_buf = io.StringIO()
    output_buf = io.StringIO()

    with open(output_buf, 'w') as f:
        f.write(w2.search(pattern_buf, target_buf, output_buf)

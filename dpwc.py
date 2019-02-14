import sys
import os
import io
import math
import json
import logging
import tqdm
import music21
from enum import Enum

from subprocess import Popen, PIPE, call, check_output
from multiprocessing import Pool

flask_logger = logging.getLogger("flask.app")

#w_path = "/app/patternfinder/patternfinder/geometric_helsinki/_w"
w_path = os.path.join(os.path.abspath(os.path.dirname(__file__)), os.path.pardir, "_w")

class Threshold(Enum):
    ALL = 0
    SOME = 1

class Transposition(Enum):
    ALL = 0
    NONE = 1
    OCTAVE = 2

def w_wrapper(pattern, target):
    #ps = Popen([w_path, '--stream', target], stdin=PIPE, stdout=PIPE, stderr=PIPE, universal_newlines=True)
    #output, _ = ps.communicate(input=pattern)

    #todo capture stdout
    args = "echo '{}' | {} --stream {}".format(pattern, w_path, target)
    output = str(check_output(args, shell=True), encoding='utf-8')

    #print("output " + output)


    return output

def search(pattern_str, mass_path):
    mass_vector_path = mass_path
    #print("Processing " + mass_path)
    result = w_wrapper(
        pattern=pattern_str,
        target='"{}"'.format(mass_vector_path))
    result = json.loads(result)
    #print("RESULT\n" + str(result))
    if result:
        # Result is a JSON list of objects
        for occ in result:
            # mass is a path which ends in the XML file of that mass
            occ['piece'] = mass_path.split('/')[-1].split('.')[0]
    return result

def search_scores(pattern_str, palestrina_path):

    flask_logger.info("Searching in path: {}".format(palestrina_path))
    masses = [os.path.join(palestrina_path, m) for m in os.listdir(palestrina_path)]
    #print(len(masses))

    #with Pool(2) as p:
    #    response = [occ for sublst in p.starmap(search, zip([pattern_path] * len(masses), masses)) for occ in sublst]
    masses = tqdm.tqdm(masses)
    response = [occ for sublst in [search(pattern_str, mass) for mass in masses] for occ in sublst]

    return response or []

def filter_threshold(result, query_length, threshold):
    if threshold == Threshold.ALL.value:
      return len(result['targetNotes']) == query_length
    else:
      return True

def filter_window(result, window):
    notes = result['targetNotes']
    return sum(r - l <= window for l, r in zip(notes, notes[1:])) == len(notes) - 1

def filter_transposition(result, transposition):
    if transposition == Transposition.ALL.value:
        return True
    elif transposition == Transposition.NONE.value:
        return result['transposition'] == 0
    elif transposition == Transposition.OCTAVE.value:
        return result['transposition'] % 12 == 0
    else:
        raise Exception("Unsupported transposition type")

def filter_diatonic(result, diatonic):
    """
    If diatonic == True, filter only for diatonic occurrences
    Otherwise return both chromatic and diatonic occurrences
    """
    return result['diatonicOcc'] if diatonic else True

def rank(result):
    notes = result['targetNotes']
    # Penalize for more note skips
    # Add exponential points for length
    return sum(r - l for l, r in zip(notes, notes[1:])) - pow(len(notes), 2)

def rank_results(results):
    for r in results:
      r['rank'] = rank(r)
    return sorted(results, key=lambda r: r['rank'])

def filter_results(results, threshold, query_length, transposition):
    filtered_results = [r for r in results if filter_threshold(r, query_length, threshold) and filter_transposition(r, transposition)]

def paginate(results, page_length):
    num_pages = int(math.ceil(len(results) / float(page_length)))
    response = {
        'count': len(results),
        'pages': [{
                'pageNum': i,
                'occurrences': results[i * page_length : i * page_length + page_length]}
            for i in range(num_pages)]
    }
    return response

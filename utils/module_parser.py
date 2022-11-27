import os
import sys
import json
import re
import glob


def parse_module(file_name):
    retval = {}
    module_name = re.search(r'\w+(?=\.cpp)', file_name).group(0)
    retval['module_name'] = module_name
    params = {}
    f = open(file_name, 'r')
    for line in f:
        match = re.search(r'config_.setDefault.+"(.+)"\w*,\w*(.+(?=\)))', line)
        if match:
            key = match.group(1)
            default_val = match.group(2)
            params[key] = default_val

    retval['parameters'] = params
    return retval


directory = sys.argv[1]
output_file = sys.argv[2]
out = open(output_file, 'w')
out.write('[')
modules = {}
globalConfig = {"module_name": "Corryvreckan", "parameters": {"log_level": "INFO", "log_format": "",
                                                          "purge_output_directory": "false",
                                                          "detectors_file": "detectors.geo",
                                                          "detectors_file_updated": "detectors_updated.geo",
                                                          "histogram_file": "out.root",
                                                          "number_of_tracks": "-1",
                                                          "number_of_events": "-1"
                                                          }}
out.write(json.dumps(globalConfig))
out.write('\n\n')
first = True
files = glob.glob(os.path.join(directory, '**/*.cpp'))
for filename in files:
    if first:
        first = False
    else:
        out.write(' ,\n\n')
    if os.path.isfile(filename):
        print('processing ', filename)
        module = parse_module(filename)
        description = json.dumps(module)
        out.write(description)

out.write(']')

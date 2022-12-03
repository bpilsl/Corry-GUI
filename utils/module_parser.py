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
        match_default = re.search(r'config_.setDefault.+"(.+)"\w*,\w*(.+(?=\)))', line)
        if match_default:
            key = match_default.group(1)
            default_val = match_default.group(2)
            match_unit = re.search(r'Unit.+\((.+),.*"(.+)"', default_val)
            if match_unit:
                params[key] = {'value': match_unit.group(1), 'unit': match_unit.group(2)}
            else:
                params[key] = default_val
            # there are config keys without default values
            # we also need to capture those
            # defaulted parameters have priority though
        match_no_default = re.search(r'config_\.get.+\("(.+?)"', line)
        if match_no_default:
            if not params.keys().__contains__(match_no_default.group(1)):
                params[match_no_default.group(1)] = ''  # no default value

    retval['parameters'] = params
    return retval


directory = sys.argv[1]
output_file = sys.argv[2]
out = open(output_file, 'w')
modules = []
globalConfig = {"module_name": "Corryvreckan", "parameters": {"log_level": "INFO", "log_format": "",
                                                              "purge_output_directory": "false",
                                                              "detectors_file": "detectors.geo",
                                                              "detectors_file_updated": "detectors_updated.geo",
                                                              "histogram_file": "out.root",
                                                              "number_of_tracks": "-1",
                                                              "number_of_events": "-1"
                                                              }}
modules.append(globalConfig)
files = glob.glob(os.path.join(directory, '**/*.cpp'))
for filename in files:
    if os.path.isfile(filename):
        print('processing ', filename)
        module = parse_module(filename)
        modules.append(module)

out.write(json.dumps(modules, indent=4))
